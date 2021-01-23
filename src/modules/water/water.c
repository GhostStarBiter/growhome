#include "water_priv.h"

water_t water;

//--------------------------------------------------------------------------------------------------
void water_init(void)
{
  water.pump_power = WATER_PUMP_INITIAL_POWER;
  water_pump_set_status(DISABLE);
}


//--------------------------------------------------------------------------------------------------
uint8_t water_get_level(void)
{
  uint16_t raw_adc_measure;

  raw_adc_measure = mcu_adc_get_raw_data_channel_water_tank();
  water.level = raw_adc_measure*100/ADC_MAX_OUTPUT_RAW_VALUE; // in percents

  return water.level;
}


//--------------------------------------------------------------------------------------------------
void water_pump_set_status(FunctionalState pump_state)
{
  water.pump_status = pump_state;

  switch(pump_state)
  {
    case ENABLE:
      mcu_pwm_timer_set_channel_pulse_width(WATER_PUMP, water.pump_power);
    break;

    case DISABLE:
    default:
      mcu_pwm_timer_set_channel_pulse_width(WATER_PUMP, 0);
    break;
  }
}
