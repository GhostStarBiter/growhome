#include <stdlib.h>

#include "stm32f10x.h"

// FreeRTOS includes
#include "task.h"
#include "timers.h"

#include "configuration/peripherals_config.h"
#include "configuration/adc_config.h"
#include "configuration/app_config.h"
#include "configuration/task_config.h"
#include "configuration/servo_config.h"

// MCU includes
#include "mcu_peripherals/adc/mcu_adc.h"
#include "mcu_peripherals/gpio/mcu_gpio.h"
#include "mcu_peripherals/uart/mcu_uart.h"

// MODULES includes
#include "water/water.h"
#include "triac/triac.h"
#include "servo/servo.h"

#if APPLICATION_USE_ONEWIRE_SENSOR
  #include "onewire/onewire.h"
#endif

#include "schedule/schedule.h"
#include "filter/filter.h"
#include "pi_ctrl/pi_ctrl.h"

// this module
#include "green_house.h"

typedef enum {
  NONE,
  HEAT,
  COOLDOWN,
  IN_RANGE
} temp_control_state_t;

typedef struct {
  FunctionalState status;                  // ON/OF
  uint16_t        duty_ms;
  uint16_t        cycle_counter_ms;
} heater_t;

typedef enum {
  SEED,
  SPROUT,
  PLANT,
  BLOOM
} grow_mode_t;


typedef struct {
  temp_control_state_t  state;
  uint8_t               set_point;
  int8_t                delta;                     // [-127 .. 127]
  pi_ctrl_object_t      pi_controler;
} temperature_t;


typedef struct{
    control_mode_t    control_mode;                           // MANUAL/AUTOMATIC
    heater_t          heater;
    temperature_t     temperature;

    filter_object_t   water_level;                    // 0 .. 100 %
    filter_object_t   mixed_air_temp;
    filter_object_t   income_air_temp;
    filter_object_t   humidity;

    double income_air_measurements_buffer [MEASUREMENTS_BUFFER_SIZE];
    double mixed_air_measurements_buffer  [MEASUREMENTS_BUFFER_SIZE];
    double humidity_measurements_buffer   [MEASUREMENTS_BUFFER_SIZE];
    double water_level_measurements_buffer[WATER_TANK_LEVEL_MEAS_BUFF_SIZE];

    FunctionalState   light_status;                   // ON/OFF
    FunctionalState   water_pump_status;              // ON/OFF
    FunctionalState   air_income_status;              // ON/OFF
    FunctionalState   air_outlet_valve_status;        // OPEN/CLOSED
    FunctionalState   air_mix_status;                 // ON/OFF

    uint16_t          manual_mode_timeout;
    uint32_t          air_mix_time_counter;

    uint8_t           desired_humidity;
    int8_t            delta_humidity;                 // [-127 .. 127]

} green_house_t;


//--------------------------------------------------------------------------------------------------
/// @brief Update measurements of temperatures, humidity, water level
//--------------------------------------------------------------------------------------------------
static void growbox_update_measurements(void);


//--------------------------------------------------------------------------------------------------
/// @brief Update system run-time data (temp deviation, heater work-time etc.)
//--------------------------------------------------------------------------------------------------
static void growbox_update_statistics(void);


//--------------------------------------------------------------------------------------------------
/// @brief  Growbox parameters (temp, water, light) control function
//--------------------------------------------------------------------------------------------------
static void growbox_control(void);


//--------------------------------------------------------------------------------------------------
/// @brief  Temperature control function
//--------------------------------------------------------------------------------------------------
static void growbox_control_temperature(void);


//--------------------------------------------------------------------------------------------------
/// @brief  Water supply control function
//--------------------------------------------------------------------------------------------------
static void growbox_control_water_supply(void);


//--------------------------------------------------------------------------------------------------
/// @brief  Light control function
//--------------------------------------------------------------------------------------------------
static void growbox_control_light(void);


//--------------------------------------------------------------------------------------------------
/// @brief  Set Income air ventilator intensity
/// @param  Ventilator power to set in percents
//--------------------------------------------------------------------------------------------------
static void growbox_set_income_air_intensity
(
    uint8_t intensity_percents
);


//--------------------------------------------------------------------------------------------------
/// @brief  Set status of Air Mixing ventilator
/// @param  Air Mixing ventilator status (ENABLE/DISABLE)
//--------------------------------------------------------------------------------------------------
static void growbox_set_air_mixing_status
(
    FunctionalState e_air_mixing_status
);


