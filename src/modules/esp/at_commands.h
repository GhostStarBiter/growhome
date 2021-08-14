#ifndef ESP_AT_COMMANDS_H
#define ESP_AT_COMMANDS_H

#define AT_COMMAND_PREFIX                   "AT+"
#define AT_RESPONSE_OK                      "OK"
#define AT_RESPONSE_ERROR                   "ERROR"

#define AT_RESET                            "RST"
//#define AT+CIPSTART                         "TCP","192.168.101.110\", 8080"    // protocol, server ip and port

#define AT_REQEST_ENDING                    "\r\n"                              // CR+LF
#define AT_LF_CODE                          0x0A
#define AT_CR_CODE                          0x0D
#define ESP_UART_WIFI_PASSTHROUGH_MODE      "1"                                 // CIPMODE=1
#define ESP_COMMAND_SEND_DATA               "CIPSEND"                           //From now on, data received from UART will be transmitted to the server automatically.


#endif // ESP_AT_COMMANDS_H
