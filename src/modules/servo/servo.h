#ifndef SERVO_H
#define SERVO_H


typedef struct{
  TIM_TypeDef*  timer;
  uint8_t       initial_angle;
} servo_init_t;


//--------------------------------------------------------------------------------------------------
/// @brief
//--------------------------------------------------------------------------------------------------
void servo_init
(
    servo_init_t* init_struct
);


//--------------------------------------------------------------------------------------------------
/// @brief  Set desired angle of servo
/// @param  Desired servo angle in degrees
//--------------------------------------------------------------------------------------------------
void servo_set_angle
(
  uint8_t uw_set_angle      // set angle in range 0..180 deg
);


//--------------------------------------------------------------------------------------------------
/// @brief  ms-called function to control servo angle position
//--------------------------------------------------------------------------------------------------
void servo_control(void);


#endif // SERVO_H
