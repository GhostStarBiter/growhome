#ifndef NETWORK_COMM_PRIV_H
#define NETWORK_COMM_PRIV_H

#include <string.h>
#include "esp/esp.h"

#include "network.h"

typedef struct
{
  bool status;

} network_obj_t;


//--------------------------------------------------------------------------------------------------
/// @brief Initiate server connection procedure
//--------------------------------------------------------------------------------------------------
result_t network_server_connect(char* ip_addr, char* port);



#endif  // NETWORK_COMM_PRIV_H
