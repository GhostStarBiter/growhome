#ifndef PI_CTRL_H
#define PI_CTRL_H

typedef struct{
  double ref_value;
  double input_data;
  double kp;
  double ki;
  double integral_sum;
  double err;
  double prev_err;
  double output;
  double saturation_max;
  double saturation_min;
} pi_ctrl_object_t;

//--------------------------------------------------------------------------------------------------
/// @brief  PI controller init function
//--------------------------------------------------------------------------------------------------
void pi_ctrl_init(pi_ctrl_object_t* p_object);


//--------------------------------------------------------------------------------------------------
/// @brief  PI controller execution function
//--------------------------------------------------------------------------------------------------
void pi_ctrl_run(pi_ctrl_object_t* p_object);


#endif // PI_CTRL_H
