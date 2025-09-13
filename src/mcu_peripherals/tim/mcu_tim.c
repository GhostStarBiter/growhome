#include "configuration/peripherals_config.h"
#include "stm32f10x_tim.h"
#include "mcu_tim.h"

//------------------------------------------------------------------------------
/// @brief
//------------------------------------------------------------------------------
static void init_servo_timer(void);


//------------------------------------------------------------------------------
/// @brief  Init timer for DC drive PWM control
//------------------------------------------------------------------------------
static void init_pwm_timer(void);


//------------------------------------------------------------------------------
/// @brief  Init timer for OneWire bus
//------------------------------------------------------------------------------
static void init_onewire_timer(void);


//--------------------------------------------------------------------------------------------------
void mcu_tim_init(void)
{
  init_servo_timer();

  init_pwm_timer();
#if GRWHS_USE_ONEWIRE_SENSOR
  init_onewire_timer();
#endif
}


//--------------------------------------------------------------------------------------------------
static void init_servo_timer(void)
{
  TIM_TimeBaseInitTypeDef sTimBaseServoControl;
  TIM_OCInitTypeDef sOutputChannelInit;

  sTimBaseServoControl.TIM_ClockDivision       = TIM_CKD_DIV1;
  sTimBaseServoControl.TIM_CounterMode         = TIM_CounterMode_Up;
  sTimBaseServoControl.TIM_Period              = SERVO_TIMER_PERIOD;                      // 1000 to have update event with frequency of 36 kHz
  sTimBaseServoControl.TIM_Prescaler           = SERVO_TIMER_PRESCALER;                   // run timer on 1 MHz
  TIM_TimeBaseInit(SERVO_TIMER, &sTimBaseServoControl);

  TIM_OCStructInit(&sOutputChannelInit);

  sOutputChannelInit.TIM_OCIdleState  = TIM_OCIdleState_Reset;
  sOutputChannelInit.TIM_OCMode       = TIM_OCMode_PWM1;
  sOutputChannelInit.TIM_OCPolarity   = TIM_OCPolarity_High;
  sOutputChannelInit.TIM_OutputState  = TIM_OutputState_Enable;
  sOutputChannelInit.TIM_Pulse        = 0;

  //  channel 1 - servo drive control
  TIM_OC1Init(SERVO_TIMER, &sOutputChannelInit);
  TIM_CCxCmd(SERVO_TIMER, TIM_Channel_1, TIM_CCx_Enable);

  TIM_CtrlPWMOutputs(SERVO_TIMER, ENABLE);
  TIM_Cmd(SERVO_TIMER, ENABLE);
}


//--------------------------------------------------------------------------------------------------
static void init_pwm_timer(void)
{
  TIM_TimeBaseInitTypeDef sTimBasePWMControl;
  TIM_OCInitTypeDef sOutputChannelInit;

  sTimBasePWMControl.TIM_ClockDivision       = TIM_CKD_DIV1;
  sTimBasePWMControl.TIM_CounterMode         = TIM_CounterMode_Up;
  sTimBasePWMControl.TIM_Period              = PWM_CONTROL_TIMER_PERIOD;                      // 1000 to have update event with frequency of 36 kHz
  sTimBasePWMControl.TIM_Prescaler           = 0;                                             // run timer on 36 MHz
  TIM_TimeBaseInit(PWM_CONTROL_TIMER, &sTimBasePWMControl);

  TIM_OCStructInit(&sOutputChannelInit);

  sOutputChannelInit.TIM_OCIdleState  = TIM_OCIdleState_Reset;
  sOutputChannelInit.TIM_OCMode       = TIM_OCMode_PWM1;
  sOutputChannelInit.TIM_OCPolarity   = TIM_OCPolarity_High;
  sOutputChannelInit.TIM_OutputState  = TIM_OutputState_Enable;
  //sOutputChannelInit.TIM_Pulse        = 0;

  //  channel 1 - water pump regulation
  TIM_OC1Init(PWM_CONTROL_TIMER, &sOutputChannelInit);
  TIM_CCxCmd(PWM_CONTROL_TIMER, TIM_Channel_1, TIM_CCx_Enable);

  //  channel 2 - air mix intensity regulation
  TIM_OC2Init(PWM_CONTROL_TIMER, &sOutputChannelInit);
  TIM_CCxCmd(PWM_CONTROL_TIMER, TIM_Channel_2, TIM_CCx_Enable);

  TIM_ITConfig(PWM_CONTROL_TIMER, TIM_IT_CC2, ENABLE);

  TIM_CtrlPWMOutputs(PWM_CONTROL_TIMER, ENABLE);
  TIM_Cmd(PWM_CONTROL_TIMER, ENABLE);
}


