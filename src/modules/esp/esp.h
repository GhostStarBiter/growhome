#ifndef ESP8266_MODULE_H
#define ESP8266_MODULE_H

#include "main/types.h"
#include "stm32f10x.h"

#include "at_commands.h"


typedef enum {
  ESP_NOSTATE,
  ESP_RESET,
  ESP_WAIT_READY,
  ESP_READY,
  ESP_LIST_AP,
  ESP_CONNECT,
  ESP_PROCESS_RESPONSE,
  ESP_INCOMING_DATA,
  ESP_TUNELLING,
  ESP_ERROR
} esp_state_t;


//------------------------------------------------------------------------------
/// @brief Function to init ESP8266 module
//------------------------------------------------------------------------------
void esp_init(void (*callback_process_incoming_data)(uint8_t*));


//------------------------------------------------------------------------------
/// @brief  ESP runner
//------------------------------------------------------------------------------
void esp_update(void);


#endif // ESP8266_MODULE_H
