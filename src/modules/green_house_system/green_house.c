#include "green_house_priv.h"

volatile green_house_t growbox;

uint8_t u8_income_air_measurements_buffer[MEASUREMENTS_BUFFER_SIZE];
uint8_t u8_mixed_air_measurements_buffer[MEASUREMENTS_BUFFER_SIZE];
uint8_t u8_humidity_measurements_buffer[MEASUREMENTS_BUFFER_SIZE];
uint8_t u8_water_level_measurements_buffer[WATER_TANK_LEVEL_MEAS_BUFF_SIZE];


//--------------------------------------------------------------------------------------------------
void growbox_system_init(void)
{
  onewire_init_t  onewire_bus_init_struct;
  servo_init_t    servo_init_struct;
  mcu_time_t      init_time;

  // ***
  growbox.mode                            = CONTROL_MODE_AUTOMATIC;

  // Init Humidity values filter parameters
  growbox.humidity.measurement_buffer     = u8_humidity_measurements_buffer;
  growbox.humidity.u8_window_size         = MEASUREMENTS_BUFFER_SIZE;
  mean_filter_init( (filter_object_t*) &growbox.humidity);

  // Init Income air temperature value filter's parameters
  growbox.income_air_temp.measurement_buffer  = u8_income_air_measurements_buffer;
  growbox.income_air_temp.u8_window_size      = MEASUREMENTS_BUFFER_SIZE;
  mean_filter_init( (filter_object_t*) &growbox.income_air_temp);

  // Init Mixed air temperature value filter's parameters
  growbox.mixed_air_temp.measurement_buffer = u8_mixed_air_measurements_buffer;
  growbox.mixed_air_temp.u8_window_size     = MEASUREMENTS_BUFFER_SIZE;
  mean_filter_init( (filter_object_t*) &growbox.mixed_air_temp);

  // Init Water level value filter's parameters
  growbox.water_level.measurement_buffer  = u8_water_level_measurements_buffer;
  growbox.water_level.u8_window_size      = WATER_TANK_LEVEL_MEAS_BUFF_SIZE;
  mean_filter_init( (filter_object_t*) &growbox.water_level);

  growbox.temperature.pi_controler.kp = AIR_TEMP_PI_CTRL_KP;
  growbox.temperature.pi_controler.ki = AIR_TEMP_PI_CTRL_KI;
  growbox.temperature.pi_controler.saturation_max = 100;
  growbox.temperature.pi_controler.saturation_min = 0;
  pi_ctrl_init( (pi_ctrl_object_t*) &growbox.temperature.pi_controler);

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
  growbox_set_temperature(GROWBOX_DEFAULT_TEMPERATURE);

  // ***
  growbox.heater.duty_ms = 0;
  growbox.heater.cycle_counter_ms = 0;

  growbox.air_mix_time_counter = 0;

  // ***
  water_init();

  // ***
  init_time.hour  = 6;
  init_time.min   = 59;
  init_time.sec   = 50;
  mcu_rtc_set_time(init_time);

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

  growbox_system_init();

  x_last_wake_time = xTaskGetTickCount();

  for(;;)
  {
    // ***
    vTaskDelayUntil(&x_last_wake_time, x_growbox_task_timeout_ms);

    // ***
    growbox_update_measurements();

    // ***
    growbox_control();
  }

}

