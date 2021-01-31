#include "green_house_priv.h"

green_house_t growbox;

uint8_t u8_income_air_measurements_buffer[MEASUREMENTS_BUFFER_SIZE];
uint8_t u8_mixed_air_measurements_buffer[MEASUREMENTS_BUFFER_SIZE];
uint8_t u8_humidity_measurements_buffer[MEASUREMENTS_BUFFER_SIZE];
uint8_t u8_water_level_measurements_buffer[MEASUREMENTS_BUFFER_SIZE];


//--------------------------------------------------------------------------------------------------
void growbox_system_init(void)
{
  onewire_init_t  onewire_bus_init_struct;
  triac_init_t    triac_heater_init_struct;
  servo_init_t    servo_init_struct;

  // ***
  growbox.mode                            = GROW_MODE_AUTOMATIC;

  // Init Humidity values filter parameters
  growbox.humidity.pu8_buffer             = u8_humidity_measurements_buffer;
  growbox.humidity.u8_window_size         = MEASUREMENTS_BUFFER_SIZE;
  mean_filter_init(&growbox.humidity);

  // Init Income air temperature value filter's parameters
  growbox.income_air_temp.pu8_buffer      = u8_income_air_measurements_buffer;
  growbox.income_air_temp.u8_window_size  = MEASUREMENTS_BUFFER_SIZE;
  mean_filter_init(&growbox.income_air_temp);

  // Init Mixed air temperature value filter's parameters
  growbox.mixed_air_temp.pu8_buffer       = u8_mixed_air_measurements_buffer;
  growbox.mixed_air_temp.u8_window_size   = MEASUREMENTS_BUFFER_SIZE;
  mean_filter_init(&growbox.mixed_air_temp);

  // Init Water level value filter's parameters
  growbox.water_level.pu8_buffer          = u8_water_level_measurements_buffer;
  growbox.water_level.u8_window_size      = MEASUREMENTS_BUFFER_SIZE;
  mean_filter_init(&growbox.water_level);

  // ***
  onewire_bus_init_struct.timer        = ONEWIRE_TIMER;
  onewire_bus_init_struct.gpio_port    = ONEWIRE_PORT;
  onewire_bus_init_struct.gpio_pin     = ONEWIRE_PIN;
  onewire_bus_init_struct.EXTIchannel  = ONEWIRE_EXTI_IRQn;
  onewire_bus_init_struct.EXTI_line    = ONEWIRE_EXTI_LINE;
  onewire_init(&onewire_bus_init_struct);

  // ***
  servo_init_struct.timer          = SERVO_TIMER;
  servo_init_struct.initial_angle  = SERVO_AIR_OUTLET_CLOSED;
  servo_init(&servo_init_struct);

  // ***
  growbox_set_light(DISABLE);

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
  const TickType_t  x_growbox_task_timeout_ms = GROWBOX_TASK_TIMEOUT;
  TickType_t        x_last_wake_time;
  uint16_t          timeout = 10;
  uint16_t          ms_counter = 0;

  growbox_system_init();

  x_last_wake_time            = xTaskGetTickCount();

  for(;;)
  {
    // ***
    vTaskDelayUntil(&x_last_wake_time, x_growbox_task_timeout_ms);

//    // ***
//    growbox_update_measurements();
//
//    // ***
//    growbox_control();
  }

}

//--------------------------------------------------------------------------------------------------
void growbox_set_temperature(uint8_t set_temperature)
{
  growbox.desired_air_temp = set_temperature;
}


//--------------------------------------------------------------------------------------------------
static void growbox_control(void)
{
  if(GROW_MODE_AUTOMATIC == growbox.mode)
  {
    schedule_update(  &growbox.light_status,
                      &growbox.water_pump_status);
  }

  // ***
  growbox_control_temperature();

  // ***
  growbox_control_water_supply();

  // ***
  growbox_control_light();

  //growbox_control_communication();
}


