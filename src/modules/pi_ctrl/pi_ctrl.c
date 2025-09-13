#include "pi_ctrl_priv.h"


//--------------------------------------------------------------------------------------------------
// Set saturation_min and saturation_max independently for every object
//  Example:
//    pi_ctrl_object_t object;
//    object.pi_controller.kp = proportional_coeff;
//    object.pi_controller.ki = integral_coeff;
//    object.pi_controller.sample_time_s = time_in_sec;
//    object.saturation_min = some_min_value;
//    object.saturation_max = some_max_value;
//
//  pi_ctrl_init(&object);
//--------------------------------------------------------------------------------------------------
void pi_ctrl_init(pi_ctrl_object_t* p_object)
{
  p_object->err = 0.0f;
  p_object->prev_err = 0.0f;
  p_object->integral_sum = 0.0f;
  p_object->output = 0.0f;
}


//--------------------------------------------------------------------------------------------------
// Set ref_value and input_data from outside before call
//--------------------------------------------------------------------------------------------------
double pi_ctrl_run(pi_ctrl_object_t* p_object, double set_value, double new_measure)
{
  double integral_lim_min;
  double integral_lim_max;
  double proportional;

  p_object->err = set_value - new_measure;

  proportional = p_object->err*p_object->kp;

  p_object->integral_sum = p_object->integral_sum +
                              0.5f * p_object->ki * p_object->sample_time_s * (p_object->err + p_object->prev_err);

  if(proportional < p_object->saturation_max)
  {
    integral_lim_max = p_object->saturation_max - proportional;
  }
  else
  {
    integral_lim_max = 0;
  }

  if(proportional > p_object->saturation_min)
  {
    integral_lim_min = p_object->saturation_min - proportional;
  }
  else
  {
    integral_lim_min = 0;
  }

  if(p_object->integral_sum > integral_lim_max)
  {
    p_object->integral_sum = integral_lim_max;
  }
  else if(p_object->integral_sum < integral_lim_min)
  {
    p_object->integral_sum = integral_lim_min;
  }

  p_object->output = proportional + p_object->integral_sum;

  if(p_object->output > p_object->saturation_max)
  {
    p_object->output = p_object->saturation_max;
  }
  else if(p_object->output < p_object->saturation_min)
  {
    p_object->output = p_object->saturation_min;
  }

  p_object->prev_err = p_object->err;


  return p_object->output;
}
