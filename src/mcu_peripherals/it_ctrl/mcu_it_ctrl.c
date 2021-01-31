#include "mcu_it_ctrl_priv.h"


//--------------------------------------------------------------------------------------------------
void mcu_irq_init(void)
{
  // Configuration: NVIC_PriorityGroup_1
  //
  // Interrupts grouped in 2 groups:  IMPORTANT (PreemptionPriority 0)
  //                                  USUAL     (PreemptionPriority 1)
  //
  // Inside of each group interrupts can have 7 subpriorities
  // (ability to interrupt other interrupt handlers from same group, but with lower subpriority)
  NVIC_SetPriorityGrouping(NVIC_PriorityGroup_1);

  //config_irq(ONEWIRE_TIMER_IRQn,           IRQ_GPOUR_IMPORTANT,  2);                     // onewire timer
  //config_irq(ONEWIRE_EXTI_IRQn,          IRQ_GPOUR_IMPORTANT,  3);                     // onewire sensor on EXTI line 2

  //config_irq(RTC_IRQn,            IRQ_GROUP_USUAL, 1);                            // RTC alarm
  //config_irq(USART3_IRQn,         IRQ_GROUP_USUAL, 2);                            // esp8266 communication line
  //config_irq(DMA1_Channel3_IRQn,  IRQ_GROUP_USUAL, 2);                            // MCU transmit to esp8266
  //config_irq(I2C1_EV_IRQn,        IRQ_GROUP_USUAL, 3);                            // i2c sensors
  config_irq(ADC1_2_IRQn,         IRQ_GROUP_USUAL, 4);                              // analog signals measurement
  config_irq(TIM3_IRQn,           IRQ_GROUP_USUAL, 4);                              // pwm control
  config_irq(EXTI15_10_IRQn,      IRQ_GROUP_USUAL, 5);                              // encoder signals
}


//--------------------------------------------------------------------------------------------------
static void config_irq(IRQn_Type irq, uint8_t preempt_prio, uint8_t sub_prio)
{
  NVIC_InitTypeDef IT_Config;

  IT_Config.NVIC_IRQChannel                    = irq;
  IT_Config.NVIC_IRQChannelPreemptionPriority  = preempt_prio;
  IT_Config.NVIC_IRQChannelSubPriority         = sub_prio;
  IT_Config.NVIC_IRQChannelCmd                 = ENABLE;

  NVIC_Init(&IT_Config);
}

