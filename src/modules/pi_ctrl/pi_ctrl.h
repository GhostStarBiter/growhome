#ifndef PI_CTRL_H
#define PI_CTRL_H

typedef struct{
  double ref_value;         // value to follow
  double input_data;        // new measured data
  double sample_time_s;     // sample time in seconds
  double kp;                // proportional coefficient
  double ki;                // integral coefficient
  double integral_sum;      // sum of intergal part
  double err;               // current error
  double prev_err;          // previous control cycle error
  double output;            // calculated regulator output
  double saturation_max;    // regulator maximum output
  double saturation_min;    // regulator minimum output
} pi_ctrl_object_t;


//--------------------------------------------------------------------------------------------------
/// @brief  PI controller init function
//--------------------------------------------------------------------------------------------------
void pi_ctrl_init(pi_ctrl_object_t* p_object);


//--------------------------------------------------------------------------------------------------
/// @brief  PI controller execution function
/// @return Output of PI controller (saturated)
//--------------------------------------------------------------------------------------------------
double pi_ctrl_run(pi_ctrl_object_t* p_object, double set_value, double new_measure);


#endif // PI_CTRL_H
