#ifndef NETWORK_COMM_H
#define NETWORK_COMM_H


//--------------------------------------------------------------------------------------------------
/// @brief  Init network communication module
//--------------------------------------------------------------------------------------------------
void network_init(void);


//--------------------------------------------------------------------------------------------------
/// @brief  Incoming network's packet parser
//--------------------------------------------------------------------------------------------------
void network_parser(uint8_t* buf_ptr);


//--------------------------------------------------------------------------------------------------
/// @brief  RTOS task funciton
//--------------------------------------------------------------------------------------------------
void network_communication_task(void* pvParameters);


//--------------------------------------------------------------------------------------------------
/// @brief  Getter of WiFi Access Point SSID name
/// @return String of characters with name of Access Point connected to
///         (NULL if not connected)
//--------------------------------------------------------------------------------------------------
char* network_get_ap_ssid(void);


//--------------------------------------------------------------------------------------------------
/// @brief  Getter of online status
/// @return true - online/connected
///         false - ofline/disconnected
//--------------------------------------------------------------------------------------------------
bool network_get_online_status(void);

#endif  // NETWORK_COMM_H
