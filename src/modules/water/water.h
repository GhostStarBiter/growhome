#ifndef WATER_H
#define WATER_H

#include "stm32f10x.h"

//--------------------------------------------------------------------------------------------------
/// @brief  Init of watering module
/// init water pump status and power
/// check water level and get ready
//--------------------------------------------------------------------------------------------------
void water_pump_init(void);


//--------------------------------------------------------------------------------------------------
/// @brief  Get water level in percents [0 .. 100]
/// @return Water tank level in percents [0 .. 100]
//--------------------------------------------------------------------------------------------------
uint8_t water_get_level(void);


//--------------------------------------------------------------------------------------------------
/// @brief  Setter of water pump state
/// @param  New water pump state
//--------------------------------------------------------------------------------------------------
void water_pump_set_status
(
    FunctionalState set_state
);

//--------------------------------------------------------------------------------------------------
/// @brief  Set water pump power in percents
/// @param  Water pump power in percents
//--------------------------------------------------------------------------------------------------
void water_set_pump_power
(
    uint8_t power_percents
);

//--------------------------------------------------------------------------------------------------
/// @brief  Getter for water pump power value
/// @return Water pump power value
//--------------------------------------------------------------------------------------------------
uint8_t water_get_pump_power(void);



#endif // WATER_PUMP_H
