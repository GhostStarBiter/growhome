#include "mcu_gpio.h"

#include "configuration/app_config.h"

#include "configuration/peripherals_config.h"

#include "stm32f10x_exti.h"


//------------------------------------------------------------------------------
/// @brief Init of MCU pins for analog signal's measurement
//------------------------------------------------------------------------------
static void gpio_init_analog(void);


//------------------------------------------------------------------------------
/// @brief Init of MCU pins for LCD 2x16 (HD44780)
//------------------------------------------------------------------------------
static void gpio_init_LCD216(void);


//------------------------------------------------------------------------------
/// @brief Init of MCU pins for triac control
//------------------------------------------------------------------------------
static void gpio_init_triac(void);


//------------------------------------------------------------------------------
/// @brief Init of MCU pins for servo motor control
//------------------------------------------------------------------------------
static void gpio_init_servo(void);


//------------------------------------------------------------------------------
/// @brief Init of MCU pins for DC elements (mostly PWM)
//------------------------------------------------------------------------------
static void gpio_init_pwm(void);


//------------------------------------------------------------------------------
/// @brief Init of MCU pins for communication interfaces
//------------------------------------------------------------------------------
static void gpio_init_communication(void);


//------------------------------------------------------------------------------
/// @brief Init of MCU pins responsible for external interrupts
//------------------------------------------------------------------------------
static void gpio_init_EXTI(void);


//------------------------------------------------------------------------------
/// @brief Init of MCU pins miscelaneous use
//------------------------------------------------------------------------------
static void gpio_init_misc(void);


//--------------------------------------------------------------------------------------------------
void mcu_gpio_init(void)
{
  gpio_init_analog();
  gpio_init_LCD216();
  gpio_init_triac();
  gpio_init_pwm();
  gpio_init_servo();
  gpio_init_communication();
  gpio_init_EXTI();
  gpio_init_misc();
}


//--------------------------------------------------------------------------------------------------
static void gpio_init_analog(void)
{
  GPIO_InitTypeDef sGPIOInit;
  // analog measurement
  sGPIOInit.GPIO_Pin    = INCOME_AIR_TEMP_PIN |
                          OUTLET_AIR_TEMP_PIN |
                          WATER_TANK_LEVEL_MEASURE_PIN;
  sGPIOInit.GPIO_Mode   = GPIO_Mode_AIN;
  GPIO_Init(ANALOG_MEASURE_PORT, &sGPIOInit);
}


//--------------------------------------------------------------------------------------------------
static void gpio_init_LCD216(void)
{
  if( APPLICATION_MODE )
  {
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);
  }

  GPIO_InitTypeDef sGPIOInit;

  sGPIOInit.GPIO_Pin    = LCD216_RS_PIN;
  sGPIOInit.GPIO_Mode   = GPIO_Mode_Out_PP;
  sGPIOInit.GPIO_Speed  = GPIO_Speed_2MHz;
  GPIO_Init(LCD216_RS_PORT, &sGPIOInit);

  sGPIOInit.GPIO_Pin    = LCD216_EN_PIN;
  sGPIOInit.GPIO_Mode   = GPIO_Mode_Out_PP;
  sGPIOInit.GPIO_Speed  = GPIO_Speed_2MHz;
  GPIO_Init(LCD216_EN_PORT, &sGPIOInit);

  sGPIOInit.GPIO_Pin    = LCD216_D4_PIN;
  sGPIOInit.GPIO_Mode   = GPIO_Mode_Out_PP;
  sGPIOInit.GPIO_Speed  = GPIO_Speed_2MHz;
  GPIO_Init(LCD216_D4_PORT, &sGPIOInit);

  sGPIOInit.GPIO_Pin    = LCD216_D5_PIN;
  sGPIOInit.GPIO_Mode   = GPIO_Mode_Out_PP;
  sGPIOInit.GPIO_Speed  = GPIO_Speed_2MHz;
  GPIO_Init(LCD216_D5_PORT, &sGPIOInit);

  sGPIOInit.GPIO_Pin    = LCD216_D6_PIN;
  sGPIOInit.GPIO_Mode   = GPIO_Mode_Out_PP;
  sGPIOInit.GPIO_Speed  = GPIO_Speed_2MHz;
  GPIO_Init(LCD216_D6_PORT, &sGPIOInit);

  sGPIOInit.GPIO_Pin    = LCD216_D7_PIN;
  sGPIOInit.GPIO_Mode   = GPIO_Mode_Out_PP;
  sGPIOInit.GPIO_Speed  = GPIO_Speed_2MHz;
  GPIO_Init(LCD216_D7_PORT, &sGPIOInit);


}


