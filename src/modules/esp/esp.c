#include "esp_priv.h"

esp_struct_t esp;

//--------------------------------------------------------------------------------------------------
result_t esp_init(void (*callback_process_incoming_data)(uint8_t*))
{
  result_t operation_result;
  esp.send_buf[0] = 'A';
  esp.send_buf[1] = 'T';

  mcu_gpio_set_esp_reset_status(DISABLE);

  esp.chip_status = ENABLE;
  mcu_gpio_set_esp_enable_status(esp.chip_status);

  mcu_uart_set_dma_buffer_address((uint32_t) &esp.send_buf[0]);

  // wait for ESP startup after EN signal presence
  vTaskDelay(500);

  operation_result = esp_send_command(AT_STARTUP_TEST, esp_simple_response);

  if(callback_process_incoming_data != NULL)
  {
    esp.process_incoming_data = callback_process_incoming_data;
  }
  else
  {
    operation_result = result_fail;
  }

  return operation_result;
}


//--------------------------------------------------------------------------------------------------
void esp_update(void)
{
  uint8_t response_ok;
  uint8_t* buf_ptr;

  switch(esp.wifi_mode)
  {
    case WIFI_SEND_REQUEST:
      if(mcu_uart_is_transmission_complete())
      {
        esp_next_event(WIFI_WAIT_RESPONSE);
      }
    break;

    case WIFI_WAIT_RESPONSE:
      if(esp.receive_finished)
      {
        esp_next_event(WIFI_PROCESS_RESPONSE);
      }
      else
      {
        vTaskDelay(1);
      }
    break;

    case WIFI_PROCESS_RESPONSE:
      if(esp.response_type == esp_simple_response)
      {
        // @TODO:
        //              Example
        //      AT      ->        AT\r\r\n\r\n OK\r\n
        //    Write parser
        // response_ok = !strncmp((char*) esp.recv_buf, AT_RESPONSE_OK, sizeof(AT_RESPONSE_OK));


        if(response_ok)
        {
          esp.sw_status = ESP_READY;
        }
        else
        {
          esp.sw_status = ESP_ERROR;
        }
        esp.uw_recv_size = 0;
        memset(esp.recv_buf, 0, ESP_RECV_BUF_SIZE);
        memset(esp.send_buf, 0, ESP_SEND_BUF_SIZE);
        esp_next_event(WIFI_IDLE);
      }
      else
      {
        if(result_success == esp_process_incoming_data())
        {
          esp.sw_status = ESP_READY;
        }
        else
        {
          esp.sw_status = ESP_ERROR;
        }
      }

    break;

    case WIFI_INCOMING_DATA:
      if(esp.receive_finished)
      {
        buf_ptr = esp.recv_buf;
        esp.process_incoming_data(buf_ptr);
        esp_next_event(WIFI_IDLE);
      }
      else
      {
        vTaskDelay(1);
      }
    break;

    case WIFI_IDLE:
    default:

    break;
  }
}


//--------------------------------------------------------------------------------------------------
result_t esp_send_command(char* command_string, esp_response_type_t e_expected_response)
{
  result_t operation_result = result_success;
  char* ptr_send_buffer;
  uint8_t request_size;

  if(strlen(command_string) < ESP_MAX_COMMAND_LEN &&
      esp.wifi_mode == WIFI_IDLE)
  {
    esp.response_type = e_expected_response;

    ptr_send_buffer = (char*) &esp.send_buf[AT_COMMAND_START_INDEX];
    if(command_string)
    {
      strcpy(ptr_send_buffer, command_string);
    }

    request_size = strlen("AT") + strlen(command_string);
    ptr_send_buffer = (char*) &esp.send_buf[request_size];
    strcpy(ptr_send_buffer, AT_REQEST_ENDING);

    esp.uw_send_size = request_size + strlen(AT_REQEST_ENDING);
    esp.receive_finished = 0;

    esp_set_status(ESP_BUSY);
    esp_next_event(WIFI_SEND_REQUEST);
    mcu_uart_dma_start_transmit(esp.uw_send_size);
  }
  else
  {
    operation_result = result_fail;
  }

  return operation_result;
}


//--------------------------------------------------------------------------------------------------
result_t esp_send_data(uint8_t* ptr_data_buf, uint16_t data_size_bytes)
{
  result_t operation_result = result_success;
  char* esp_send_data_command = ESP_COMMAND_SEND_DATA;
  char* str_data_size;
  str_data_size = convert_num_to_str(data_size_bytes);
  strcat(esp_send_data_command, str_data_size);

  esp_send_command(esp_send_data_command, esp_no_response);


  return operation_result;
}


//--------------------------------------------------------------------------------------------------
static void esp_next_event(wifi_mode_t e_event)
{
  esp.wifi_mode = e_event;
}


//--------------------------------------------------------------------------------------------------
static void esp_set_status(esp_sw_status e_new_status)
{
  esp.sw_status = e_new_status;
}


//--------------------------------------------------------------------------------------------------
result_t esp_receive_data(uint8_t byte)
{
  result_t operation_result = result_fail;

  if(ESP_RECV_BUFFER_NOT_FULL)
  {
    if( ESP_RECV_BUFFER_EMPTY &&
        esp.sw_status == ESP_READY &&
        esp.wifi_mode == WIFI_IDLE)
    {
      esp_set_status(ESP_BUSY);
      esp_next_event(WIFI_INCOMING_DATA);
    }

    uint16_t index = esp.uw_recv_size;
    esp.recv_buf[index] = byte;
    ++esp.uw_recv_size;
    operation_result = result_success;
  }

  return operation_result;
}


//--------------------------------------------------------------------------------------------------
static result_t esp_process_incoming_data(void)
{
  result_t operation_result;
  //uint8_t arg_pos = 0;
  //uint8_t index = 0;

  //TODO: add processing of complex response
  operation_result = result_success;

  return operation_result;
}


//--------------------------------------------------------------------------------------------------
void ESP_INTERFACE_IRQ_HANDLER(void)
{
  uint8_t data;

  if(USART_GetITStatus(ESP_UART_INTERFACE, USART_IT_RXNE) == SET)
  {
    USART_ClearFlag(ESP_UART_INTERFACE, USART_IT_RXNE);

    data = (uint8_t) USART_ReceiveData(ESP_UART_INTERFACE);

    // replace CR or LF characters by Space
    if( esp.wifi_mode != WIFI_INCOMING_DATA &&
        ((data == AT_LF_CODE) || (data == AT_CR_CODE)))
    {
      data = ' ';
    }

    esp_receive_data(data);
  }

  if(USART_GetITStatus(ESP_UART_INTERFACE, USART_IT_IDLE) == SET)
  {
    esp.receive_finished = 1;
    USART_ITConfig(ESP_UART_INTERFACE, USART_IT_RXNE, DISABLE);
    (void) USART_ReceiveData(ESP_UART_INTERFACE);
  }
}
