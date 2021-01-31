#ifndef WATER_H
#define WATER_H

#include "stm32f10x.h"

//--------------------------------------------------------------------------------------------------
/// @brief  Init of watering module
/// init water pump status and power
/// check water level and get ready
//--------------------------------------------------------------------------------------------------
void water_init(void);


//--------------------------------------------------------------------------------------------------
/// @brief  Get water level in percents [0 .. 100]
/// @return Water tank level in percents [0 .. 100]
//--------------------------------------------------------------------------------------------------
uint8_t water_get_level(void);


//--------------------------------------------------------------------------------------------------
/// @brief  Set water pump power in percents
/// @param  Water pump power in percents
//--------------------------------------------------------------------------------------------------
void water_set_pump_power
(
    uint8_t power_percents
);



#endif // WATER_PUMP_H