//--------------------------------------------------------------------------------------------------
static void gpio_init_triac(void)
{
  GPIO_InitTypeDef sGPIOInit;

  sGPIOInit.GPIO_Pin    = TRIAC_AIR_HEATER_PIN;
  sGPIOInit.GPIO_Mode   = GPIO_Mode_Out_PP;
  sGPIOInit.GPIO_Speed  = GPIO_Speed_50MHz;
  GPIO_Init(TRIAC_GPIO_PORT, &sGPIOInit);

  GPIO_WriteBit(TRIAC_GPIO_PORT, TRIAC_AIR_HEATER_PIN, Bit_RESET);
}


//--------------------------------------------------------------------------------------------------
static void gpio_init_servo(void)
{
  GPIO_InitTypeDef sGPIOInit;

  sGPIOInit.GPIO_Pin    = SERVO_GPIO_PIN;
  sGPIOInit.GPIO_Mode   = GPIO_Mode_AF_PP;
  sGPIOInit.GPIO_Speed  = GPIO_Speed_50MHz;
  GPIO_Init(SERVO_GPIO_PORT, &sGPIOInit);
}


//--------------------------------------------------------------------------------------------------
static void gpio_init_pwm(void)
{
  GPIO_InitTypeDef sGPIOInit;

  // DC driven elements (30 kHz PWM)
  sGPIOInit.GPIO_Pin    = PWM_WATER_PUMP_PIN |
                          PWM_AIR_INPUT_PIN;
  sGPIOInit.GPIO_Mode   = GPIO_Mode_AF_PP;
  sGPIOInit.GPIO_Speed  = GPIO_Speed_50MHz;
  GPIO_Init(PWM_CONTROL_PORT, &sGPIOInit);

  sGPIOInit.GPIO_Pin    = MIX_AIR_PIN;
  sGPIOInit.GPIO_Mode   = GPIO_Mode_Out_PP;
  sGPIOInit.GPIO_Speed  = GPIO_Speed_2MHz;
  GPIO_Init(MIX_AIR_PORT, &sGPIOInit);
}


//--------------------------------------------------------------------------------------------------
static void gpio_init_communication(void)
{
  GPIO_InitTypeDef sGPIOInit;

  // i2c bus
  sGPIOInit.GPIO_Pin    = I2C1_SDA_PIN;
  sGPIOInit.GPIO_Mode   = GPIO_Mode_AF_OD;
  sGPIOInit.GPIO_Speed  = GPIO_Speed_50MHz;
  GPIO_Init(I2C1_PORT, &sGPIOInit);

  sGPIOInit.GPIO_Pin    = I2C1_SCL_PIN;
  GPIO_Init(I2C1_PORT, &sGPIOInit);

  // uart (esp8266)
  sGPIOInit.GPIO_Pin    = ESP_RST_PIN;
  sGPIOInit.GPIO_Mode   = GPIO_Mode_Out_PP;
  sGPIOInit.GPIO_Speed  = GPIO_Speed_2MHz;
  GPIO_Init(ESP_RST_PORT, &sGPIOInit);

  sGPIOInit.GPIO_Pin    = ESP_EN_PIN;
  sGPIOInit.GPIO_Mode   = GPIO_Mode_Out_PP;
  sGPIOInit.GPIO_Speed  = GPIO_Speed_2MHz;
  GPIO_Init(ESP_EN_PORT, &sGPIOInit);

  sGPIOInit.GPIO_Pin    = ESP_UART_TX_PIN;
  sGPIOInit.GPIO_Mode   = GPIO_Mode_AF_PP;
  sGPIOInit.GPIO_Speed  = GPIO_Speed_50MHz;
  GPIO_Init(ESP_UART_PORT, &sGPIOInit);

  sGPIOInit.GPIO_Pin    = ESP_UART_RX_PIN;
  sGPIOInit.GPIO_Mode   = GPIO_Mode_IN_FLOATING;
  GPIO_Init(ESP_UART_PORT, &sGPIOInit);

  sGPIOInit.GPIO_Pin = SPI_NSS_PIN | SPI_CLK_PIN | SPI_MOSI_PIN;
  sGPIOInit.GPIO_Speed = GPIO_Speed_50MHz;
  sGPIOInit.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(SPI_PORT, &sGPIOInit);

}