//--------------------------------------------------------------------------------------------------
/// @brief  Set status of Air Heater
/// @param  Air Heater status (ENABLE/DISABLE)
//--------------------------------------------------------------------------------------------------
static void growbox_set_heater_status
(
    FunctionalState heater_state
);


//--------------------------------------------------------------------------------------------------
/// @brief  Run timeout handling for air mix ventilator control
//--------------------------------------------------------------------------------------------------
static void growbox_control_air_mix(bool temp_request);


volatile green_house_t growbox;




//--------------------------------------------------------------------------------------------------
void growbox_system_init(void)
{
  servo_init_t    servo_init_struct;

#if APPLICATION_USE_ONEWIRE_SENSOR
  onewire_init_t  onewire_bus_init_struct;
  onewire_bus_init_struct.timer        = ONEWIRE_TIMER;
  onewire_bus_init_struct.gpio_port    = ONEWIRE_PORT;
  onewire_bus_init_struct.gpio_pin     = ONEWIRE_PIN;
  onewire_bus_init_struct.EXTIchannel  = ONEWIRE_EXTI_IRQn;
  onewire_bus_init_struct.EXTI_line    = ONEWIRE_EXTI_LINE;
  onewire_init(&onewire_bus_init_struct);
#endif

  // ***
  growbox.control_mode                            = CONTROL_MODE_AUTOMATIC;

  // Init Humidity values filter parameters
#if APPLICATION_USE_AM2301_TEMP
  growbox.humidity.measurement_buffer     = humidity_measurements_buffer;
  growbox.humidity.window_size            = MEASUREMENTS_BUFFER_SIZE;
  mean_filter_init( (filter_object_t*) &growbox.humidity);
#endif

  // Init Income air temperature value filter's parameters
  growbox.income_air_temp.measurement_buffer  = (double*) growbox.income_air_measurements_buffer;
  growbox.income_air_temp.window_size      = MEASUREMENTS_BUFFER_SIZE;
  mean_filter_init( (filter_object_t*) &growbox.income_air_temp);

  // Init Mixed air temperature value filter's parameters
  growbox.mixed_air_temp.measurement_buffer = (double*) growbox.mixed_air_measurements_buffer;
  growbox.mixed_air_temp.window_size     = MEASUREMENTS_BUFFER_SIZE;
  mean_filter_init( (filter_object_t*) &growbox.mixed_air_temp);

  // Init Water level value filter's parameters
  growbox.water_level.measurement_buffer  = (double*) growbox.water_level_measurements_buffer;
  growbox.water_level.window_size      = WATER_TANK_LEVEL_MEAS_BUFF_SIZE;
  mean_filter_init( (filter_object_t*) &growbox.water_level);

  growbox.temperature.pi_controler.kp = AIR_TEMP_PI_CTRL_KP;
  growbox.temperature.pi_controler.ki = AIR_TEMP_PI_CTRL_KI;
  growbox.temperature.pi_controler.saturation_max = 100;
  growbox.temperature.pi_controler.saturation_min = (-100);
  growbox.temperature.pi_controler.sample_time_s = 2;
  pi_ctrl_init( (pi_ctrl_object_t*) &growbox.temperature.pi_controler);

  // ***
  servo_init_struct.timer          = SERVO_TIMER;
  servo_init_struct.initial_angle  = SERVO_AIR_OUTLET_CLOSED;
  servo_init(&servo_init_struct);

  // ***
  growbox_set_light(DISABLE);

  // ***
  growbox_set_temperature(GROWBOX_DEFAULT_TEMPERATURE);

  // ***
  growbox.heater.duty_ms = 0;
  growbox.heater.cycle_counter_ms = 0;

  growbox.air_mix_time_counter = 0;

  // ***
  water_init();

  // ***
  schedule_init();

  // ***
  mcu_adc_start();
}


//--------------------------------------------------------------------------------------------------
void growbox_control_rtos_task(void *pvParameters)
{
  const TickType_t  x_growbox_task_cycle = GROWBOX_TASK_CYCLE;
  TickType_t        x_last_wake_time;

  growbox_system_init();

  x_last_wake_time = xTaskGetTickCount();

  for(;;)
  {
    // ***
    vTaskDelayUntil(&x_last_wake_time, x_growbox_task_cycle);

    // ***
    growbox_update_measurements();

    // ***
    growbox_control();

    // ***
    growbox_update_statistics();
  }

}


//--------------------------------------------------------------------------------------------------
static void growbox_update_statistics(void)
{
  //
}

