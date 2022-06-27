#ifndef TRIAC_H
#define TRIAC_H

#include "stm32f10x_tim.h"

#include "tim/mcu_tim.h"


typedef struct{
    TIM_TypeDef*    timer;
    GPIO_TypeDef*   zerocross_gpio_port;
    uint16_t        zerocross_gpio_pin;
    uint32_t        zerocross_exti_line;
} triac_init_t;


//------------------------------------------------------------------------------
void triac_heater_init
(
    triac_init_t* init_struct
);


//------------------------------------------------------------------------------
void triac_heater_set_status
(
    FunctionalState triac_heater_status
);


//------------------------------------------------------------------------------
void triac_set_heater_power
(
  uint16_t set_power_percents
);

#endif  // TRIAC_H