//--------------------------------------------------------------------------------------------------
static void gpio_init_EXTI(void)
{
  EXTI_InitTypeDef EXTI_InitStructure;
  GPIO_InitTypeDef sGPIOInit;

  // ONEWIRE
  sGPIOInit.GPIO_Pin    = ONEWIRE_PIN;
  sGPIOInit.GPIO_Mode   = GPIO_Mode_IN_FLOATING;
  sGPIOInit.GPIO_Speed  = GPIO_Speed_50MHz;
  GPIO_Init(ONEWIRE_PORT, &sGPIOInit);

  GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, ONEWIRE_PIN_SOURCE);
  EXTI_InitStructure.EXTI_Line    = ONEWIRE_EXTI_LINE;
  EXTI_InitStructure.EXTI_Mode    = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  // Push-Button Encoder
  sGPIOInit.GPIO_Pin    = ENCODER_A_PIN |
                          ENCODER_B_PIN |
                          ENCODER_BUTTON_PIN;
  sGPIOInit.GPIO_Mode   = GPIO_Mode_IN_FLOATING;
  GPIO_Init(ENCODER_PORT, &sGPIOInit);

  EXTI_InitStructure.EXTI_Mode    = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;

  // ***
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, ENCODER_BUTTON_PIN_SOURCE);
  EXTI_InitStructure.EXTI_Line    = ENCODER_BUTTON_EXTI_LINE;
  EXTI_Init(&EXTI_InitStructure);

  // ***
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, ENCODER_A_PIN_SOURCE);
  EXTI_InitStructure.EXTI_Line    = ENCODER_A_EXTI_LINE;
  EXTI_Init(&EXTI_InitStructure);

  // ***
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, ENCODER_B_PIN_SOURCE);
  EXTI_InitStructure.EXTI_Line    = ENCODER_B_EXTI_LINE;
  EXTI_Init(&EXTI_InitStructure);
}


//--------------------------------------------------------------------------------------------------
// GPIO port B initialization function
//--------------------------------------------------------------------------------------------------
static void gpio_init_misc(void)
{
  GPIO_InitTypeDef sGPIOInit;

  // onboard led
  sGPIOInit.GPIO_Pin    = SYSTEM_STATUS_LED_PIN;
  sGPIOInit.GPIO_Mode   = GPIO_Mode_Out_PP;
  sGPIOInit.GPIO_Speed  = GPIO_Speed_10MHz;
  GPIO_Init(SYSTEM_STATUS_LED_PORT, &sGPIOInit);

  // Light On-Off control using relay
  sGPIOInit.GPIO_Pin = LIGHT_CTRL_PIN;
  sGPIOInit.GPIO_Mode = GPIO_Mode_Out_PP;
  sGPIOInit.GPIO_Speed  = GPIO_Speed_2MHz;
  GPIO_Init(LIGHT_CTRL_PORT, &sGPIOInit);

  // debug pin
  sGPIOInit.GPIO_Pin    = DEBUG_PIN;
  sGPIOInit.GPIO_Mode   = GPIO_Mode_Out_PP;
  sGPIOInit.GPIO_Speed  = GPIO_Speed_50MHz;
  GPIO_Init(DEBUG_PORT, &sGPIOInit);

  // onboard button
  sGPIOInit.GPIO_Pin    = SYSTEM_BUTTON_PIN;
  sGPIOInit.GPIO_Mode   = GPIO_Mode_IPD;
  sGPIOInit.GPIO_Speed  = GPIO_Speed_10MHz;
  GPIO_Init(SYSTEM_BUTTON_PORT, &sGPIOInit);
}


