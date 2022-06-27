#ifndef SYSTEM_USER_INTERFACE_H
#define SYSTEM_USER_INTERFACE_H

#include "main/types.h"

//------------------------------------------------------------------------------
/// @brief RTOS task to update system data for user
//------------------------------------------------------------------------------
void system_user_interface_task(void *pvParameters);


//--------------------------------------------------------------------------------------------------
/// @brief  Get Name of Access Point entered by the User
/// @return String of characters with AP Name
//--------------------------------------------------------------------------------------------------
char const * sui_get_user_ssid(void);


//--------------------------------------------------------------------------------------------------
/// @brief  Get Password to Access Point entered by the User
/// @return String of characters with AP Password
//--------------------------------------------------------------------------------------------------
char const * sui_get_user_pswd(void);


//--------------------------------------------------------------------------------------------------
/// @brief  Function returns boolean flag of current User Credentials status
/// @return true - WiFi User Credentials entered;
///         false - WiFi User Credentials are not entered
//--------------------------------------------------------------------------------------------------
bool sui_user_enter_credentials(void);

#endif // SYSTEM_USER_INTERFACE_H