//--------------------------------------------------------------------------------------------------
static void growbox_control_temperature(void)
{
  uint8_t income_air_intensity = 0;

  growbox.delta_temp = growbox.desired_air_temp - growbox.mixed_air_temp.filtered;


  // AIR HEAT
  if(growbox.delta_temp > AIR_REGULATION_TOLERANCE_DEGREES)
  {
    growbox_set_air_mixing_status(ENABLE);
    servo_set_angle(SERVO_AIR_EXCHANGE_ANGLE);

    // set income air ventilator power
    income_air_intensity = abs(growbox.delta_temp) * 10;                        // 10 - magic number @todo: define coefficient
    growbox_set_income_air_intensity(income_air_intensity);

    // run PID regulator to set heater power

    // turn on the heater
    growbox.air_heater_power = 25;                                              // @todo: magic number. TESTING PURPOSES ONLY!
    triac_set_heater_power(growbox.air_heater_power);
  }
  // AIR COOL DOWN
  else if(growbox.delta_temp < (-AIR_REGULATION_TOLERANCE_DEGREES))
  {
    growbox_set_air_mixing_status(ENABLE);
    // turn off the heater
    triac_set_heater_power(0);

    // wide open air outlet
    servo_set_angle(SERVO_AIR_OUTLET_ANGLE);

    // set income air ventilator power
    income_air_intensity = abs(growbox.delta_temp) * 10;                        // 10 - magic number @todo: define coefficient
    growbox_set_income_air_intensity(income_air_intensity);
  }
  // NO ACTION NEEDED
  else
  {
    // turn off the heater
    triac_set_heater_power(0);
    servo_set_angle(SERVO_AIR_OUTLET_CLOSED);
    growbox_set_air_mixing_status(DISABLE);
  }

}


//--------------------------------------------------------------------------------------------------
static void growbox_control_water_supply(void)
{
  if(GROW_MODE_AUTOMATIC == growbox.mode)
  {
    water_pump_set_status(growbox.water_pump_status);
  }
}


//--------------------------------------------------------------------------------------------------
static void growbox_set_income_air_intensity(uint8_t intensity_percents)
{
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
  if(GROW_MODE_AUTOMATIC == growbox.mode)
  {
    growbox_set_light(growbox.light_status);
  }
}


//--------------------------------------------------------------------------------------------------
void growbox_set_light(FunctionalState light_state)
{
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
  growbox.mode = set_control_mode;
}


//--------------------------------------------------------------------------------------------------
static void growbox_update_measurements(void)
{
  uint16_t measured = 0;

  onewire_measure();

  // ***
  // Inside air humidity measurement update
  measured = onewire_get_humidity();
  mean_filter_update(&growbox.humidity, measured);


  // ***
  // Inside green house air temp measurement update
  if(APPLICATION_USE_AM2301_TEMP)
  {
    // AM2301 (onewire) sensor
    measured = onewire_get_temperature();
  }
  else
  {
    // LM45 (analog) sensor
    measured = mcu_adc_get_raw_data_channel_temp_1();

    measured = (uint8_t) ((RAW_ADC_TO_MV(measured) - LM60_ZERO_DEGREES_OFFSET_CONVERTED)/LM60_TEMP_SENSOR_OPAMP_MV_PER_DEG);
  }
  mean_filter_update(&growbox.mixed_air_temp, measured);

  // ***
  // Input air temperature measurement update
  measured = mcu_adc_get_raw_data_channel_temp_2();
  measured = (uint8_t) (RAW_ADC_TO_MV(measured)/LM60_TEMP_SENSOR_OPAMP_MV_PER_DEG);
  mean_filter_update(&growbox.income_air_temp, measured);

  // ***
  // Water tank level measurement update
  measured = water_get_level();
  mean_filter_update(&growbox.water_level, measured);

}


//--------------------------------------------------------------------------------------------------
uint8_t growbox_get_mixed_air_temp(void)
{
  return (uint8_t) growbox.mixed_air_temp.filtered;
}


//--------------------------------------------------------------------------------------------------
uint8_t growbox_get_water_level(void)
{
  return (uint8_t) growbox.water_level.filtered;
}



//  *** END OF FILE ***
