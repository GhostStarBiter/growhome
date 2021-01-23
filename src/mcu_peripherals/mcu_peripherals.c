#include "mcu_peripherals.h"


//--------------------------------------------------------------------------------------------------
void mcu_init_peripherals(void)
{
  mcu_clock_init();
  mcu_gpio_init();
  mcu_adc_init();
  mcu_uart_init();
  mcu_tim_init();
  //mcu_rtc_init();
  //mcu_i2c_init();

  mcu_irq_init();
}
