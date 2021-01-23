#ifndef SERVO_H
#define SERVO_H

#include "main/application_map.h"

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
/// @brief
/// @param
/// @param
//--------------------------------------------------------------------------------------------------
void servo_set_angle
(
  uint8_t uw_set_angle      // set angle in range 0..180 deg
);

#endif // SERVO_H
