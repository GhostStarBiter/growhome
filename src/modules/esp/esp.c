#include "stm32f10x.h"
#include  "FreeRTOS.h"
#include "task.h"

#include "stm32f10x_usart.h"

#include "gpio/mcu_gpio.h"
#include "uart/mcu_uart.h"
#include "utils/utils.h"
#include "system_user_interface/sui.h"

#include "string.h"
#include "esp.h"

#define ESP_SEND_BUF_SIZE                   2048
#define ESP_MAX_COMMAND_LEN                 (ESP_SEND_BUF_SIZE - AT_COMMAND_START_INDEX - sizeof(AT_REQEST_ENDING))

#define ESP_RECV_BUF_SIZE                   2048

#define ESP_RECV_TIMEOUT_MAX                1000  // [ms]

#define ESP_COMPARE_RESPONSE_BUF_SIZE       64    // [bytes]

#define AT_COMMAND_START_INDEX              2     // position in buffer "AT+[command]"

#define ESP_RECV_BUFFER_NOT_FULL            (esp_chip.recv_size < ESP_RECV_BUF_SIZE)

typedef enum {
  ESP_NOSTATE,
  ESP_HW_RESET,
  ESP_AT_RESET,
  ESP_READY,
  ESP_PREP_CONNECT,
  ESP_CONNECTED,
  ESP_PROCESS_RESPONSE,
  ESP_INCOMING_DATA,
  ESP_TUNELLING,
  ESP_ERROR
} esp_state_t;

typedef enum {
  NOT_RECV,
  BY_IDLE,
  BY_TIMEOUT,
  INCOMING_DATA
} esp_recv_end_t;

typedef struct{
    USART_TypeDef*            interface;
    FunctionalState           status;
    esp_state_t               state;
    bool                      recv_finished;
    uint8_t                   send_buf[ESP_RECV_BUF_SIZE];
    uint16_t                  send_size;
    uint8_t                   recv_buf[ESP_SEND_BUF_SIZE];
    uint16_t                  recv_size;
    esp_recv_end_t            recv_type;
    void                      (*process_incoming_data)(uint8_t*);
} esp_struct_t;


static void esp_send_at_command(const char* const command);

//--------------------------------------------------------------------------------------------------
/// @brief Put received over UART data to receive buffer
//--------------------------------------------------------------------------------------------------
static void esp_receive_data(uint8_t byte);

//--------------------------------------------------------------------------------------------------
/// @brief
//--------------------------------------------------------------------------------------------------
static void esp_set_state(esp_state_t e_new_sw_status);


//--------------------------------------------------------------------------------------------------
/// @brief Wait ESP_RESPONSE_TIMEOUT ms or less if data receive finished
//--------------------------------------------------------------------------------------------------
static result_t esp_timeout_wait_receive_finished(esp_recv_end_t recv_finished, uint16_t timeout);

//--------------------------------------------------------------------------------------------------
/// @brief
//--------------------------------------------------------------------------------------------------
result_t esp_search_in_buf(char* search, char* buffer);


//--------------------------------------------------------------------------------------------------
/// @brief
//--------------------------------------------------------------------------------------------------
static result_t esp_initiate_network_access(char*, char*);


//  *****************
esp_struct_t esp_chip;


//--------------------------------------------------------------------------------------------------
void esp_init(USART_TypeDef* esp_uart_interface, void (*callback_process_incoming_data)(uint8_t*))
{
  // check of NULL pointers
  while(!esp_uart_interface);
  while(!callback_process_incoming_data);

  esp_chip.interface = esp_uart_interface;
  esp_chip.process_incoming_data = callback_process_incoming_data;

  esp_chip.status = DISABLE;
  esp_chip.state = ESP_HW_RESET;
  esp_chip.recv_type = BY_TIMEOUT;

  memset(esp_chip.send_buf, 0, ESP_SEND_BUF_SIZE);

  esp_update();
}


//--------------------------------------------------------------------------------------------------
static void esp_reset_and_enable_chip(void)
{
  mcu_gpio_set_esp_reset_status(ENABLE);
  vTaskDelay(200);

  mcu_gpio_set_esp_enable_status(ENABLE);
  vTaskDelay(50);
  esp_chip.status = ENABLE;

  mcu_gpio_set_esp_reset_status(DISABLE);

  // wait for ESP startup after EN signal presence
  vTaskDelay(1000);
}


//--------------------------------------------------------------------------------------------------
void esp_update(void)
{
  result_t operation_result;

  switch(esp_chip.state)
  {
    case ESP_HW_RESET:
      esp_reset_and_enable_chip();
      esp_set_state(ESP_AT_RESET);
      break;

    case ESP_AT_RESET:
      esp_send_at_command(AT_RESET);

      operation_result = esp_timeout_wait_receive_finished(BY_TIMEOUT, 500);

      if(operation_result)
      {
        operation_result = esp_search_in_buf("ready", (char*) esp_chip.recv_buf);

        operation_result == result_success ? esp_set_state(ESP_READY) : esp_set_state(ESP_ERROR);
      }

      if(operation_result)
      {
        esp_send_at_command(AT_ECHO_OFF);
        operation_result = esp_timeout_wait_receive_finished(BY_TIMEOUT, 10);

        operation_result = esp_search_in_buf("OK", (char*) esp_chip.recv_buf);

        operation_result == result_success ? esp_set_state(ESP_READY) : esp_set_state(ESP_ERROR);
      }

      break;

    case ESP_READY:
      esp_send_at_command(ESP_SET_WIFI_MODE_SOFT_AP);

      operation_result = esp_search_in_buf("OK", (char*) esp_chip.recv_buf);
      operation_result == result_success ? esp_set_state(ESP_PREP_CONNECT) : esp_set_state(ESP_ERROR);

      break;

    case ESP_PREP_CONNECT:
      // wait for activation of connection procedure
      if(sui_user_enter_credentials())
      {
        char* user_ssid = sui_get_user_ssid();
        char* user_pswd = sui_get_user_pswd();
        operation_result = esp_initiate_network_access(user_ssid, user_pswd);
        if(operation_result == result_success)
          {
            esp_set_state(ESP_CONNECTED);
          }
        else
        {
          esp_set_state(ESP_ERROR);
        }
      }
      else
      {
        vTaskDelay(1);
      }
      break;

    case ESP_CONNECTED:
      break;

    case ESP_INCOMING_DATA:
      esp_timeout_wait_receive_finished(BY_IDLE, 0);
      esp_chip.process_incoming_data(esp_chip.recv_buf);
      esp_set_state(ESP_CONNECTED);
      break;

    case ESP_ERROR:

      break;

    default:

      break;
  }
}


