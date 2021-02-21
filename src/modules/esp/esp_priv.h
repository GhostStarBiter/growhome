#ifndef ESP8266_MODULE_PRIV_H
#define ESP8266_MODULE_PRIV_H

#include "stm32f10x.h"
#include "task.h"

#include "main/application_map.h"

#include "stm32f10x_usart.h"

#include "gpio/mcu_gpio.h"
#include "uart/mcu_uart.h"
#include "utils/utils.h"

#include "string.h"
#include "esp.h"

#define ESP_SEND_BUF_SIZE                   512
#define ESP_MAX_COMMAND_LEN                 (ESP_SEND_BUF_SIZE - AT_COMMAND_START_INDEX - sizeof(AT_REQEST_ENDING))

#define ESP_RECV_BUF_SIZE                   512

#define ESP_COMPARE_RESPONSE_BUF_SIZE       64

#define AT_COMMAND_START_INDEX              2

#define AT_STARTUP_TEST                     ""
#define AT_NO_RESPONSE                      ""
#define AT_RESPONSE_OK                      "OK"
#define AT_RESPONSE_ERROR                   "ERROR"

#define AT_RESET                            "+RESET"

#define AT_REQEST_ENDING                    "\r\n"
#define ESP_COMMAND_SEND_DATA               "+CIPSEND="

#define AT_LF_CODE                          0x0A
#define AT_CR_CODE                          0x0D

#define ESP_RECV_BUFFER_EMPTY               (!esp.uw_recv_size)
#define ESP_RECV_BUFFER_NOT_FULL            (esp.uw_recv_size < ESP_RECV_BUF_SIZE)

typedef struct{
    USART_TypeDef *           pUART;
    FunctionalState           chip_status;
    esp_sw_status             sw_status;
    wifi_mode_t               wifi_mode;
    uint8_t                   receive_finished;
    uint8_t                   send_buf[ESP_RECV_BUF_SIZE];
    uint16_t                  uw_send_size;
    esp_response_type_t       response_type;
    uint8_t                   recv_buf[ESP_SEND_BUF_SIZE];
    uint16_t                  uw_recv_size;
    void                      (*process_incoming_data)(uint8_t*);
} esp_struct_t;





//--------------------------------------------------------------------------------------------------
static void esp_next_event(wifi_mode_t e_status);


//--------------------------------------------------------------------------------------------------
static void esp_set_status(esp_sw_status e_new_sw_status);


//--------------------------------------------------------------------------------------------------
static result_t esp_process_incoming_data(void);


#endif // ESP8266_MODULE_PRIV_H
