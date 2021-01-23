#ifndef ESP8266_MODULE_H
#define ESP8266_MODULE_H

#include "main/types.h"
#include "stm32f10x.h"

typedef enum
{
  esp_no_response,
  esp_simple_response,  // OK/ERROR
  esp_parsed_response   // need to be parsed and analyzed
} esp_response_type_t;


typedef enum {
  ESP_READY,
  ESP_BUSY,
  ESP_ERROR
} esp_module_status;


typedef enum {
  WIFI_IDLE,
  WIFI_SEND_REQUEST,
  WIFI_WAIT_RESPONSE,
  WIFI_PROCESS_RESPONSE,
  WIFI_INCOMING_DATA,
  WIFI_TUNELLING
} wifi_mode_t;


//------------------------------------------------------------------------------
/// @brief Function to init ESP8266 module
//------------------------------------------------------------------------------
result_t esp_init(void (*callback_process_incoming_data)(uint8_t*));


//------------------------------------------------------------------------------
/// @brief  ESP runner
//------------------------------------------------------------------------------
void esp_update(void);


//------------------------------------------------------------------------------
/// @brief  Send AT command to ESP8266 (using DMA TX)
/// @param  command string to be attached to basic AT symbols
/// @param  expected response(if any) from esp module
//------------------------------------------------------------------------------
result_t esp_send_command(char* char_command_string, esp_response_type_t e_expected_response);


//--------------------------------------------------------------------------------------------------
/// @brief  Function to put received from ESP module data to receive buffer
//--------------------------------------------------------------------------------------------------
result_t esp_receive_data(uint8_t byte);

#endif // ESP8266_MODULE_H
