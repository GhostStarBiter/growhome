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
/// @brief  Set water pump status ENABLED/DISABLED
//--------------------------------------------------------------------------------------------------
void water_pump_set_status(FunctionalState pump_state);



#endif // WATER_PUMP_H
