#include "mcu_peripherals.h"
#include "configuration/app_config.h"


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
#if  GRWHS_USE_GY21_SENSOR
  mcu_i2c_init();
#endif
  mcu_spi_init();

  mcu_irq_init();
}
