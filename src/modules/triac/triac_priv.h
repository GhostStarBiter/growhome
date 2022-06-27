#ifndef TRIAC_PRIV_H
#define TRIAC_PRIV_H

#include "stm32f10x.h"

//#include "main/application_map.h"

#include "triac.h"



typedef struct {
    TIM_TypeDef*    timer;
    GPIO_TypeDef*   zerocross_gpio_port;
    uint16_t        zerocross_gpio_pin;
    uint32_t        zerocross_exti_line;
    uint8_t         set_power;
    FunctionalState status;
} triac_t;


//------------------------------------------------------------------------------
static uint32_t triac_heater_convert_power
(
    uint8_t power_in_percents
);


#endif  // TRIAC_PRIV_H
