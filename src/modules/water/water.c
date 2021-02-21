#include "water_priv.h"

water_t water;

//--------------------------------------------------------------------------------------------------
void water_init(void)
{
  (void) water_get_level();
  water_set_pump_power(0);
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
void water_set_pump_power(uint8_t power_percents)
{
  if(power_percents > 100)
  {
    power_percents = 100;
  }

  water.pump_power = power_percents;

  mcu_pwm_timer_set_channel_pulse_width(WATER_PUMP, water.pump_power);
}
