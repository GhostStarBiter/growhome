//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  GPIO pins assignment
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#ifndef APPLICATION_MAP_H
#define APPLICATION_MAP_H

#include "stm32f10x.h"
#include "system_config/stm32f10x_conf.h"

//  If set to 1
//  don't remap SWD pins for LCD216 usage
#define APPLICATION_DEBUG                   0


//  APPLICATION
#define APPLICATION_NAME                    "GROWHOME"
#define APPLICATION_NAME_POS_X              4
#define APPLICATION_NAME_POS_Y              0

#define APPLICATION_VERSION                 "v00.01"
#define APPLICATION_VERSION_POS_X           10
#define APPLICATION_VERSION_POS_Y           1


// ***
// Task priorities
#define USER_INTERFACE_TASK_PRIORITY        2
#define SERVICE_TASK_PRIORITY               1
#define GROWBOX_TASK_PRIORITY               3

// ***
// Task call intervals
#define INTERFACE_TASK_TIMEOUT              10  // [ticks] == ms
#define SERVICE_TASK_TIMEOUT                1000   // [ticks] == ms
#define GROWBOX_TASK_TIMEOUT                1   // [tick] == ms


// ***
// ADC measurement
#define MEASUREMENTS_BUFFER_SIZE            8

#define APPLICATION_USE_LM60_TEMP           1
#define APPLICATION_USE_AM2301_TEMP         (!(APPLICATION_USE_LM60_TEMP))

#define ADC_MAX_INPUT_VOLTAGE               3300  // mV
#define ADC_MAX_OUTPUT_RAW_VALUE            4096  // n/a


//          Servo motor
#define SERVO_GPIO_PORT                     GPIOA
#define SERVO_GPIO_PIN                      GPIO_Pin_0

#define SERVO_TIMER                         TIM2
#define SERVO_TIMER_PRESCALER               (36 - 1)                                // 1MHz = 1us tick
#define SERVO_TIMER_PERIOD                  20000                                   // 20 ms

#define SERVO_AIR_OUTLET_CLOSED             0
#define SERVO_AIR_EXCHANGE_ANGLE            15
#define SERVO_AIR_OUTLET_ANGLE              45

// ****
#define SIM800_RESET                        GPIO_Pin_1

//------------------------------------------------
//          ANALOG measurement
#define ANALOG_MEASURE_PORT                 GPIOA

#define ADC_CONVERSION_CHANNELS             3


#define OPAMP_AMPLIFICATION_RATIO           3
#define LM60_ZERO_DEGREES_OFFSET_CONVERTED  (424 * OPAMP_AMPLIFICATION_RATIO)
#define LM60_TEMP_SENSOR_OPAMP_MV_PER_DEG   19    // 1000 mv [sensor@100C] * 3 [opamp ratio]/ 100 C
#define RAW_ADC_TO_MV(raw)                  (float) ((raw)*ADC_MAX_INPUT_VOLTAGE/ADC_MAX_OUTPUT_RAW_VALUE)

// Air temperature 1 measurement pin
#define INCOME_AIR_TEMP_PIN                 GPIO_Pin_2
#define ANALOG_TEMP_1_CHANNEL               ADC_Channel_2
#define TEMP_1_CONVERSION_ORDER             1
#define CHANNEL_TEMP_1                      (TEMP_1_CONVERSION_ORDER - 1)

// Air temperature 2 measurement pin
#define OUTLET_AIR_TEMP_PIN                 GPIO_Pin_3
#define ANALOG_TEMP_2_CHANNEL               ADC_Channel_3
#define TEMP_2_CONVERSION_ORDER             2
#define CHANNEL_TEMP_2                      (TEMP_2_CONVERSION_ORDER - 1)

#define AIR_REGULATION_TOLERANCE_DEGREES    2

// Water tank 1 level measurement pin
#define WATER_TANK_LEVEL_MEASURE_PIN        GPIO_Pin_4
#define WATER_TANK_LEVEL_CHANNEL            ADC_Channel_4
#define WATER_TANK_LEVEL_CONVERSION_ORDER   3
#define CHANNEL_WATER_TANK                  (WATER_TANK_LEVEL_CONVERSION_ORDER - 1)

// ESP wifi module RST pin
#define ESP_RST_PORT                        GPIOA
#define ESP_RST_PIN                         GPIO_Pin_5

// PWM water pump (AF TIM3 CH1)
#define PWM_CONTROL_PORT                    GPIOA
#define PWM_WATER_PUMP_PIN                  GPIO_Pin_6
#define WATER_PUMP_CTRL_CHANNEL_FLAG        TIM_FLAG_CC1
#define WATER_PUMP_INITIAL_POWER            50                                          // [%]

//  PWM Air Input Green House (AF TIM3_CH2)
#define PWM_AIR_INPUT_PIN                   GPIO_Pin_7
#define AIR_INPUT_MOTOR_CHANNEL_FLAG        TIM_FLAG_CC2

#define PWM_CONTROL_TIMER                   TIM3
#define PWM_CONTROL_TIMER_PERIOD            1000
#define PWM_CONTROL_IRQHandler              TIM3_IRQHandler

typedef enum {
  WATER_PUMP  = 1,
  AIR_INPUT   = 2
} PWM_Channel_t;

// Triac control
#define TRIAC_GPIO_PORT                     GPIOA
#define TRIAC_AIR_HEATER_PIN                GPIO_Pin_8

//  SIM800 UART
#define SIM800_PORT                         GPIOA
#define SIM800_UART_TX_PIN                  GPIO_Pin_9
#define SIM800_UART_RX_PIN                  GPIO_Pin_10
#define SIM800_UART_INTERFACE               USART1

// Green House Light
#define LIGHT_CTRL_PORT                     GPIOA
#define LIGHT_CTRL_PIN                      GPIO_Pin_11

