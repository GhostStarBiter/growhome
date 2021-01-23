#ifndef SYSTEM_USER_INTERFACE_PRIV_H
#define SYSTEM_USER_INTERFACE_PRIV_H

#include <stdlib.h>
#include <string.h>

#include "FreeRTOS.h"
#include "task.h"

#include "main/types.h"
#include "main/application_map.h"

#include "lcd216/lcd216.h"
#include "encoder_input/encoder_input.h"

#include "utils/utils.h"

#include "schedule/schedule.h"
#include "green_house_system/green_house.h"

#include "sui.h"

#include "gpio/mcu_gpio.h"      // debug

#define MAIN_SCREEN_ELEMENTS_NUMBER         8
#define TEMPERATURE_SCREEN_ELEMENTS_NUMBER  5
#define WATER_SCREEN_ELEMENTS_NUMBER        5
#define LIGHT_SCREEN_ELEMENTS_NUMBER        5
#define CONNECTION_SCREEN_ELEMENTS_NUMBER   5
#define SET_TIME_SCREEN_ELEMENTS_NUMBER     5


#define SUI_DISPLAY_BUFFER_SIZE             (LCD216_ROWS*LCD216_COLUMNS)

typedef enum {
  CURRENT_TIME_HOURS          = 1,
  CURRENT_TIME_MINS           = 3,
  LIGHT_STATUS                = 5,
  LIGHT_T_ON_TIME             = 7,
  LIGHT_T_OFF_TIME            = 9,
  CURRENT_AIR_TEMP            = 11,
  AIR_HEATER                  = 13,
  AIR_OUTLET                  = 15,
  WATERING_STATUS             = 17,
  WATER_LEVEL                 = 19,
  WATER_T_ON_TIME             = 21,
  WATER_T_OFF_TIME            = 23,
  ONLINE_LINK_STATUS          = 25,
  SELECT_WIFI_AP              = 27
} ctrl_item_id_t;

typedef enum {
  DISPLAY_ITEM_NUMERIC,
  DISPLAY_ITEM_TEXTUAL
} item_type_t;


typedef struct {
    uint8_t x;
    uint8_t y;
} on_screen_position_t;


typedef struct {
    ctrl_item_id_t        id;
    item_type_t           type;
    char*                 p_text;                           // char or text representative of display item
    on_screen_position_t  text_position;                    // coordinates of item (char or string) on display
    uint16_t              data;                             // item data (optional) to display/set
    uint16_t              (*update_data)(ctrl_item_id_t);   // function pointer to update screen item data;
    void                  (*action)(int16_t);                // execute function when item activated by pressing encoder button
} screen_item_t;


typedef struct screen{
    struct screen*  prev;
    struct screen*  next;
    screen_item_t*  item;
    uint8_t         items_cnt;
} display_screen_t;


typedef struct {
    rtc_time_t  set_time;             //@todo maybe (char) and convert to numbers?
    uint8_t     ip_address[4];        // [0..255] [0..225] [0..225] [0..225]
} user_settings_t;


typedef struct {
    // *** INTERNAL VARIABLES
    char              display_buffer[SUI_DISPLAY_BUFFER_SIZE];
    uint8_t           active_item_index;
    int8_t            menu_iterator;

    // *** OBJECTS
    display_screen_t* active_display;
    user_settings_t   settings;
} system_user_interface_t;



//--------------------------------------------------------------------------------------------------
/// @brief  Initializes configured interface for user interaction with system
///           - LCD 216
///           - TFT (to be implemented)
//--------------------------------------------------------------------------------------------------
void system_user_interface_init(void);


//--------------------------------------------------------------------------------------------------
/// @brief  Function initializes user menu screens
//--------------------------------------------------------------------------------------------------
static void sui_init_user_menu(void);


//--------------------------------------------------------------------------------------------------
/// @brief
//--------------------------------------------------------------------------------------------------
static void system_user_update_display(void);


//--------------------------------------------------------------------------------------------------
/// @brief
//--------------------------------------------------------------------------------------------------
static void system_user_interface_update(void);


//--------------------------------------------------------------------------------------------------
/// @brief
//--------------------------------------------------------------------------------------------------
static void system_user_interface_set_active_display
(
    display_screen_t* p_set_screen
);


//--------------------------------------------------------------------------------------------------
/// @brief  Initialize display menu main screen
//--------------------------------------------------------------------------------------------------
static void sui_main_screen_init(void);


//--------------------------------------------------------------------------------------------------
/// @brief  Initialize display menu temperature screen
//--------------------------------------------------------------------------------------------------
static void sui_temperature_screen_init(void);


//--------------------------------------------------------------------------------------------------
/// @brief  Initialize display menu water screen
//--------------------------------------------------------------------------------------------------
static void sui_water_screen_init(void);


//--------------------------------------------------------------------------------------------------
/// @brief  Initialize display menu light screen
//--------------------------------------------------------------------------------------------------
static void sui_light_screen_init(void);


//--------------------------------------------------------------------------------------------------
/// @brief  Initialize display menu connection screen
//--------------------------------------------------------------------------------------------------
static void sui_connection_screen_init(void);


//--------------------------------------------------------------------------------------------------
/// @brief  Initialize display menu set time screen
//--------------------------------------------------------------------------------------------------
static void sui_set_time_screen_init(void);


//--------------------------------------------------------------------------------------------------
/// @brief  Set active previous dispaly screen
//--------------------------------------------------------------------------------------------------
static void sui_go_to_prev_screen
(
    int16_t dummy
);


//--------------------------------------------------------------------------------------------------
/// @brief  Set active previous dispaly screen
//--------------------------------------------------------------------------------------------------
static void sui_go_to_next_screen
(
    int16_t dummy
);


//--------------------------------------------------------------------------------------------------
/// @brief  Generic action-function for menu items
/// @param  Relative encoder ticks
//--------------------------------------------------------------------------------------------------
void sui_item_action
(
    int16_t encoder_ticks
);


#endif // SYSTEM_USER_INTERFACE_PRIV_H

//  *** END OF FILE ***
