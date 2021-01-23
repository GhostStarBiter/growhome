#ifndef GROW_GREEN_H
#define GROW_GREEN_H

typedef enum {
  GROW_MODE_MANUAL    = 0,
  GROW_MODE_AUTOMATIC = 1
} control_mode_t;


//--------------------------------------------------------------------------------------------------
/// @brief
//--------------------------------------------------------------------------------------------------
void growbox_system_init(void);


//--------------------------------------------------------------------------------------------------
/// @brief  Grow Green House running task
//--------------------------------------------------------------------------------------------------
void growbox_control_rtos_task(
    void *pvParameters
);


//--------------------------------------------------------------------------------------------------
/// @brief  Setter of light status ON/OFF
/// @param  New status of light (ENABLE/DISABLE)
//--------------------------------------------------------------------------------------------------
void growbox_set_light
(
    FunctionalState light_state
);


//--------------------------------------------------------------------------------------------------
/// @brief    Getter of light status ON/OFF
/// @return   Status of light (ENABLE/DISABLE)
//--------------------------------------------------------------------------------------------------
FunctionalState growbox_get_light_status(void);


//--------------------------------------------------------------------------------------------------
/// @brief    Getter of mixed air temperature
/// @return   Mixed air temperature
//--------------------------------------------------------------------------------------------------
uint8_t growbox_get_mixed_air_temp(void);


//--------------------------------------------------------------------------------------------------
/// @brief    Getter of water tank level (in %)
/// @return   Water tank level in %
//--------------------------------------------------------------------------------------------------
uint8_t growbox_get_water_level(void);

//--------------------------------------------------------------------------------------------------
/// @brief  Setter of Growbox control mode
/// @param  Control mode - MANUAL/AUTOMATIC
//--------------------------------------------------------------------------------------------------
void growbox_set_control_mode
(
    control_mode_t set_control_mode
);


#endif  // GROW_GREEN_H
