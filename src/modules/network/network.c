#include "network_priv.h"


//--------------------------------------------------------------------------------------------------
void network_init(void)
{

}


//--------------------------------------------------------------------------------------------------
void network_parser(uint8_t* buf_ptr)
{

}


//--------------------------------------------------------------------------------------------------
void network_communication_task(void* pvParameters)
{

  network_init();

  esp_init(network_parser);

  for(;;)
  {
    esp_update();


  }
}