//--------------------------------------------------------------------------------------------------
GPIO_TypeDef* mcu_gpio_get_onewire_port(void)
{
  return ONEWIRE_PORT;
}


//--------------------------------------------------------------------------------------------------
uint16_t mcu_gpio_get_onewire_pin(void)
{
  return ONEWIRE_PIN;
}


//--------------------------------------------------------------------------------------------------
void mcu_gpio_set_debug_pin(FunctionalState dbg_pin_status)
{
  if(dbg_pin_status)
  {
    GPIO_SetBits(DEBUG_PORT, DEBUG_PIN);
  }
  else
  {
    GPIO_ResetBits(DEBUG_PORT, DEBUG_PIN);
  }
}


//--------------------------------------------------------------------------------------------------
void mcu_gpio_toggle_debug_pin(void)
{
  static uint8_t state = 0;
  if(state)
  {
    GPIO_ResetBits(DEBUG_PORT, DEBUG_PIN);
    state = 0;
  }
  else
  {
    GPIO_SetBits(DEBUG_PORT, DEBUG_PIN);
    state = 1;
  }
}


//--------------------------------------------------------------------------------------------------
FlagStatus mcu_gpio_get_button_status(void)
{
  // ACTIVE LOW
  return (FlagStatus) !(GPIO_ReadInputDataBit(SYSTEM_BUTTON_PORT, SYSTEM_BUTTON_PIN));
}


//--------------------------------------------------------------------------------------------------
void mcu_gpio_set_led_status(FunctionalState led_status)
{
  if(led_status)
  {
    GPIO_SetBits(SYSTEM_STATUS_LED_PORT, SYSTEM_STATUS_LED_PIN);
  }
  else
  {
    GPIO_ResetBits(SYSTEM_STATUS_LED_PORT, SYSTEM_STATUS_LED_PIN);
  }
}


//--------------------------------------------------------------------------------------------------
void mcu_gpio_set_light(FunctionalState light_state)
{
  if(light_state)
  {
    GPIO_SetBits(LIGHT_CTRL_PORT, LIGHT_CTRL_PIN);
  }
  else
  {
    GPIO_ResetBits(LIGHT_CTRL_PORT, LIGHT_CTRL_PIN);
  }
}


//--------------------------------------------------------------------------------------------------
void mcu_gpio_set_air_mixing_status(FunctionalState air_mixing_status)
{
  if(air_mixing_status)
  {
    GPIO_SetBits(MIX_AIR_PORT, MIX_AIR_PIN);
  }
  else
  {
    GPIO_ResetBits(MIX_AIR_PORT, MIX_AIR_PIN);
  }
}


//--------------------------------------------------------------------------------------------------
void mcu_gpio_set_heater_status(FunctionalState heater_state)
{
  if(heater_state)
  {
    GPIO_WriteBit(TRIAC_GPIO_PORT, TRIAC_AIR_HEATER_PIN, Bit_SET);
  }
  else
  {
    GPIO_WriteBit(TRIAC_GPIO_PORT, TRIAC_AIR_HEATER_PIN, Bit_RESET);
  }
}


//--------------------------------------------------------------------------------------------------
void mcu_gpio_set_esp_enable_status(FunctionalState esp_enable_state)
{
  if(esp_enable_state)
  {
    GPIO_WriteBit(ESP_EN_PORT, ESP_EN_PIN, Bit_SET);
  }
  else
  {
    GPIO_WriteBit(ESP_EN_PORT, ESP_EN_PIN, Bit_RESET);
  }
}


//--------------------------------------------------------------------------------------------------
void mcu_gpio_set_esp_reset_status(FunctionalState esp_reset_state)
{
  if(esp_reset_state)
  {
    GPIO_WriteBit(ESP_RST_PORT, ESP_RST_PIN, Bit_RESET);
  }
  else
  {
    GPIO_WriteBit(ESP_RST_PORT, ESP_RST_PIN, Bit_SET);
  }
}
