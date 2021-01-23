#ifndef MCU_TIM_H
#define MCU_TIM_H

#include "stm32f10x_tim.h"
#include "main/application_map.h"

//------------------------------------------------------------------------------
/// @brief  Init function of STM32F103 timers
//------------------------------------------------------------------------------
void mcu_tim_init(void);


//------------------------------------------------------------------------------
/// @brief Getter of TRIAC timer instance
//------------------------------------------------------------------------------
TIM_TypeDef* mcu_timer_triac(void);


//------------------------------------------------------------------------------
/// @brief Getter of SERVO timer instance
//------------------------------------------------------------------------------
TIM_TypeDef* mcu_timer_servo(void);


//------------------------------------------------------------------------------
/// @brief Getter of ONEWIRE bus timer instance
//------------------------------------------------------------------------------
TIM_TypeDef* mcu_timer_onewire(void);


//------------------------------------------------------------------------------
/// @brief Getter of PWM timer instance
//------------------------------------------------------------------------------
TIM_TypeDef* mcu_pwm_timer(void);


//------------------------------------------------------------------------------
/// @brief  Set pulse width (in percents) for channel 1 of PWM timer
/// @param  Pulse width in percents [0 .. 100]
//------------------------------------------------------------------------------
void mcu_pwm_timer_set_channel_pulse_width
(
    PWM_Channel_t pwm_channel,
    uint8_t pulse_percents
);


#endif  // MCU_TIM_H

//  *** END OF FILE ***
