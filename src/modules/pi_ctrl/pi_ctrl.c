#include "pi_ctrl_priv.h"


//--------------------------------------------------------------------------------------------------
void pi_ctrl_init(pi_ctrl_object_t* p_object)
{
  p_object->err = 0.0f;
  p_object->prev_err = 0.0f;
  p_object->integral_sum = 0.0f;
  p_object->output = 0.0f;
}


//--------------------------------------------------------------------------------------------------
void pi_ctrl_run(pi_ctrl_object_t* p_object)
{
  p_object->err = p_object->ref_value - p_object->input_data;
  p_object->integral_sum = p_object->integral_sum +
                              p_object->err*p_object->ki +
                                p_object->prev_err*p_object->ki;
  p_object->prev_err = p_object->err;

  p_object->output = p_object->err*p_object->kp + p_object->integral_sum;

  if(p_object->output > p_object->saturation_max)
  {
    p_object->output = p_object->saturation_max;
  }
  else if(p_object->output < p_object->saturation_min)
  {
    p_object->output = p_object->saturation_min;
  }
}
