#include "triac_priv.h"

triac_t triac_heater;


//--------------------------------------------------------------------------------------------------
void triac_heater_init(triac_init_t* init_struct)
{
  triac_heater.timer                = init_struct->timer;
  triac_heater.zerocross_gpio_port  = init_struct->zerocross_gpio_port;
  triac_heater.zerocross_gpio_pin   = init_struct->zerocross_gpio_pin;
  triac_heater.zerocross_exti_line  = init_struct->zerocross_exti_line;
  triac_heater.set_power  = 0;
  triac_set_heater_power(0);
}


//--------------------------------------------------------------------------------------------------
void triac_heater_set_status(FunctionalState triac_heater_status)
{
  triac_heater.status = triac_heater_status;

  if(triac_heater_status)
  {
    // enable Capture-Compare channel
    triac_heater.timer->CCER |= (1 << TIM_Channel_1);
    // enable timer
    triac_heater.timer->CR1 |= TIM_CR1_CEN;
  }
  else
  {
    // disable Capture-Compare channel
    triac_heater.timer->CCER &= ~(1 << TIM_Channel_1);
    // disable timer
    triac_heater.timer->CR1 &= ~(TIM_CR1_CEN);
  }
}


//--------------------------------------------------------------------------------------------------
void triac_set_heater_power(uint16_t set_power_percents)
{
  if(set_power_percents > 100)
  {
    set_power_percents = 100;
  }

  triac_heater.set_power = set_power_percents;

  if(set_power_percents > 0)
  {
    triac_heater.timer->CCR1 = triac_heater_convert_power(set_power_percents);
    triac_heater.timer->CCER |= (1 << TIM_Channel_1);

  }
  else
  {
    triac_heater.timer->CCER &= ~(1 << TIM_Channel_1);
  }
}


//--------------------------------------------------------------------------------------------------
static uint32_t triac_heater_convert_power(uint8_t power_in_percents)
{
  return (triac_heater.timer->ARR - (triac_heater.timer->ARR*power_in_percents)/100);
}


//--------------------------------------------------------------------------------------------------
void MAINS_ZEROCROSS_EXTI_IRQHandler(void)
{
  if( EXTI_GetITStatus(triac_heater.zerocross_exti_line) != RESET &&
      GPIO_ReadInputDataBit(triac_heater.zerocross_gpio_port, triac_heater.zerocross_gpio_pin)
  )
  {
    triac_heater.timer->CNT = 0;
  }

  EXTI_ClearITPendingBit(triac_heater.zerocross_exti_line);
}

//  *** END OF FILE ***