// LCD216 D4 - Data 4 pin
#define LCD216_D4_PORT                      GPIOA
#define LCD216_D4_PIN                       GPIO_Pin_12

// LCD216 D5 - Data 5 pin
#define LCD216_D5_PORT                      GPIOA
#define LCD216_D5_PIN                       GPIO_Pin_13

// LCD216 D6 - Data 6 pin
#define LCD216_D6_PORT                      GPIOA
#define LCD216_D6_PIN                       GPIO_Pin_14

// LCD216 D7 - Data 7 pin
#define LCD216_D7_PORT                      GPIOA
#define LCD216_D7_PIN                       GPIO_Pin_15



//------------------------------------------------------------------------------
//                              PORT B
// Air Income ventilator control status
#define MIX_AIR_PORT                        GPIOB
#define MIX_AIR_PIN                         GPIO_Pin_0

// LED status
#define SYSTEM_STATUS_LED_PORT              GPIOB
#define SYSTEM_STATUS_LED_PIN               GPIO_Pin_1

// ONEWIRE
#define ONEWIRE_PORT                        GPIOB
#define ONEWIRE_PIN                         GPIO_Pin_2

#define ONEWIRE_TIMER                       TIM4
#define ONEWIRE_TIMER_IRQn                  TIM4_IRQn
#define ONEWIRE_TIMER_IRQHandler            TIM4_IRQHandler

#define ONEWIRE_PIN_SOURCE                  GPIO_PinSource2
#define ONEWIRE_EXTI_LINE                   EXTI_Line2
#define ONEWIRE_EXTI_IRQn                   EXTI2_IRQn
#define ONEWIRE_EXTI_IRQHandler             EXTI2_IRQHandler

// Mains Voltage zero-cross
#define MAINS_ZEROCROSS_DETECT_PORT         GPIOB
#define MAINS_ZEROCROSS_DETECT_PIN          GPIO_Pin_3
#define MAINS_ZEROCROSS_DETECT_PINSOURCE    GPIO_PinSource3
#define MAINS_ZEROCROSS_DETECT_EXTI_LINE    EXTI_Line3
#define MAINS_ZEROCROSS_EXTI_CHANNEL        EXTI3_IRQn
#define MAINS_ZEROCROSS_EXTI_IRQHandler     EXTI3_IRQHandler

// LCD216 RS - Register select pin
#define LCD216_RS_PORT                      GPIOB
#define LCD216_RS_PIN                       GPIO_Pin_4

// ESP wifi chip Enable signal
#define ESP_EN_PORT                         GPIOB
#define ESP_EN_PIN                          GPIO_Pin_5

// I2C
#define I2C1_PORT                           GPIOB

#define I2C1_SCL_PIN                        GPIO_Pin_6
#define I2C1_SCL_PinSource                  GPIO_PinSource6

#define I2C1_SDA_PIN                        GPIO_Pin_7
#define I2C1_SDA_PinSource                  GPIO_PinSource7

#define SYSTEM_BUTTON_PORT                  GPIOB
#define SYSTEM_BUTTON_PIN                   GPIO_Pin_8

// LCD216 E - Enable pin
#define LCD216_EN_PORT                      GPIOB
#define LCD216_EN_PIN                       GPIO_Pin_9

// ESP wifi chip communication interface
#define ESP_UART_PORT                       GPIOB

#define ESP_UART_TX_PIN                     GPIO_Pin_10
#define ESP_UART_RX_PIN                     GPIO_Pin_11

#define ESP_INTERFACE_IRQ_HANDLER           USART3_IRQHandler
#define ESP_UART_INTERFACE                  USART3
#define ESP_UART_DMA_TX_CHANNEL             DMA1_Channel3
#define ESP_UART_DMA_TX_COMPLETE            DMA1_FLAG_TC3
#define ESP8266_UART_BAUDRATE               115200

// Debug pin
#define DEBUG_PORT                          GPIOB
#define DEBUG_PIN                           GPIO_Pin_12

// SPI interface
#define SD_CARD_SPI_INTERFACE               SPI2
#define SD_CARD_SPI_PORT                    GPIOB
#define SD_CARD_SPI_CLK_PIN                 GPIO_Pin_13
#define SD_CARD_SPI_MISO_PIN                GPIO_Pin_14
#define SD_CARD_SPI_MOSI_PIN                GPIO_Pin_15

//------------------------------------------------------------------------------
//                              PORT C

#define ENCODER_POS_MAX_TICKS               512                 // 1024 ticks - full range of output value
#define ENCODER_NEG_MAX_TICKS               (-512)

#define ENCODER_PORT                        GPIOC
#define ENCODER_BUTTON_PORT                 ENCODER_PORT
#define ENCODER_BUTTON_PIN                  GPIO_Pin_13
#define ENCODER_BUTTON_PIN_SOURCE           GPIO_PinSource13
#define ENCODER_BUTTON_EXTI_LINE            EXTI_Line13

#define ENCODER_A_PORT                      ENCODER_PORT
#define ENCODER_A_PIN                       GPIO_Pin_14
#define ENCODER_A_PIN_SOURCE                GPIO_PinSource14
#define ENCODER_A_EXTI_LINE                 EXTI_Line14

#define ENCODER_B_PORT                      ENCODER_PORT
#define ENCODER_B_PIN                       GPIO_Pin_15
#define ENCODER_B_PIN_SOURCE                GPIO_PinSource15
#define ENCODER_B_EXTI_LINE                 EXTI_Line15

#define ENCODER_EXTI_IRQn                   EXTI15_10_IRQn
#define ENCODER_INPUT_IRQHandler            EXTI15_10_IRQHandler



#endif // APPLICATION_MAP_H

//  *** END OF FILE ***
