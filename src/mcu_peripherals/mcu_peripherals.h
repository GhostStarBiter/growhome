#ifndef MCU_PERIPHERALS_H
#define MCU_PERIPHERALS_H

#include "adc/mcu_adc.h"
#include "gpio/mcu_gpio.h"
#include "clock/mcu_clock.h"
#include "tim/mcu_tim.h"
#include "it_ctrl/mcu_it_ctrl.h"
#include "uart/mcu_uart.h"
#include "rtc/mcu_rtc.h"
#include "i2c/mcu_i2c.h"


//--------------------------------------------------------------------------------------------------
/// @brief Init function of STM32F103 MCU peripherals (GPIO, IRQ, TIM, UART etc.)
//--------------------------------------------------------------------------------------------------
void mcu_init_peripherals(void);

#endif // MCU_PERIPHERALS_MCU_PERIPHERALS_H
