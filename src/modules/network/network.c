#include "network_priv.h"

#define NETWORK_DEFAULT_SERVER_IP     "124.0.0.100"

typedef struct {
  bool online;

  char server_ip[15];   // range from 0.0.0.0 to 255.255.255.255
  char port[5];         // 0 .. 65535
} network_t;

network_t network;

//--------------------------------------------------------------------------------------------------
void network_init(void)
{
  network.online = false;

  esp_init(ESP_UART_INTERFACE, network_parser);
}


//--------------------------------------------------------------------------------------------------
void network_parser(uint8_t* buf_ptr)
{

}


//--------------------------------------------------------------------------------------------------
void network_communication_task(void* pvParameters)
{
  network_init();

  for(;;)
  {
    esp_update();

  }
}



//--------------------------------------------------------------------------------------------------
result_t network_connect_AP(char* ssid, char* password)
{
  result_t connection_status = result_fail;

  return connection_status;
}


//--------------------------------------------------------------------------------------------------
result_t network_server_connect(char* ip_addr, char* port)
{
  result_t connection_status = result_fail;

  return connection_status;
}


//--------------------------------------------------------------------------------------------------
bool network_get_online_status(void)
{
  return network.online;
}
