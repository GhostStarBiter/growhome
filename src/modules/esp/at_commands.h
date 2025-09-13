#ifndef ESP_AT_COMMANDS_H
#define ESP_AT_COMMANDS_H

#define AT_COM                              "AT"
#define AT_OK                               "OK"
#define AT_ERROR                            "ERROR"
#define AT_ECHO_OFF                         "E0"

#define AT_RESET                            "+RST"

#define AT_REQEST_ENDING                    "\r\n"                              // CR+LF
#define AT_LF_CODE                          0x0A
#define AT_CR_CODE                          0x0D
#define ESP_UART_WIFI_PASSTHROUGH_MODE      "1"                                 // CIPMODE=1
#define ESP_COMMAND_SEND_DATA               "+CIPSEND"                           //From now on, data received from UART will be transmitted to the server automatically.

#define ESP_SET_WIFI_MODE_SOFT_AP           "+CWMODE_CUR=3"
#define ESP_JOIN_AP                         "+CWJAP="
#define ESP_ENABLE_WPS_MODE                 "+WPS=<enable>"
#define ESP_LIST_AVAILABLE_AP               "+CWLAP"



#endif // ESP_AT_COMMANDS_H
