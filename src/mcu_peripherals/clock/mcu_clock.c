#include "mcu_clock_priv.h"


//--------------------------------------------------------------------------------------------------
void mcu_clock_init(void)
{
  // APB2 72 MHz
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA |
                          RCC_APB2Periph_GPIOB |
                          RCC_APB2Periph_GPIOC |
                          RCC_APB2Periph_TIM1 |       // triac control timer
                          RCC_APB2Periph_ADC1 |
                          RCC_APB2Periph_USART1,
                          ENABLE);

  /* Enable AFIO clock (for EXTI)*/
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
  // 72 MHz / 6 = 12 MHz  ADC peripherals bus clock frequency
  RCC_ADCCLKConfig(RCC_PCLK2_Div6);

  /* PCLK1 = HCLK/4 */
  RCC_PCLK1Config(RCC_HCLK_Div4); // ????
  // APB1 36 MHz
  RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM2 |
                          RCC_APB1Periph_TIM3 |
                          RCC_APB1Periph_I2C1, ENABLE);

  //RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
}
