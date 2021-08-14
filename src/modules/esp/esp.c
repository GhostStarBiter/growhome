#include "stm32f10x.h"
#include "task.h"

#include "configuration/peripherals_config.h"

#include "stm32f10x_usart.h"

#include "gpio/mcu_gpio.h"
#include "uart/mcu_uart.h"
#include "utils/utils.h"

#include "string.h"
#include "esp.h"

#define ESP_SEND_BUF_SIZE                   512
#define ESP_MAX_COMMAND_LEN                 (ESP_SEND_BUF_SIZE - AT_COMMAND_START_INDEX - sizeof(AT_REQEST_ENDING))

#define ESP_RECV_BUF_SIZE                   512

#define ESP_RESPONSE_TIMEOUT                500   // [ms]

#define ESP_COMPARE_RESPONSE_BUF_SIZE       64    // [bytes]

#define AT_COMMAND_START_INDEX              2     // position in buffer "AT+[command]"


#define ESP_RECV_BUFFER_EMPTY               (!esp.recv_size)
#define ESP_RECV_BUFFER_NOT_FULL            (esp.recv_size < ESP_RECV_BUF_SIZE)

typedef struct{
    FunctionalState           chip_status;
    esp_state_t               state;
    bool                      data_received;
    uint8_t                   send_buf[ESP_RECV_BUF_SIZE];
    uint16_t                  send_size;
    uint8_t                   recv_buf[ESP_SEND_BUF_SIZE];
    uint16_t                  recv_size;
    void                      (*process_incoming_data)(uint8_t*);
} esp_struct_t;


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
static result_t esp_timeout_wait_receive_finished(void);

//--------------------------------------------------------------------------------------------------
/// @brief
//--------------------------------------------------------------------------------------------------
result_t esp_search_response(char* search, char* buffer);



//  *****************
esp_struct_t esp;


//--------------------------------------------------------------------------------------------------
void esp_init(void (*callback_process_incoming_data)(uint8_t*))
{
  esp.chip_status = DISABLE;
  esp.state = ESP_RESET;
  esp.process_incoming_data = callback_process_incoming_data;

  mcu_uart_set_dma_buffer_address((uint32_t) &esp.send_buf);

  memset(esp.send_buf, 0, ESP_SEND_BUF_SIZE);


  esp_update();
}


//--------------------------------------------------------------------------------------------------
static void esp_reset_and_enable_chip(void)
{
  mcu_gpio_set_esp_reset_status(ENABLE);
  vTaskDelay(200);

  mcu_gpio_set_esp_enable_status(ENABLE);
  vTaskDelay(50);
  esp.chip_status = ENABLE;

  mcu_gpio_set_esp_reset_status(DISABLE);

  // wait for ESP startup after EN signal presence
  vTaskDelay(1000);
}

//--------------------------------------------------------------------------------------------------
void esp_update(void)
{
  result_t operation_result;

  switch(esp.state)
  {
    case ESP_RESET:
      esp_reset_and_enable_chip();

      memcpy(esp.send_buf, AT_COMMAND_PREFIX, sizeof(AT_COMMAND_PREFIX));
      strcat((char*) esp.send_buf, AT_RESET);
      strcat((char*) esp.send_buf, AT_REQEST_ENDING);

      esp.send_size = strlen((char*) esp.send_buf);

      mcu_uart_dma_start_transmit(esp.send_size);

      operation_result = esp_timeout_wait_receive_finished();

      if(operation_result)
      {
        operation_result = esp_search_response("ready", (char*) esp.recv_buf);

        operation_result == result_success ? esp_set_state(ESP_READY) : esp_set_state(ESP_ERROR);
      }

      break;

    case ESP_READY:

      break;

    case ESP_INCOMING_DATA:

      if(esp.data_received)
      {
        esp.process_incoming_data(esp.recv_buf);
        esp_set_state(ESP_READY);
      }
      else
      {
        vTaskDelay(1);
      }
      break;

    case ESP_ERROR:

      break;

    default:

      break;
  }
}


//--------------------------------------------------------------------------------------------------
static result_t esp_timeout_wait_receive_finished(void)
{
  static uint32_t counter = 0;
  result_t  result = result_success;

  while(!esp.data_received)
  {
    vTaskDelay(1);
    counter++;

    if(counter > ESP_RESPONSE_TIMEOUT)
    {
      counter = 0;
      esp.state = ESP_ERROR;
      result = result_fail;
      break;
    }
  }

  return result;
}


//--------------------------------------------------------------------------------------------------
result_t esp_search_response(char* search, char* buffer)
{
  result_t result = result_fail;

  result = (result_t) strstr(buffer, search);

  return result;
}


//--------------------------------------------------------------------------------------------------
static void esp_set_state(esp_state_t new_state)
{
  esp.state = new_state;
}


//--------------------------------------------------------------------------------------------------
static void esp_receive_data(uint8_t byte)
{
  uint16_t index;

  if(ESP_RECV_BUFFER_NOT_FULL && !ESP_RECV_BUFFER_EMPTY)
  {
    index = esp.recv_size;
  }
  else
  {
    index = esp.recv_size = 0;
  }

  esp.recv_size++;
  esp.recv_buf[index] = byte;
}


//--------------------------------------------------------------------------------------------------
void ESP_INTERFACE_IRQ_HANDLER(void)
{
  uint8_t data;
  static bool ssid_name_started = false;

  //  RECEIVE DATA
  if(USART_GetITStatus(ESP_UART_INTERFACE, USART_IT_RXNE) == SET)
  {
    USART_ClearFlag(ESP_UART_INTERFACE, USART_IT_RXNE);
    USART_ITConfig(ESP_UART_INTERFACE, USART_IT_IDLE, ENABLE);

    data = (uint8_t) USART_ReceiveData(ESP_UART_INTERFACE);

    esp_receive_data(data);
  }

  //  DATA RECEIVING FINISHED
  if(USART_GetITStatus(ESP_UART_INTERFACE, USART_IT_IDLE) == SET)
  {
    USART_ClearFlag(ESP_UART_INTERFACE, USART_IT_IDLE);

    esp.data_received = true;

    USART_ITConfig(ESP_UART_INTERFACE, USART_IT_IDLE, DISABLE);

    // what for??
    //(void) USART_ReceiveData(ESP_UART_INTERFACE);
  }
}
