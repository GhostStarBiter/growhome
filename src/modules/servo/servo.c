#include "servo_priv.h"

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
  uint16_t uw_tim_cc_new_value;
  float servo_impulse_additional_duty_us;

  if(angle_degrees > SERVO_AIR_OUTLET_REGULATION_ANGLE)
  {
    angle_degrees = SERVO_AIR_OUTLET_REGULATION_ANGLE;
  }

  servodrive.current_angle = angle_degrees;

  servo_impulse_additional_duty_us = angle_degrees * servodrive.coef_angle_duty;
  uw_tim_cc_new_value = (uint16_t) (SERVO_MIN_IMPULSE_DUTY + servo_impulse_additional_duty_us);

  servodrive.timer->CCR1 = uw_tim_cc_new_value;
}
