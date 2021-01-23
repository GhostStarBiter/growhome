#ifndef ENCODER_INPUT_PRIV_H
#define ENCODER_INPUT_PRIV_H

#include "stm32f10x.h"
#include "main/application_map.h"

#include "encoder_input.h"

#define ENCODER_BUTTON_PRESSED              GPIO_ReadInputDataBit(ENCODER_BUTTON_PORT, ENCODER_BUTTON_PIN)
#define ENCODER_A_HIGH                      GPIO_ReadInputDataBit(ENCODER_A_PORT, ENCODER_A_PIN)
#define ENCODER_B_HIGH                      GPIO_ReadInputDataBit(ENCODER_B_PORT, ENCODER_B_PIN)

typedef volatile struct{
  bool      button_activated;
  int16_t   ticks;
} encoder_input_t;


#endif // ENCODER_INPUT_PRIV_H

//  *** END OF FILE ***
