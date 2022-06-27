#include "tim/mcu_tim.h"
#include "servo.h"

#define SERVO_MIN_IMPULSE_DUTY              544   // us
#define SERVO_TIME_REGULATION_INTERVAL      2000  // us
#define SERVO_AIR_OUTLET_REGULATION_ANGLE   180   // deg


typedef struct
{
    TIM_TypeDef*  timer;

    uint16_t      set_angle;
    float         current_angle;
    float         angle_delta;
    float         coef_angle_duty;
} servo_t;

servo_t servodrive;


//--------------------------------------------------------------------------------------------------
void servo_init(servo_init_t* init_struct)
{
  servodrive.timer                = init_struct->timer;
  servodrive.current_angle        = init_struct->initial_angle;

  servodrive.coef_angle_duty = ((float) SERVO_TIME_REGULATION_INTERVAL / (float) SERVO_AIR_OUTLET_REGULATION_ANGLE);
  // close air outlet at init phase
  servo_set_angle(init_struct->initial_angle);

}


//--------------------------------------------------------------------------------------------------
void servo_set_angle(uint8_t angle_degrees)
{
  servodrive.set_angle = angle_degrees;
}


//--------------------------------------------------------------------------------------------------
void servo_control(void)
{
  uint16_t uw_tim_cc_new_value;
  float servo_impulse_additional_duty_us;

  servodrive.angle_delta = (float) (servodrive.set_angle - servodrive.current_angle);

  if(servodrive.angle_delta != 0)
  {
    if(servodrive.angle_delta < 0.5f)
    {
      servodrive.current_angle -=  0.01f;
    }
    else if(servodrive.angle_delta > 0.5f)
    {
      servodrive.current_angle += 0.01f;
    }
    else
    {
      // nothing to do
    }
  }


  servo_impulse_additional_duty_us = (uint16_t) servodrive.current_angle * servodrive.coef_angle_duty;
  uw_tim_cc_new_value = (uint16_t) (SERVO_MIN_IMPULSE_DUTY + servo_impulse_additional_duty_us);

  servodrive.timer->CCR1 = uw_tim_cc_new_value;
}
