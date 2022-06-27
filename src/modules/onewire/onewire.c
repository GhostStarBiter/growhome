#include "onewire_priv.h"


onewire_bus_t onewire;


//--------------------------------------------------------------------------------------------------
void onewire_init(onewire_init_t* init_struct)
{
  onewire.e_operation           = ONEWIRE_MODULE_IDLE;
  onewire.timer                 = init_struct->timer;
  onewire.gpio_pin              = init_struct->gpio_pin;
  onewire.gpio_port             = init_struct->gpio_port;
  onewire.exti_channel          = init_struct->EXTIchannel;
  onewire.exti_line             = init_struct->EXTI_line;
  onewire.uw_sensor_humidity    = 0;
  onewire.uw_sensor_temperature = 0;
  onewire.uw_sensor_parity      = 0;
  onewire.uw_idle_ms_counter    = 0;

  onewire_pin_interrupt_status(DISABLE);
  onewire.module_enable      = DISABLE;
}


//--------------------------------------------------------------------------------------------------
static void onewire_pin_interrupt_status(FunctionalState e_status)
{
  if(e_status)
  {
    /* Enable the Selected IRQ Channels --------------------------------------*/
    NVIC->ISER[onewire.exti_channel >> 0x05] =
        (uint32_t)0x01 << (onewire.exti_channel & (uint8_t)0x1F);
  }
  else
  {
    /* Disable the Selected IRQ Channels -------------------------------------*/
    NVIC->ICER[onewire.exti_channel >> 0x05] =
        (uint32_t)0x01 << (onewire.exti_channel & (uint8_t)0x1F);
  }
}


//--------------------------------------------------------------------------------------------------
void onewire_measure(void)
{
  if(onewire.uw_idle_ms_counter > ONEWIRE_UPDATE_RATE_MS)
  {

    onewire_set_status(ENABLE);
    onewire.uw_idle_ms_counter = 0;
  }
  else
  {
    onewire.uw_idle_ms_counter++;
  }
}


//--------------------------------------------------------------------------------------------------
static void onewire_set_status(FunctionalState eState)
{
  if(eState)
  {
    onewire.module_enable = ENABLE;
    onewire.sensor_present = ENABLE;
    onewire.data_valid = 0;
    onewire_config_pin(ONEWIRE_WRITE_RESET);
    // line to "HIGH"
    GPIO_WriteBit(onewire.gpio_port, onewire.gpio_pin, Bit_SET);

    TIM_ITConfig(onewire.timer, TIM_IT_Update, ENABLE);
    TIM_Cmd(onewire.timer, ENABLE);
  }
  else
    // DISABLE module
  {
    onewire_config_pin(ONEWIRE_MODULE_IDLE);
    // line to "HIGH"
    GPIO_WriteBit(onewire.gpio_port, onewire.gpio_pin, Bit_SET);

    TIM_ITConfig(onewire.timer, TIM_IT_Update, DISABLE);
    TIM_Cmd(onewire.timer, DISABLE);
    onewire.module_enable = DISABLE;
    onewire.sensor_present = DISABLE;
    onewire.ul_timeframes_counter = 0;
  }
}





