#include "stm32f10x.h"
#include "configuration/peripherals_config.h"

#include "encoder_input.h"

#define ENCODER_POS_MAX_TICKS               512                 // 1024 ticks - full range of output value
#define ENCODER_NEG_MAX_TICKS               (-512)

#define ENCODER_BUTTON_PRESSED              GPIO_ReadInputDataBit(ENCODER_BUTTON_PORT, ENCODER_BUTTON_PIN)
#define ENCODER_A_HIGH                      GPIO_ReadInputDataBit(ENCODER_A_PORT, ENCODER_A_PIN)
#define ENCODER_B_HIGH                      GPIO_ReadInputDataBit(ENCODER_B_PORT, ENCODER_B_PIN)

typedef struct{
  bool      button_activated;
  int16_t   ticks;
} encoder_input_t;

volatile encoder_input_t encoder;

//--------------------------------------------------------------------------------------------------
void encoder_init(void)
{
  encoder.button_activated  = false;
  encoder.ticks             = 0;
}


//--------------------------------------------------------------------------------------------------
int16_t encoder_get_ticks(void)
{
  return encoder.ticks;
}


//--------------------------------------------------------------------------------------------------
void encoder_reset_ticks(void)
{
  encoder.ticks = 0;
}


//--------------------------------------------------------------------------------------------------
void encoder_deactivate_button(void)
{
  encoder.button_activated = false;
}


//--------------------------------------------------------------------------------------------------
bool encoder_button_activated(void)
{
  return encoder.button_activated;
}


//--------------------------------------------------------------------------------------------------
void ENCODER_INPUT_IRQHandler(void)
{
  // *** BUTTON
  if(EXTI_GetITStatus(ENCODER_BUTTON_EXTI_LINE) != RESET && ENCODER_BUTTON_PRESSED)
  {
    if(!encoder.button_activated)
    {
      encoder.button_activated = true;
      encoder.ticks = 0;
    }
    else
    {
      encoder.button_activated = false;
      encoder.ticks = 0;
    }
    EXTI_ClearITPendingBit(ENCODER_BUTTON_EXTI_LINE);
  }

  // ENCODER
  if(EXTI_GetITStatus(ENCODER_A_EXTI_LINE) != RESET || EXTI_GetITStatus(ENCODER_B_EXTI_LINE) != RESET)
  {
    // *** "POSITIVE" rotation
    if(ENCODER_A_HIGH && !ENCODER_B_HIGH)
    {
      // check overflow
      if(encoder.ticks < ENCODER_POS_MAX_TICKS)
      {
        encoder.ticks++;
      }
    }

    // *** "NEGATIVE" rotation
    if(ENCODER_B_HIGH && !ENCODER_A_HIGH)
    {
      // check underflow
      if(encoder.ticks > ENCODER_NEG_MAX_TICKS)
      {
        encoder.ticks--;
      }
    }
    EXTI_ClearITPendingBit(ENCODER_A_EXTI_LINE);
    EXTI_ClearITPendingBit(ENCODER_B_EXTI_LINE);
  }
}

//  *** END OF FILE ***
