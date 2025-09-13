#ifndef PERIPHERALS_CONFIG_H
#define PERIPHERALS_CONFIG_H

#include "stm32f10x_gpio.h"
#include "stm32f10x_exti.h"

//          Servo motor
#define SERVO_GPIO_PORT                     GPIOA
#define SERVO_GPIO_PIN                      GPIO_Pin_0

#define SERVO_TIMER                         TIM2
#define SERVO_TIMER_PRESCALER               (36 - 1)                                // 1MHz = 1us tick
#define SERVO_TIMER_PERIOD                  20000                                   // 20 ms


//          ANALOG measurement
#define ANALOG_MEASURE_PORT                 GPIOA

// Air temperature 1 measurement pin
#define INCOME_AIR_TEMP_PIN                 GPIO_Pin_2

// Air temperature 2 measurement pin
#define OUTLET_AIR_TEMP_PIN                 GPIO_Pin_3

// Water tank 1 level measurement pin
#define WATER_TANK_LEVEL_MEASURE_PIN        GPIO_Pin_4


// ESP wifi module RST pin
#define ESP_RST_PORT                        GPIOA
#define ESP_RST_PIN                         GPIO_Pin_5



// PWM water pump (AF TIM3 CH1)
#define PWM_CONTROL_PORT                    GPIOA
#define PWM_WATER_PUMP_PIN                  GPIO_Pin_6
#define WATER_PUMP_CTRL_CHANNEL_FLAG        TIM_FLAG_CC1

//  PWM Air Input Green House (AF TIM3_CH2)
#define PWM_AIR_INPUT_PIN                   GPIO_Pin_7
#define AIR_INPUT_MOTOR_CHANNEL_FLAG        TIM_FLAG_CC2

#define PWM_CONTROL_TIMER                   TIM3
#define PWM_CONTROL_TIMER_PERIOD            1000
#define PWM_CONTROL_IRQHandler              TIM3_IRQHandler

// Triac control
#define TRIAC_GPIO_PORT                     GPIOA
#define TRIAC_AIR_HEATER_PIN                GPIO_Pin_8


//  SIM800 UART
#define SIM800_PORT                         GPIOA

#define SIM800_RESET                        GPIO_Pin_1

#define SIM800_UART_TX_PIN                  GPIO_Pin_9
#define SIM800_UART_RX_PIN                  GPIO_Pin_10
#define SIM800_UART_INTERFACE               USART1

// Green House Light
#define LIGHT_CTRL_PORT                     GPIOA
#define LIGHT_CTRL_PIN                      GPIO_Pin_11


//        LCD 2x16
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

#define I2C1_DMA_Channel                    DMA1_Channel7
#define I2C1_DMA_RX_IRQn                    DMA1_Channel7_IRQn
#define I2C1_DMA_RX_IRQHandler              DMA1_Channel7_IRQHandler

#define SYSTEM_BUTTON_PORT                  GPIOB
#define SYSTEM_BUTTON_PIN                   GPIO_Pin_8

// LCD216 E - Enable pin
#define LCD216_EN_PORT                      GPIOB
#define LCD216_EN_PIN                       GPIO_Pin_9

// ESP wifi chip communication interface
#define ESP_UART_PORT                       GPIOB

#define ESP_UART_TX_PIN                     GPIO_Pin_10
#define ESP_UART_RX_PIN                     GPIO_Pin_11

#define ESP_UART_IRQn                       USART3_IRQn
#define ESP_UART_DMA_IRQn                   DMA1_Channel2_IRQn
#define ESP_INTERFACE_IRQ_HANDLER           USART3_IRQHandler
#define ESP_UART_INTERFACE                  USART3
#define ESP_UART_DMA_TX_CHANNEL             DMA1_Channel2
#define ESP_UART_DMA_TX_COMPLETE            DMA1_FLAG_TC2

// Debug pin
#define DEBUG_PORT                          GPIOB
#define DEBUG_PIN                           GPIO_Pin_14

// SPI interface
#define SPI_INTERFACE                       SPI2
#define SPI_PORT                            GPIOB
#define SPI_NSS_PIN                         GPIO_Pin_12
#define SPI_CLK_PIN                         GPIO_Pin_13
//#define SPI_MISO_PIN                        GPIO_Pin_14
#define SPI_MOSI_PIN                        GPIO_Pin_15


// Encoder with push-button
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

#endif // PERIPHERALS_CONFIG_H
