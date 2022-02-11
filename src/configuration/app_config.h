//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  GPIO pins assignment
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#ifndef APP_CONFIG_H
#define APP_CONFIG_H

#include "stm32f10x.h"
#include "system_config/stm32f10x_conf.h"

//  If set to 1
//  don't remap SWD pins for LCD216 usage
#define DEBUG_MODE                          1


//  APPLICATION
#define APPLICATION_NAME                    "GROWHOME"
#define APPLICATION_NAME_POS_X              4
#define APPLICATION_NAME_POS_Y              0

#define APPLICATION_USE_NETWORK             0

#if APPLICATION_USE_NETWORK
  #define APPLICATION_VERSION                 "v00.02"
  #define WIFI_NAME_LEN_MAX                   16
  #define WIFI_PSWD_LEN_MAX                   16
#else
  #define APPLICATION_VERSION                 "v00.01"
#endif

#define APPLICATION_VERSION_POS_X           5
#define APPLICATION_VERSION_POS_Y           1



#define WATER_T_ON_SEC_DEFAULT              15  // [sec]
#define WATER_T_ON_SEC_MAX                  59  // [sec]
#define WATER_INTERVAL_MINS_DEFAULT         3   // [mins]
#define WATER_INTERVAL_MINS_MAX             90  // [mins]

#define APPLICATION_USE_ONEWIRE_SENSOR      0

#define APPLICATION_USE_TWO_LM60_TEMP_SENS  0

#define GROWBOX_MANUAL_MODE_TIMEOUT         20*1000 // [ms] 20 seconds (with respect to GROWBOX TASK CYCLE)
#define AIR_HEATER_CYCLE_TIME               5000    // [ms]

#define GROWBOX_DEFAULT_TEMPERATURE         26
#define AIR_REGULATION_TOLERANCE_DEGREES    0.5
#define AIR_TEMP_PI_CTRL_KP                 10
#define AIR_TEMP_PI_CTRL_KI                 0.001

#define GROWBOX_AIR_MIX_CYCLE_TIMEOUT       5*60*1000 // [ms]
#define GROWBOX_AIR_MIX_ACTIVE_TIMEOUT      2*60*1000 // [ms]

#define GROWBOX_MAX_LIGHT_ON_HOURS          14

#define GROWBOX_STATISTICS_UPDATE_TIMEOUT   1000      // [ms]



#define APPLICATION_USE_LM60_TEMP           1
#define APPLICATION_USE_AM2301_TEMP         (!(APPLICATION_USE_LM60_TEMP))



#endif // APP_CONFIG_H

//  *** END OF FILE ***
