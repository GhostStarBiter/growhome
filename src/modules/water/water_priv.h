#ifndef WATER_PRIV_H
#define WATER_PRIV_H

#include "stm32f10x.h"

#include "main/application_map.h"

#include "mcu_peripherals/adc/mcu_adc.h"
#include "mcu_peripherals/tim/mcu_tim.h"

#include "water.h"


typedef struct {
    uint8_t         level;
    uint8_t         pump_power;
} water_t;

#endif // WATER_PRIV_H
