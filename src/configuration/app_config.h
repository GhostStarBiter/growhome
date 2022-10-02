//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  GPIO pins assignment
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#ifndef APP_CONFIG_H
#define APP_CONFIG_H

#define GRWHS   "GrowHouse"             // @TODO Rename all APPLICATION or APP name to GRWHS

#include "stm32f10x.h"
#include "system_config/stm32f10x_conf.h"

//  If set to 1
//  don't remap SWD pins for LCD216 usage
#define DEBUG_MODE                            0
#define APPLICATION_MODE                      (!DEBUG_MODE)


//  APPLICATION
#define APPLICATION_NAME                      "GrowHouse"
#define APPLICATION_NAME_POS_X                4
#define APPLICATION_NAME_POS_Y                0
#define APPLICATION_VERSION_POS_X             5
#define APPLICATION_VERSION_POS_Y             1

#define GRWHS_USE_NETWORK               0
#if GRWHS_USE_NETWORK
  #define GRWHS_VERSION                 "v00.02"
  #define SSID_LEN_MAX                        16
  #define PSWD_LEN_MAX                        16
#else
  #define GRWHS_VERSION                 "v00.01"
#endif

#define GRWHS_USE_ONEWIRE_SENSOR              0
#define GRWHS_USE_TWO_LM60_TEMP_SENS          0

// Analog temeprature sensor
#define GRWHS_USE_LM60_TEMP                   1
// OneWire temperature and humidiry sensor
#define GRWHS_USE_AM2301_TEMP                 (!(GRWHS_USE_LM60_TEMP))
// GY21 - I2C bus temperature and humidity sensor
#define GRWHS_USE_GY21_SENSOR                 1

// GROW HOUSE
#define GRWHS_MANUAL_MODE_TIMEOUT             20*1000 // [ms] 20 seconds (with respect to GROWBOX TASK CYCLE)
#define AIR_HEATER_CYCLE_TIME                 5000    // [ms]

#define GRWHS_DEFAULT_TEMP                    26
#define AIR_REGULATION_TOLERANCE_DEGREES      0.4
#define AIR_TEMP_PI_CTRL_KP                   10
#define AIR_TEMP_PI_CTRL_KI                   0.001

#define GROWBOX_AIR_MIX_CYCLE_MINS            15
#define GROWBOX_AIR_MIX_ACTIVE_MINS           2
#define GROWBOX_AIR_MIX_CYCLE_TIMEOUT         GROWBOX_AIR_MIX_CYCLE_MINS*60*1000 // [ms]
#define GROWBOX_AIR_MIX_ACTIVE_TIMEOUT        GROWBOX_AIR_MIX_ACTIVE_MINS*60*1000 // [ms]

#define GROWBOX_MAX_LIGHT_ON_HOURS            14

#define GROWBOX_STATISTICS_UPDATE_TIMEOUT     1000      // [ms]

// WATER
#define WATER_T_ON_SEC_DEFAULT                15  // [sec]
#define WATER_T_ON_SEC_MAX                    59  // [sec]
#define WATER_INTERVAL_MINS_DEFAULT           3   // [mins]
#define WATER_INTERVAL_MINS_MAX               90  // [mins]


#endif // APP_CONFIG_H

//  *** END OF FILE ***
