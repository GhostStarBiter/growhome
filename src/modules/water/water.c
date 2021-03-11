#include "water_priv.h"

water_t water;

//--------------------------------------------------------------------------------------------------
void water_init(void)
{
  (void) water_get_level();
  water_set_pump_power(WATER_PUMP_DEFAULT_POWER);
  water_pump_set_status(DISABLE);
}


//--------------------------------------------------------------------------------------------------
uint8_t water_get_level(void)
{
  uint16_t raw_adc_measure;

  raw_adc_measure = mcu_adc_get_raw_data_channel_water_tank();
  water.level = raw_adc_measure*99/ADC_MAX_OUTPUT_RAW_VALUE; // in percents

  return water.level;
}


//--------------------------------------------------------------------------------------------------
void water_pump_set_status(FunctionalState set_state)
{
  uint8_t tmp = 0;

  if(set_state && water.level > WATER_TANK_MINIMAL_LEVEL)
  {
    tmp = water.pump_power;
  }
  else
  {
    tmp = 0;
  }

  mcu_pwm_timer_set_channel_pulse_width(WATER_PUMP, tmp);
}

//--------------------------------------------------------------------------------------------------
void water_set_pump_power(uint8_t power_percents)
{
  if(power_percents > 100)
  {
    power_percents = 100;
  }

  water.pump_power = power_percents;
}


//--------------------------------------------------------------------------------------------------
uint8_t water_get_pump_power(void)
{
  return water.pump_power;
}
