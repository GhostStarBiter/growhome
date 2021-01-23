#ifndef SERVO_PRIV_H
#define SERVO_PRIV_H

#include "tim/mcu_tim.h"
#include "servo.h"

#define SERVO_MIN_IMPULSE_DUTY              544   // us
#define SERVO_TIME_REGULATION_INTERVAL      2000  // us
#define SERVO_AIR_OUTLET_REGULATION_ANGLE   180   // deg


typedef struct
{
    TIM_TypeDef*  timer;
    uint16_t      current_angle;
    float         coef_angle_duty;
} servo_t;

#endif // SERVO_PRIV_H