//--------------------------------------------------------------------------------------------------
void growbox_set_temperature(uint8_t set_temperature)
{
  growbox.temperature.set_point = set_temperature;
}


//--------------------------------------------------------------------------------------------------
void growbox_set_light(FunctionalState light_state)
{
  growbox.light_status = light_state;

  if(ENABLE == light_state)
  {
    mcu_gpio_set_light(ENABLE);
  }
  else
  {
    mcu_gpio_set_light(DISABLE);
  }
}


//--------------------------------------------------------------------------------------------------
FunctionalState growbox_get_light_status(void)
{
  return growbox.light_status;
}


//--------------------------------------------------------------------------------------------------
void growbox_set_control_mode(control_mode_t set_control_mode)
{
  growbox.control_mode = set_control_mode;
}


//--------------------------------------------------------------------------------------------------
uint8_t growbox_get_mixed_air_temp(void)
{
  return (uint8_t) (growbox.mixed_air_temp.filtered/LM_60_SW_AMPLIFICATION_RATIO);
}


//--------------------------------------------------------------------------------------------------
uint8_t growbox_get_water_level(void)
{
  return (uint8_t) growbox.water_level.filtered;
}


//--------------------------------------------------------------------------------------------------
static void growbox_control(void)
{
  static bool once_action = false;

  switch(growbox.control_mode)
  {
    case CONTROL_MODE_MANUAL:
      // in means of safety after system control mode changed to MANUAL
      //  disable Heater and water pump
      if(once_action == false)
      {
        growbox_set_heater_status(DISABLE);
        water_set_pump_power(0);
        growbox.manual_mode_timeout = 0;
        once_action = true;
      }

      // wait manual mode timeout
      if(growbox.manual_mode_timeout < GROWBOX_MANUAL_MODE_TIMEOUT)
      {
        growbox.manual_mode_timeout++;
      }
      else
      {
        once_action = false;
        growbox_set_control_mode(CONTROL_MODE_AUTOMATIC);
      }
    break;

    case CONTROL_MODE_AUTOMATIC:
      schedule_update(  (FunctionalState*) &growbox.light_status,
                        (FunctionalState*) &growbox.water_pump_status);

      // ***
      growbox_control_temperature();

      // ***
      growbox_control_water_supply();

      // ***
      growbox_control_light();
    break;

    case CONTROL_MODE_REMOTE:
      // to be implemented

    default:
      growbox_set_heater_status(DISABLE);
      water_set_pump_power(0);
    break;
  }
}


//--------------------------------------------------------------------------------------------------
static void growbox_control_temperature(void)
{
  static uint32_t heater_cycle_counter = 0;
  double regulator_output;

  // ***
  regulator_output = pi_ctrl_run((pi_ctrl_object_t*) &growbox.temperature.pi_controler,
                                    (double) growbox.temperature.set_point*LM_60_SW_AMPLIFICATION_RATIO,
                                    (double) growbox.mixed_air_temp.filtered);


  // difference between filtered and desired temp is more than 1 C (respect to LM_60_SW_AMPLIFICATION_RATIO)
  if(abs(growbox.mixed_air_temp.filtered - growbox.temperature.set_point) > 1*LM_60_SW_AMPLIFICATION_RATIO)
  {
    if(regulator_output > 5.0)
    {
      // to prevent heater overheat set maximum power of air input ventilator
      growbox_set_income_air_intensity(100);

      servo_set_angle(SERVO_AIR_OUTLET_CLOSED);

      growbox.temperature.state = HEAT;

      heater_cycle_counter++;
      growbox.heater.duty_ms = (uint16_t) (regulator_output*AIR_HEATER_CYCLE_TIME/growbox.temperature.pi_controler.saturation_max);

      if(heater_cycle_counter >= AIR_HEATER_CYCLE_TIME)
      {
        growbox_set_heater_status(ENABLE);
        heater_cycle_counter = 0;
      }

      if(heater_cycle_counter > growbox.heater.duty_ms)
      {
        growbox_set_heater_status(DISABLE);
      }
    }
    else if(regulator_output < -5.0f )
    {
      growbox.temperature.state = COOLDOWN;
      growbox_set_heater_status(DISABLE);
      servo_set_angle(SERVO_AIR_EXCHANGE_ANGLE);
      growbox_set_income_air_intensity(75);

    }
  }
  else
  {
    growbox_set_income_air_intensity(0);
    growbox_set_heater_status(DISABLE);
    servo_set_angle(SERVO_AIR_OUTLET_CLOSED);
  }

  servo_control();
  growbox_control_air_mix(growbox.heater.status);
}


