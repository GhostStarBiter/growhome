#ifndef ESP8266_MODULE_H
#define ESP8266_MODULE_H

#include "main/types.h"
#include "stm32f10x.h"

#include "configuration/peripherals_config.h"

#include "at_commands.h"


//------------------------------------------------------------------------------
/// @brief Function to init ESP8266 module
//------------------------------------------------------------------------------
void esp_init(USART_TypeDef* esp_uart_interface, void (*callback_process_incoming_data)(uint8_t*));


//------------------------------------------------------------------------------
/// @brief  ESP runner
//------------------------------------------------------------------------------
void esp_update(void);


#endif // ESP8266_MODULE_H
