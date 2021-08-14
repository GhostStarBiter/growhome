#ifndef MCU_TIM_H

#define MCU_TIM_H

#include "stm32f10x_tim.h"
#include "main/types.h"

//------------------------------------------------------------------------------
/// @brief  Init function of STM32F103 timers
//------------------------------------------------------------------------------
void mcu_tim_init(void);


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
