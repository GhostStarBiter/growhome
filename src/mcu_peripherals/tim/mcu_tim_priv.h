#ifndef MCU_TIM_PRIV_H
#define MCU_TIM_PRIV_H

#include "main/application_map.h"
#include "stm32f10x_tim.h"
#include "mcu_tim.h"



//------------------------------------------------------------------------------
/// @brief
//------------------------------------------------------------------------------
static void init_servo_timer(void);


//------------------------------------------------------------------------------
/// @brief
//------------------------------------------------------------------------------
static void init_pwm_timer(void);


//------------------------------------------------------------------------------
/// @brief
//------------------------------------------------------------------------------
static void init_onewire_timer(void);

#endif  // MCU_TIM_PRIV_H
