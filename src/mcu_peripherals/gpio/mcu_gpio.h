#ifndef MCU_GPIO_H
#define MCU_GPIO_H

#include "stm32f10x_gpio.h"


//--------------------------------------------------------------------------------------------------
/// @brief MCU GPIO initialization function
//--------------------------------------------------------------------------------------------------
void mcu_gpio_init(void);


//--------------------------------------------------------------------------------------------------
/// @brief  Setter for Debug pin status
/// @param  Debug pin status to set (ENABLE/DISABLE or SET/RESET)
//--------------------------------------------------------------------------------------------------
void mcu_gpio_set_debug_pin
(
    FunctionalState dbg_pin_status
);


//--------------------------------------------------------------------------------------------------
/// @brief  Debug pin toggle function
//--------------------------------------------------------------------------------------------------
void mcu_gpio_toggle_debug_pin(void);


//--------------------------------------------------------------------------------------------------
/// @brief  Setter for On-Board LED status
/// @param  On-Board LED status to set (ENABLE/DISABLE or ON/OFF)
//--------------------------------------------------------------------------------------------------
void mcu_gpio_set_led_status
(
    FunctionalState led_status
);


//--------------------------------------------------------------------------------------------------
/// @brief  Getter of on-board button status
/// @return Button status - SET/RESET (i.e. PRESSED/RELEASED)
//--------------------------------------------------------------------------------------------------
FlagStatus mcu_gpio_get_button_status(void);


//--------------------------------------------------------------------------------------------------
/// @brief  Getter of GPIO Port assigned to OneWire feature
/// @return pointer to GPIO Port definition
//--------------------------------------------------------------------------------------------------
GPIO_TypeDef* mcu_gpio_get_onewire_port(void);


//--------------------------------------------------------------------------------------------------
/// @brief  Getter of GPIO Pin assigned to OneWire feature
/// @return GPIO Pin number definition
//--------------------------------------------------------------------------------------------------
uint16_t mcu_gpio_get_onewire_pin(void);


//--------------------------------------------------------------------------------------------------
/// @brief  Setter of Light (lamp) status
/// @param  New status of Light (ENABLE/DISABLE)
//--------------------------------------------------------------------------------------------------
void mcu_gpio_set_light
(
    FunctionalState light_state
);


//--------------------------------------------------------------------------------------------------
/// @brief  Setter of Air Mixing ventilator status
/// @param  New status of Air Mixing ventilator (ENABLE/DISABLE)
//--------------------------------------------------------------------------------------------------
void mcu_gpio_set_air_mixing_status
(
    FunctionalState air_mixing_status
);


//--------------------------------------------------------------------------------------------------
/// @brief  Setter of Heater status
/// @param  New Heater status (ENABLE/DISABLE)
//--------------------------------------------------------------------------------------------------
void mcu_gpio_set_heater_status
(
    FunctionalState heater_state
);


#endif //MCU_GPIO_H