//--------------------------------------------------------------------------------------------------
static void esp_send_at_command(const char* const command)
{
  esp_chip.recv_finished = false;

  memcpy(esp_chip.send_buf, AT_COM, strlen(AT_COM));
  strcat((char*) esp_chip.send_buf, command);
  strcat((char*) esp_chip.send_buf, AT_REQEST_ENDING);

  esp_chip.send_size = strlen((char*) esp_chip.send_buf);

  mcu_uart_dma_start_transmit(esp_chip.interface, (uint32_t) &esp_chip.send_buf[0], esp_chip.send_size);
}


//--------------------------------------------------------------------------------------------------
static result_t esp_timeout_wait_receive_finished(esp_recv_end_t recv_finished_type, uint16_t timeout_ms)
{
  uint16_t counter = 0;
  result_t  result = result_success;

  switch(recv_finished_type)
  {
  case BY_IDLE:
    while(!esp_chip.recv_finished)
    {
      vTaskDelay(1);
      counter++;

      if(counter > ESP_RECV_TIMEOUT_MAX)
      {
        esp_chip.state = ESP_ERROR;
        result = result_fail;
        break;
      }
    }
    break;

  case BY_TIMEOUT:
    while(counter < timeout_ms)
    {
      vTaskDelay(1);
      counter++;
    };

    if(!esp_chip.recv_finished)
    {
      esp_chip.state = ESP_ERROR;
      result = result_fail;
    }
    break;

  default:
    break;
  }

  return result;
}


//--------------------------------------------------------------------------------------------------
static result_t esp_initiate_network_access(char* ssid, char* pswd)
{
  result_t result = result_success;

  char tmp_buf[50];
  memset((void*) tmp_buf, 0, 50);

  strcat(tmp_buf, ESP_JOIN_AP);
  strcat(tmp_buf, "\"");    // add first ["]
  strcat(tmp_buf, ssid);
  strcat(tmp_buf, "\",\""); // add second ["] and [,"]
  strcat(tmp_buf, pswd);
  strcat(tmp_buf, "\"");    // add closing ["]

  esp_send_at_command(tmp_buf);

  result = esp_timeout_wait_receive_finished(BY_TIMEOUT, 100);

  result = esp_search_in_buf("OK", (char*) esp_chip.recv_buf);

  return result;
}


//--------------------------------------------------------------------------------------------------
result_t esp_search_in_buf(char* search, char* buffer)
{
  result_t result = result_fail;

  result = (result_t) strstr(buffer, search);

  return result;
}


//--------------------------------------------------------------------------------------------------
static void esp_set_state(esp_state_t new_state)
{
  esp_chip.state = new_state;
}


//--------------------------------------------------------------------------------------------------
static void esp_receive_data(uint8_t data)
{
  uint16_t index;

  if(ESP_RECV_BUFFER_NOT_FULL)
  {
    index = esp_chip.recv_size;
  }
  else
  {
    index = esp_chip.recv_size = 0;
  }

  esp_chip.recv_size++;
  esp_chip.recv_buf[index] = data;
}


//--------------------------------------------------------------------------------------------------
void ESP_INTERFACE_IRQ_HANDLER(void)
{
  uint8_t data;

  //  RECEIVE DATA
  if(USART_GetITStatus(ESP_UART_INTERFACE, USART_IT_RXNE) == SET)
  {
    USART_ClearFlag(ESP_UART_INTERFACE, USART_IT_RXNE);

    switch(esp_chip.recv_type)
    {
    case BY_IDLE:
      USART_ITConfig(ESP_UART_INTERFACE, USART_IT_IDLE, ENABLE);
      data = (uint8_t) USART_ReceiveData(ESP_UART_INTERFACE);
      esp_receive_data(data);
      break;

    case BY_TIMEOUT:
      data = (uint8_t) USART_ReceiveData(ESP_UART_INTERFACE);
      esp_receive_data(data);
      break;

    case NOT_RECV:
      (void) USART_ReceiveData(ESP_UART_INTERFACE);
      break;

    case INCOMING_DATA:
      esp_set_state(ESP_INCOMING_DATA);
      data = (uint8_t) USART_ReceiveData(ESP_UART_INTERFACE);
      esp_receive_data(data);
      break;

    default:
      break;
    }
  }


  //  DATA RECEIVING FINISHED
  if(USART_GetITStatus(ESP_UART_INTERFACE, USART_IT_IDLE) == SET)
  {
    USART_ClearFlag(ESP_UART_INTERFACE, USART_IT_IDLE);

    if(esp_chip.recv_type == BY_IDLE)
    {
      esp_chip.recv_finished = true;
    }

    USART_ITConfig(ESP_UART_INTERFACE, USART_IT_IDLE, DISABLE);

    // what for??
    //(void) USART_ReceiveData(ESP_UART_INTERFACE);
  }
}