//--------------------------------------------------------------------------------------------------
void growbox_set_temperature(uint8_t set_temperature)
{
  growbox.temperature.desired = set_temperature;
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
uint8_t growbox_get_mixed_air_temp(void)
{
  return (uint8_t) growbox.mixed_air_temp.filtered;
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

  switch(growbox.mode)
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
        growbox_set_control_mode(CONTROL_MODE_AUTOMATIC);
        once_action = false;
      }
    break;

    case CONTROL_MODE_AUTOMATIC:
      schedule_update(  (FunctionalState*) &growbox.light_status,
                        (FunctionalState*) &growbox.water_pump_status);

      // ***
      growbox_control_air_mix();

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
  uint8_t income_air_intensity = 0;
  temp_control_state_t curr_temp_ctrl_state = NONE;
  bool start_state_change_timeout = false;

  static uint16_t state_change_timeout = 1000;

  growbox.temperature.pi_controler.input_data = growbox.mixed_air_temp.filtered;
  growbox.temperature.pi_controler.ref_value = growbox.temperature.desired;

  // ***
  pi_ctrl_run( (pi_ctrl_object_t*) &growbox.temperature.pi_controler);


  // ***
  growbox.temperature.delta = growbox.temperature.desired - growbox.mixed_air_temp.filtered;

  if(growbox.temperature.delta > AIR_REGULATION_TOLERANCE_DEGREES)
  {
    curr_temp_ctrl_state = HEAT;
  }
  else if(growbox.temperature.delta < (-AIR_REGULATION_TOLERANCE_DEGREES))
  {
    curr_temp_ctrl_state = COOLDOWN;
  }
  else
  {
    curr_temp_ctrl_state = IN_RANGE;
  }

  if(curr_temp_ctrl_state != growbox.temperature.state)
  {
    start_state_change_timeout = true;
  }

  if(start_state_change_timeout && state_change_timeout > 0)
  {
    state_change_timeout--;
  }
  else
  {
    growbox.temperature.state = curr_temp_ctrl_state;
    state_change_timeout = 1000;
  }

  switch(growbox.temperature.state)
  {
    case HEAT:
      growbox_set_air_mixing_status(ENABLE);
      servo_set_angle(SERVO_AIR_EXCHANGE_ANGLE);
      // set income air ventilator power
      income_air_intensity = 100;
      growbox_set_income_air_intensity(income_air_intensity);

      if( growbox.heater.status == DISABLE ||
          growbox.heater.cycle_counter_ms == AIR_HEATER_CYCLE_TIME
      )
      {
        growbox.heater.duty_ms = (AIR_HEATER_CYCLE_TIME * growbox.temperature.pi_controler.output) / growbox.temperature.pi_controler.saturation_max;
      }
    break;

    case COOLDOWN:
      growbox_set_air_mixing_status(ENABLE);
      // turn off the heater
      growbox_set_heater_status(DISABLE);
      // wide open air outlet
      servo_set_angle(SERVO_AIR_OUTLET_ANGLE);
      // set income air ventilator power
      income_air_intensity = 100;
      growbox_set_income_air_intensity(income_air_intensity);
    break;

    case IN_RANGE:
      growbox_set_heater_status(DISABLE);
      growbox_set_income_air_intensity(0);
      servo_set_angle(SERVO_AIR_OUTLET_CLOSED);
    break;

    default:
    break;
  }

  if(growbox.heater.cycle_counter_ms > 0)
  {
    growbox.heater.cycle_counter_ms--;

    if(growbox.heater.duty_ms > 0)
    {
      growbox.heater.duty_ms--;
      growbox_set_heater_status(ENABLE);
    }
    else
    {
      growbox_set_heater_status(DISABLE);
    }
  }
  else
  {
    growbox.heater.cycle_counter_ms = AIR_HEATER_CYCLE_TIME;
  }


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
  uint16_t measured = 0;

  // ***
  // Inside green house air temp measurement update
  if(APPLICATION_USE_AM2301_TEMP)
  {
    // ***
    onewire_measure();
    // Inside air humidity measurement update
    measured = onewire_get_humidity();
    mean_filter_update( (filter_object_t*) &growbox.humidity, measured);
    // AM2301 (onewire) sensor
    measured = onewire_get_temperature();
  }
  else
  {
    // LM45 (analog) sensor
    measured = mcu_adc_get_raw_data_channel_temp_1();

    measured = (uint8_t) ((RAW_ADC_TO_MV(measured) - LM60_ZERO_DEGREES_OFFSET_CONVERTED)/LM60_TEMP_SENSOR_OPAMP_MV_PER_DEG);
  }
  mean_filter_update( (filter_object_t*)  &growbox.mixed_air_temp, measured);

  // ***
  // Input air temperature measurement update
  measured = mcu_adc_get_raw_data_channel_temp_2();
  measured = (uint8_t) (RAW_ADC_TO_MV(measured)/LM60_TEMP_SENSOR_OPAMP_MV_PER_DEG);
  mean_filter_update( (filter_object_t*) &growbox.income_air_temp, measured);

  // ***
  // Water tank level measurement update
  measured = water_get_level();
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
static void growbox_control_air_mix(void)
{

  if(growbox.air_mix_time_counter < GROWBOX_AIR_MIX_ACTIVE_TIMEOUT)
  {
    growbox_set_air_mixing_status(ENABLE);
  }
  else
  {
    growbox_set_air_mixing_status(DISABLE);
  }

  if(growbox.air_mix_time_counter < GROWBOX_AIR_MIX_CYCLE_TIMEOUT)
  {
    growbox.air_mix_time_counter++;
  }
  else
  {
    growbox.air_mix_time_counter = 0;
  }
}



//  *** END OF FILE ***
