#ifndef MCU_GPIO_PRIVATE_H
#define MCU_GPIO_PRIVATE_H

#include "stm32f10x_gpio.h"

#include "main/application_map.h"

#include "stm32f10x_exti.h"

#include "mcu_gpio.h"


//------------------------------------------------------------------------------
/// @brief Init of MCU pins for analog signal's measurement
//------------------------------------------------------------------------------
static void gpio_init_analog(void);


//------------------------------------------------------------------------------
/// @brief Init of MCU pins for LCD 2x16 (HD44780)
//------------------------------------------------------------------------------
static void gpio_init_LCD216(void);


//------------------------------------------------------------------------------
/// @brief Init of MCU pins for triac control
//------------------------------------------------------------------------------
static void gpio_init_triac(void);


//------------------------------------------------------------------------------
/// @brief Init of MCU pins for servo motor control
//------------------------------------------------------------------------------
static void gpio_init_servo(void);


//------------------------------------------------------------------------------
/// @brief Init of MCU pins for DC elements (mostly PWM)
//------------------------------------------------------------------------------
static void gpio_init_pwm(void);


//------------------------------------------------------------------------------
/// @brief Init of MCU pins for communication interfaces
//------------------------------------------------------------------------------
static void gpio_init_communication(void);


//------------------------------------------------------------------------------
/// @brief Init of MCU pins responsible for external interrupts
//------------------------------------------------------------------------------
static void gpio_init_EXTI(void);


//------------------------------------------------------------------------------
/// @brief Init of MCU pins miscelaneous use
//------------------------------------------------------------------------------
static void gpio_init_misc(void);


#endif // MCU_GPIO_PRIVATE_H
