#ifndef ONEWIRE_PRIV_H
#define ONEWIRE_PRIV_H

#include "stm32f10x.h"
#include "stm32f10x_tim.h"

// RTOS timers
#include "timers.h"

#include "onewire.h"

#define ONEWIRE_TIMER_RUN_REPEAT            pdTRUE

#define ONEWIRE_UPDATE_RATE_MS              2000

#define ONEWIRE_TIMER_RESOLUTION_US         5                                   // [us]

#define ONEWIRE_CONVERT_US_TO_SYSTICKS(us)  ((SystemCoreClock/1000000)*(us))
#define ONEWIRE_COUNT_TO_US(count)          (count*ONEWIRE_TIMER_RESOLUTION_US)

//              ONEWIRE COMMUNICATION TIMINGS
#define ONEWIRE_HOST_HOLD_LOW_US            (1000/ONEWIRE_TIMER_RESOLUTION_US)
#define ONEWIRE_HOST_HOLD_HIGH_US           (50/ONEWIRE_TIMER_RESOLUTION_US)

#define ONEWIRE_SENSOR_RESPONSE_LOW_US      (80/ONEWIRE_TIMER_RESOLUTION_US)
#define ONEWIRE_SENSOR_RESPONSE_HIGH_US     (80/ONEWIRE_TIMER_RESOLUTION_US)

#define ONEWIRE_SENSOR_BIT_LOW_TIME_US      (60/ONEWIRE_TIMER_RESOLUTION_US)
#define ONEWIRE_SENSOR_BIT_0_HIGH_TIME_US   (30/ONEWIRE_TIMER_RESOLUTION_US)
#define ONEWIRE_SENSOR_BIT_1_HIGH_TIME_US   (75/ONEWIRE_TIMER_RESOLUTION_US)

#define ONEWIRE_SENSOR_BIT_0_TIME_US        (ONEWIRE_SENSOR_BIT_LOW_TIME_US + ONEWIRE_SENSOR_BIT_0_HIGH_TIME_US)
#define ONEWIRE_SENSOR_BIT_1_TIME_US        (ONEWIRE_SENSOR_BIT_LOW_TIME_US + ONEWIRE_SENSOR_BIT_1_HIGH_TIME_US)




typedef enum {
  ONEWIRE_WRITE_RESET,
  ONEWIRE_READ_RESPONSE,
  ONEWIRE_READ_DATA,
  ONEWIRE_MODULE_IDLE
} onewire_operation;


typedef struct{
    FunctionalState   module_enable;

    uint16_t          uw_idle_ms_counter;
    uint32_t          ul_timeframes_counter;
    onewire_operation e_operation;
    FunctionalState   sensor_present;
    uint8_t           ub_bit_counter;

    uint16_t          uw_sensor_humidity;
    uint16_t          uw_sensor_temperature;
    uint16_t          uw_sensor_parity;

    uint8_t           data_valid;

    TIM_TypeDef*      timer;
    GPIO_TypeDef*     gpio_port;
    uint16_t          gpio_pin;
    IRQn_Type         exti_channel;
    uint32_t          exti_line;
} onewire_bus_t;


//--------------------------------------------------------------------------------------------------
/// Configure GPIO pin according to module's operation
//--------------------------------------------------------------------------------------------------
static void onewire_config_pin
(
    onewire_operation e_operation
);

//--------------------------------------------------------------------------------------------------
static void onewire_set_status
(
    FunctionalState eState
);


//--------------------------------------------------------------------------------------------------
/// During ONEWIRE module's init the RTOS timer is configured (see ONEWIRE_Init)
/// This function is executed every timer's expiration event.
//--------------------------------------------------------------------------------------------------
static void onewire_5ms_callback(void);


//--------------------------------------------------------------------------------------------------
/// ONEWIRE module read operation is executed using EXTI interrupts
/// configured on Rising/Falling edge
/// ONEWIRE timer count is taken afterwards and decision about bit-read value is made.
//--------------------------------------------------------------------------------------------------
static void onewire_pin_interrupt_status
(
    FunctionalState e_status
);


//--------------------------------------------------------------------------------------------------
static BitAction onewire_line_HIGH(void);


//--------------------------------------------------------------------------------------------------
static void onewire_set_operation
(
    onewire_operation e_operation
);


//--------------------------------------------------------------------------------------------------
static void onewire_check_parity(void);


//--------------------------------------------------------------------------------------------------
static void onewire_sensor_malfunction(void);

#endif // ONEWIRE_PRIV_H