//--------------------------------------------------------------------------------------------------
void mcu_pwm_timer_set_channel_pulse_width(PWM_Channel_t pwm_channel, uint8_t pulse_percents)
{
  uint16_t converted_pulse_width = 0;

  if(pulse_percents > 100)
  {
    pulse_percents = 100;
  }

  converted_pulse_width = (PWM_CONTROL_TIMER->ARR*pulse_percents)/100;


  switch (pwm_channel)
  {
    case WATER_PUMP:
      if(converted_pulse_width > 0)
      {
        // disable compare channel 1
        PWM_CONTROL_TIMER->CCER &= (uint16_t)(~(uint16_t)TIM_CCER_CC1E);
        // set new pulse width
        PWM_CONTROL_TIMER->CCR1 = converted_pulse_width;
        // enable compare channel 1
        PWM_CONTROL_TIMER->CCER |= (uint16_t)((uint16_t)TIM_CCER_CC1E);
      }
      else
      {
        // disable compare channel 1
        PWM_CONTROL_TIMER->CCER &= (uint16_t)(~(uint16_t)TIM_CCER_CC1E);
      }
    break;

    case AIR_INPUT:
      if(converted_pulse_width > 0)
      {
        // disable compare channel 2
        //PWM_CONTROL_TIMER->CCER &= (uint16_t)(~(uint16_t)TIM_CCER_CC2E);
        // set new pulse width
        PWM_CONTROL_TIMER->CCR2 = converted_pulse_width;
        // enable compare channel 2
        PWM_CONTROL_TIMER->CCER |= (uint16_t)((uint16_t)TIM_CCER_CC2E);
      }
      else
      {
        // disable compare channel 1
        PWM_CONTROL_TIMER->CCER &= (uint16_t)(~(uint16_t)TIM_CCER_CC2E);
      }
    break;

  }
}


//--------------------------------------------------------------------------------------------------
static void init_onewire_timer(void)
{
  TIM_TimeBaseInitTypeDef sTimBaseOnewire;

  sTimBaseOnewire.TIM_ClockDivision       = TIM_CKD_DIV1;
  sTimBaseOnewire.TIM_CounterMode         = TIM_CounterMode_Up;
  sTimBaseOnewire.TIM_Period              = 4;
  sTimBaseOnewire.TIM_Prescaler           = 71;
  sTimBaseOnewire.TIM_RepetitionCounter   = 0;
  TIM_TimeBaseInit(ONEWIRE_TIMER, &sTimBaseOnewire);

  TIM_ARRPreloadConfig(ONEWIRE_TIMER, ENABLE);
}


//--------------------------------------------------------------------------------------------------
void TIM1_UP_IRQHandler(void)
{
  if(TIM_GetFlagStatus(TIM1, TIM_FLAG_Update) == SET)
  {
    TIM_ClearITPendingBit(TIM1, TIM_FLAG_Update);

  }
}


//--------------------------------------------------------------------------------------------------
void PWM_CONTROL_IRQHandler(void)
{
  // Capture-Compare Channel 1
  if(TIM_GetFlagStatus(PWM_CONTROL_TIMER, WATER_PUMP_CTRL_CHANNEL_FLAG) == SET)
  {
    TIM_ClearITPendingBit(PWM_CONTROL_TIMER, TIM_IT_CC1);

  }

  // Capture-Compare Channel 2
  if(TIM_GetFlagStatus(PWM_CONTROL_TIMER, AIR_INPUT_MOTOR_CHANNEL_FLAG) == SET)
  {
    TIM_ClearITPendingBit(PWM_CONTROL_TIMER, TIM_IT_CC2);
  }
}

//  *** END OF FILE ***
