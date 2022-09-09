#include "mcu_peripherals.h"


//--------------------------------------------------------------------------------------------------
void mcu_init_peripherals(void)
{
  mcu_clock_init();
  mcu_gpio_init();
  mcu_adc_init();
  // TEMPORARY SOLUTION
  // @todo Inside mcu_uart_init move ESP UART init to separate function.
#if GRWHS_USE_NETWORK
  mcu_uart_init();
#endif
  mcu_tim_init();
  mcu_rtc_init();
  mcu_i2c_init();

  mcu_irq_init();
}