//--------------------------------------------------------------------------------------------------
static void onewire_5ms_callback(void)
{
  if(onewire.module_enable && onewire.sensor_present)
  {

    onewire.ul_timeframes_counter++;

    switch(onewire.e_operation)
    {
      case ONEWIRE_WRITE_RESET:

        // communication phase "Host HOLD LOW for 1000 us"
        if(onewire.ul_timeframes_counter < ONEWIRE_HOST_HOLD_LOW_US)
        {
          GPIO_WriteBit(onewire.gpio_port, onewire.gpio_pin, Bit_RESET);
        }
        else
        // communication phase "Host HOLD HIGH for 50 us"
        if(onewire.ul_timeframes_counter < (ONEWIRE_HOST_HOLD_LOW_US + ONEWIRE_HOST_HOLD_HIGH_US))
        {
          GPIO_WriteBit(onewire.gpio_port, onewire.gpio_pin, Bit_SET);
          if(!onewire_line_HIGH())
          {
            onewire.ul_timeframes_counter = 0;
            onewire_config_pin(ONEWIRE_READ_RESPONSE);
          }
        }
        else
        {
          GPIO_WriteBit(onewire.gpio_port, onewire.gpio_pin, Bit_RESET);
          onewire.ul_timeframes_counter = 0;
          onewire_config_pin(ONEWIRE_READ_RESPONSE);
        }
      break;

      case ONEWIRE_READ_RESPONSE:
        if(onewire.ul_timeframes_counter < ONEWIRE_SENSOR_RESPONSE_LOW_US)
        {
          if(!onewire_line_HIGH())
          {
            onewire.sensor_present = ENABLE;
          }
          else
          {
            onewire_sensor_malfunction();
          }
        }
        else
        if(onewire.ul_timeframes_counter > (ONEWIRE_SENSOR_RESPONSE_LOW_US))
        {
          if(onewire_line_HIGH())
          {
            onewire.ub_bit_counter = 0;
            onewire_set_operation(ONEWIRE_READ_DATA);
            onewire_pin_interrupt_status(ENABLE);
          }
          else
          {
            onewire_sensor_malfunction();
          }
        }
      break;

      case ONEWIRE_READ_DATA:
        // EXTI IRQ driven (see ONEWIRE_EXTI_IRQHandler)

        // sensor finished transmission
        if(onewire.ub_bit_counter >= 40)
        {
          onewire.ub_bit_counter = 0;
          onewire_pin_interrupt_status(DISABLE);
          onewire_set_status(DISABLE);
          onewire_check_parity();
        }
      break;

      default:
      break;
    }
  }
  else
  {

  }
}


//--------------------------------------------------------------------------------------------------
static void onewire_check_parity(void)
{
  uint8_t ub_hum_low = 0,
          ub_hum_high = 0,
          ub_temp_low = 0,
          ub_temp_high = 0,
          ub_parity = 0;
  uint16_t uw_sum = 0;

  ub_hum_low = (uint8_t) (onewire.uw_sensor_humidity & 0xFF);
  ub_hum_high = (uint8_t) ((onewire.uw_sensor_humidity >> 8u) & 0xFF);

  ub_temp_low = (uint8_t) (onewire.uw_sensor_temperature & 0xFF);
  ub_temp_high = (uint8_t) ((onewire.uw_sensor_temperature >> 8u) & 0xFF);

  ub_parity = (uint8_t) (onewire.uw_sensor_parity & 0xFF);
  uw_sum = (ub_hum_high + ub_hum_low + ub_temp_high + ub_temp_low);

  uw_sum %= 0xFF;

  (ub_parity == (uint8_t)uw_sum) ? (onewire.data_valid = 1) : (onewire.data_valid = 0);
}


//--------------------------------------------------------------------------------------------------
static void onewire_sensor_malfunction(void)
{
  onewire.sensor_present = DISABLE;
  onewire.uw_sensor_humidity = 0;
  onewire.uw_sensor_temperature = 0;
}


//--------------------------------------------------------------------------------------------------
uint16_t onewire_get_temperature(void)
{
  uint16_t tmp = 0;

  if(onewire.data_valid)
  {
    // AM2301 sensor measures value of temperature 10 times the actual
    tmp = onewire.uw_sensor_temperature/10;
  }
  else
  {
    tmp = 0xFFFF;
  }
  return tmp;
}


//--------------------------------------------------------------------------------------------------
uint16_t onewire_get_humidity(void)
{
  uint16_t tmp;

  if(onewire.data_valid)
  {
    // AM2301 sensor measures value of humidity 10 times the actual
    tmp = onewire.uw_sensor_humidity/10;
  }
  else
  {
    tmp = 0xFFFF;
  }
  return tmp;
}


//--------------------------------------------------------------------------------------------------
static void onewire_set_operation(onewire_operation e_operation)
{
  onewire.e_operation = e_operation;
}