//--------------------------------------------------------------------------------------------------
static void growbox_control_water_supply(void)
{
  if(growbox.water_pump_status == ENABLE)
  {
    water_pump_set_status(ENABLE);
  }
  else
  {
    water_pump_set_status(DISABLE);
  }
}


//--------------------------------------------------------------------------------------------------
static void growbox_set_income_air_intensity(uint8_t intensity_percents)
{
  if(intensity_percents < 20)
  {
    intensity_percents = 0;
  }

  mcu_pwm_timer_set_channel_pulse_width(AIR_INPUT, intensity_percents);
}


//--------------------------------------------------------------------------------------------------
static void growbox_set_air_mixing_status(FunctionalState e_air_mixing_status)
{
  mcu_gpio_set_air_mixing_status(e_air_mixing_status);
}


//--------------------------------------------------------------------------------------------------
static void growbox_control_light(void)
{
  // Light status (ON/OFF) will be changed (if necessary)
  //  during schedule_update(..)

  // The Light status will be changed only if system is in automatic mode
  //  (i.e. user don't change system parameters via User Interface)
  growbox_set_light(growbox.light_status);
}


//--------------------------------------------------------------------------------------------------
static void growbox_update_measurements(void)
{
  double measured = 0;

  // ***
  // Inside green house air temp measurement update
#if APPLICATION_USE_AM2301_TEMP
  {
    // ***
    onewire_measure();
    // Inside air humidity measurement update
    measured = onewire_get_humidity();
    mean_filter_update( (filter_object_t*) &growbox.humidity, measured);
    // AM2301 (onewire) sensor
    measured = onewire_get_temperature();
  }
#elif APPLICATION_USE_LM60_TEMP
  {
    // LM60 (analog) sensor
    // multiplication by 10 for results with precision of .1 C
    measured = (double) LM_60_SW_AMPLIFICATION_RATIO * mcu_adc_get_raw_data_channel_temp_1();

    measured = (double) ((RAW_ADC_TO_MV(measured) - LM60_ZERO_DEGREES_OFFSET_CONVERTED * LM_60_SW_AMPLIFICATION_RATIO)
                            /LM60_TEMP_SENSOR_OPAMP_MV_PER_DEG);
  }
#endif

  mean_filter_update( (filter_object_t*)  &growbox.mixed_air_temp, measured);
  growbox.temperature.delta = growbox.temperature.set_point - measured;


  // ***
  // Input air temperature measurement update
  measured = mcu_adc_get_raw_data_channel_temp_2();
  measured = (uint8_t) (RAW_ADC_TO_MV(measured)/LM60_TEMP_SENSOR_OPAMP_MV_PER_DEG);
  mean_filter_update( (filter_object_t*) &growbox.income_air_temp, measured);

  // ***
  // Water tank level measurement update
  measured = (double) water_get_level();
  mean_filter_update( (filter_object_t*) &growbox.water_level, measured);

}


//--------------------------------------------------------------------------------------------------
static void growbox_set_heater_status(FunctionalState heater_state)
{
  if(heater_state == ENABLE)
  {
    growbox.heater.status = ENABLE;
    mcu_gpio_set_heater_status(ENABLE);
  }
  else
  {
    growbox.heater.status = DISABLE;
    growbox.heater.duty_ms = 0;
    mcu_gpio_set_heater_status(DISABLE);
  }
}


//--------------------------------------------------------------------------------------------------
FunctionalState growbox_get_heater_status(void)
{
  return growbox.heater.status;
}


//--------------------------------------------------------------------------------------------------
static void growbox_control_air_mix(bool heater_status)
{
  if(heater_status)
  {
    growbox_set_air_mixing_status(ENABLE);
    growbox.air_mix_time_counter = 0;
  }
  else
  {
    growbox.air_mix_time_counter++;

    if(growbox.air_mix_time_counter < GROWBOX_AIR_MIX_ACTIVE_TIMEOUT)
    {
      growbox_set_air_mixing_status(ENABLE);
    }
    else
    {
      growbox_set_air_mixing_status(DISABLE);
    }

    if(growbox.air_mix_time_counter > GROWBOX_AIR_MIX_CYCLE_TIMEOUT)
    {
      growbox.air_mix_time_counter = 0;
    }
  }
}



//  *** END OF FILE ***