//--------------------------------------------------------------------------------------------------
static void onewire_config_pin(onewire_operation e_operation)
{
  GPIO_InitTypeDef sOW_GPIO_Config;

  sOW_GPIO_Config.GPIO_Pin = onewire.gpio_pin;
  sOW_GPIO_Config.GPIO_Speed = GPIO_Speed_50MHz;

  switch(e_operation)
  {
    case ONEWIRE_WRITE_RESET:
      onewire_set_operation(ONEWIRE_WRITE_RESET);
      sOW_GPIO_Config.GPIO_Mode = GPIO_Mode_Out_OD; //GPIO_Mode_Out_OD;
    break;

    case ONEWIRE_READ_RESPONSE:
      onewire_set_operation(ONEWIRE_READ_RESPONSE);
      sOW_GPIO_Config.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    break;

    case ONEWIRE_MODULE_IDLE:
    default:
      onewire_set_operation(ONEWIRE_MODULE_IDLE);
      sOW_GPIO_Config.GPIO_Mode = GPIO_Mode_Out_OD; //GPIO_Mode_Out_OD;
    break;
  }

  GPIO_Init(onewire.gpio_port, &sOW_GPIO_Config);

}


//--------------------------------------------------------------------------------------------------
static BitAction onewire_line_HIGH(void)
{
  return (BitAction) (onewire.gpio_port->IDR & (onewire.gpio_pin));
}


//--------------------------------------------------------------------------------------------------
void ONEWIRE_TIMER_IRQHandler(void)
{
  if(TIM_GetITStatus(onewire.timer, TIM_IT_Update))
  {
    TIM_ClearITPendingBit(onewire.timer, TIM_IT_Update);
    onewire_5ms_callback();
  }
}


//--------------------------------------------------------------------------------------------------
void ONEWIRE_EXTI_IRQHandler(void)
{
  static uint8_t ub_falling_edge_counter = 0;
  uint8_t bit_shift = 0;
  uint16_t* p_data_16bit = 0;

  if(EXTI_GetITStatus(onewire.exti_line) != RESET)
  {
    /* Clear the  EXTI line 0 pending bit */
    EXTI_ClearITPendingBit(onewire.exti_line);

    // FALLING edge
    if(!onewire_line_HIGH())
    {
      ub_falling_edge_counter++;

      if(!onewire.ub_bit_counter && ub_falling_edge_counter == 1)
      {
        onewire.ul_timeframes_counter = 0;
      }


      if(ub_falling_edge_counter > 1)
      {
        // Finished bit-sequence
        if(onewire.ub_bit_counter < 16)
        {
          bit_shift = 16 - onewire.ub_bit_counter;
          p_data_16bit = &onewire.uw_sensor_humidity;
        }
        else if(onewire.ub_bit_counter < 32)
        {
          bit_shift = 32 - onewire.ub_bit_counter;
          p_data_16bit = &onewire.uw_sensor_temperature;
        }
        else if(onewire.ub_bit_counter < 40)
        {
          bit_shift = 39 - onewire.ub_bit_counter;
          p_data_16bit = &onewire.uw_sensor_parity;
        }

        // Decision of bit-read value
        if(onewire.ul_timeframes_counter <= ONEWIRE_SENSOR_BIT_0_TIME_US)
        {
          *p_data_16bit &= ~(1u << bit_shift);
        }
        else
        if(onewire.ul_timeframes_counter <= ONEWIRE_SENSOR_BIT_1_TIME_US)
        {
          *p_data_16bit |= (1u << bit_shift);
        }

        onewire.ub_bit_counter++;

        onewire.ul_timeframes_counter = 0;
        // the second falling edge (end-of-bit-sequence)
        // is the first falling edge for next bit
        ub_falling_edge_counter = 1;
      }
    }
    else
    // RISING edge
    {
      if(onewire.ul_timeframes_counter > ONEWIRE_SENSOR_BIT_LOW_TIME_US)
      {
        // @todo: error - wrong sensor's bit low timing
      }
    }
  }
}

