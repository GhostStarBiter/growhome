#include <stdlib.h>
#include <string.h>

#include "FreeRTOS.h"
#include "task.h"

#include "main/types.h"
#include "configuration/app_config.h"
#include "configuration/task_config.h"
#include "configuration/water_pump_config.h"

#include "lcd216/lcd216.h"
#include "encoder_input/encoder_input.h"

#include "utils/utils.h"

#include "schedule/schedule.h"
#include "green_house_system/green_house.h"
#include "water/water.h"
#include "network/network.h"

#include "sui.h"

#include "gpio/mcu_gpio.h"      // debug



#define MAIN_SCREEN_ELEMENTS_NUMBER         8
#define TEMPERATURE_SCREEN_ELEMENTS_NUMBER  5
#define WATER_SCREEN_ELEMENTS_NUMBER        5
#define LIGHT_SCREEN_ELEMENTS_NUMBER        7
#if APPLICATION_USE_NETWORK
#define NETWORK_SCREEN_ELEMENTS_NUMBER      4
#define AP_NAME_SCREEN_ELEMENTS_NUMBER      19
#define AP_PSWD_SCREEN_ELEMENTS_NUMBER      19

#define ASCII_CHARACTER_QUESTION_MARK       63  // ?
#define SPACE_SYMBOL                        ' '  // Space
#define TILDA_SYMBOL                        '~' //
#endif

#define SUI_DISPLAY_BUFFER_SIZE             (LCD216_ROWS*LCD216_COLUMNS)

typedef enum {
  CURRENT_TIME_HOURS          = 1,
  CURRENT_TIME_MINS           = 3,
  LIGHT_STATUS                = 5,
  LIGHT_SCREEN_TEXT           = 6,
  TIME_HOUR_SET_LIGHT_ON      = 7,
  TIME_MINUTE_SET_LIGHT_ON    = 8,
  LIGHT_DURATION_TEXT         = 9,
  LIGHT_DURATION_HOURS        = 10,
  CURRENT_AIR_TEMP            = 11,
  AIR_HEATER                  = 13,
  AIR_OUTLET                  = 15,
  WATER_POWER                 = 17,
  WATER_LEVEL                 = 19,
  WATER_T_ON_TIME             = 21,
  WATER_CYCLE                 = 23,
#if APPLICATION_USE_NETWORK
  ONLINE_LINK_STATUS          = 25,
  CONNECT_WIFI_AP             = 27,
  // Wi-Fi Access Point name symbols are to be entered separately
  AP_NAME_TEXT                = 29,
  AP_NAME_SYMBOL_0            = 30,
  AP_NAME_SYMBOL_1            = 31,
  AP_NAME_SYMBOL_2            = 32,
  AP_NAME_SYMBOL_3            = 33,
  AP_NAME_SYMBOL_4            = 34,
  AP_NAME_SYMBOL_5            = 35,
  AP_NAME_SYMBOL_6            = 36,
  AP_NAME_SYMBOL_7            = 37,
  AP_NAME_SYMBOL_8            = 38,
  AP_NAME_SYMBOL_9            = 39,
  AP_NAME_SYMBOL_10           = 40,
  AP_NAME_SYMBOL_11           = 41,
  AP_NAME_SYMBOL_12           = 42,
  AP_NAME_SYMBOL_13           = 43,
  AP_NAME_SYMBOL_14           = 44,
  AP_NAME_SYMBOL_15           = 45,

  AP_NAME_ESCAPE               = 47,
  AP_NAME_ENTERED             = 48,

  PSWD_TEXT                   = 49,
  PSWD_SYMBOL_0               = 50,
  PSWD_SYMBOL_1               = 51,
  PSWD_SYMBOL_2               = 52,
  PSWD_SYMBOL_3               = 53,
  PSWD_SYMBOL_4               = 54,
  PSWD_SYMBOL_5               = 55,
  PSWD_SYMBOL_6               = 56,
  PSWD_SYMBOL_7               = 57,
  PSWD_SYMBOL_8               = 58,
  PSWD_SYMBOL_9               = 59,
  PSWD_SYMBOL_10              = 60,
  PSWD_SYMBOL_11              = 61,
  PSWD_SYMBOL_12              = 62,
  PSWD_SYMBOL_13              = 63,
  PSWD_SYMBOL_14              = 64,
  PSWD_SYMBOL_15              = 65,

  PSWD_BREAK                  = 67,
  PSWD_ENTERED                = 69,
#else
  WORD_GROW                   = 25,
#endif



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
    bool                  activated;                        // true/false - is item value is to be set
    char*                 p_text;                           // char or text representative of display item
    on_screen_position_t  text_position;                    // coordinates of item (char or string) on display
    uint16_t              data;                             // item data (optional) to display/set
    uint16_t              (*update_data)(ctrl_item_id_t);   // function pointer to update screen item data;
    void                  (*action)(int16_t);                // execute function when item activated by pressing encoder button
} screen_item_t;


typedef struct screen{
    struct screen*  prev;
    struct screen*  next;
    screen_item_t** item;
    uint8_t         items_cnt;
} display_screen_t;


// structure controlled by encoder input which is interrupt-processed
typedef struct {
    // *** INTERNAL VARIABLES
    volatile char     display_buffer[SUI_DISPLAY_BUFFER_SIZE];
    uint8_t           active_item_index;
    display_screen_t* active_display;
#if APPLICATION_USE_NETWORK
    char              wifi_name[WIFI_NAME_LEN_MAX];
    char              wifi_pswd[WIFI_PSWD_LEN_MAX];
#endif
    bool              wifi_enter_done;
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


#if APPLICATION_USE_NETWORK
//--------------------------------------------------------------------------------------------------
/// @brief  Initialize display menu connection screen
//--------------------------------------------------------------------------------------------------
static void sui_network_screen_init(void);


//--------------------------------------------------------------------------------------------------
/// @brief  Initialize Wi-Fi-connection procedure
//--------------------------------------------------------------------------------------------------
static void sui_ap_name_screen_init(void);


//--------------------------------------------------------------------------------------------------
/// @brief  Initialize Wi-Fi-connection procedure
/// @param  Pointer to Access Point name buffer
//--------------------------------------------------------------------------------------------------
static void sui_clear_user_credentials
(
    char* credential
);
//--------------------------------------------------------------------------------------------------
///
//--------------------------------------------------------------------------------------------------
static void sui_ap_pswd_screen_init(void);


//--------------------------------------------------------------------------------------------------
/// @brief  Function to check that character if in range [! ; ~]
//--------------------------------------------------------------------------------------------------
static void sui_check_character_range
(
    char* character
);

#endif


//--------------------------------------------------------------------------------------------------
/// @brief  Set active previous display screen
//--------------------------------------------------------------------------------------------------
static void sui_go_to_prev_screen
(
    int16_t dummy
);


//--------------------------------------------------------------------------------------------------
/// @brief  Set active previous display screen
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




#define LIGHT_ON_TEXT                       "LT:ON"
#define LIGHT_OFF_TEXT                      "LT:OFF"

// *****************************************
//  *** User interface structure
volatile system_user_interface_t sui;

screen_item_t go_to_prev_screen = {
    .p_text                   = "<",
    .type                     = DISPLAY_ITEM_TEXTUAL,
    .text_position.x          = 14,
    .text_position.y          = LCD216_FIRST_ROW,
    .action                   = sui_go_to_prev_screen,
    .update_data              = NULL
};

screen_item_t go_to_next_screen = {
    .p_text                   = ">",
    .type                     = DISPLAY_ITEM_TEXTUAL,
    .text_position.x          = 15,
    .text_position.y          = LCD216_FIRST_ROW,
    .action                   = sui_go_to_next_screen,
    .update_data              = NULL
};

//------------------------------------------------------------------------------
//  *** MAIN SCREEN
screen_item_t main_screen_temperature,
              main_screen_light,
              water_level,
              time_hours,
              time_mins,
#if APPLICATION_USE_NETWORK
              online_status;
#else
              word_grow;
#endif

screen_item_t* main_screen_items[MAIN_SCREEN_ELEMENTS_NUMBER];

//------------------------------------------------------------------------------
//  *** TEMPERATURE SCREEN
screen_item_t temperature,
              heater_status,
              air_outlet_status;

screen_item_t* temperature_screen_items[TEMPERATURE_SCREEN_ELEMENTS_NUMBER];

//------------------------------------------------------------------------------
//  *** WATER SCREEN
screen_item_t water_pump_power,
              water_t_on,
              water_t_cycle;

screen_item_t* water_screen_items[WATER_SCREEN_ELEMENTS_NUMBER];

//------------------------------------------------------------------------------
//  *** LIGHT SCREEN
screen_item_t light_text,
              light_t_on_hour,
              light_t_on_min,
              light_duration_text,
              light_duration_hours;

screen_item_t* light_screen_items[LIGHT_SCREEN_ELEMENTS_NUMBER];


//**************************************************************************************************
//**************************************************************************************************
#if APPLICATION_USE_NETWORK
//------------------------------------------------------------------------------
//  *** NETWORK SCREEN
screen_item_t online_connection_status,
              select_wi_fi_ap;

screen_item_t* network_screen_items[NETWORK_SCREEN_ELEMENTS_NUMBER];


//------------------------------------------------------------------------------
//  *** ENTER AP NAME SCREEN
screen_item_t ap_name_text,
              ap_name[WIFI_NAME_LEN_MAX],
              ap_name_done,
              ap_name_escape;

screen_item_t* ap_name_screen_items[AP_NAME_SCREEN_ELEMENTS_NUMBER];


//------------------------------------------------------------------------------
//  *** ENTER AP PASSWORD SCREEN

screen_item_t ap_pswd_text,
              ap_pswd[WIFI_PSWD_LEN_MAX],
              ap_pswd_done,
              ap_pswd_cancel;

screen_item_t* ap_pswd_screen_items[AP_PSWD_SCREEN_ELEMENTS_NUMBER];
#endif  // APPLICATION_USE_NETWORK


// *** User menu displays
display_screen_t  main_display,
#if APPLICATION_USE_NETWORK
                  network_display,
                  ap_name_display,
                  ap_pswd_display,
#endif  // APPLICATION_USE_NETWORK
                  temperature_display,
                  water_display,
                  light_display;


//--------------------------------------------------------------------------------------------------
void system_user_interface_init(void)
{
  // ***
  lcd216_init();

  // ***
  encoder_init();

  // ***
  sui_init_user_menu();

  sui.active_item_index = 0;
  sui.wifi_enter_done = false;
#if APPLICATION_USE_NETWORK
  memset(sui.wifi_name, '?', WIFI_NAME_LEN_MAX);
  memset(sui.wifi_pswd, '?', WIFI_PSWD_LEN_MAX);
#endif

  system_user_interface_set_active_display(&main_display);

}


//--------------------------------------------------------------------------------------------------
static void system_user_interface_set_active_display(display_screen_t* p_set_screen)
{
  uint8_t buf_position = 0;
  uint8_t x_pos, y_pos;
  uint8_t i = 0;

  sui.active_display = p_set_screen;

  // clean display buffer
  for(i = 0; i < SUI_DISPLAY_BUFFER_SIZE; i++)
  {
    sui.display_buffer[i] = ' ';
  }

  // prepare new display buffer
  for(i = 0; i < sui.active_display->items_cnt; i++)
  {
    x_pos = p_set_screen->item[i]->text_position.x;
    y_pos = p_set_screen->item[i]->text_position.y;

    // to define display item's position in display buffer
    // by multiplying x_pos by y_pos
    // the y_pos(ROW) must be non-zero(but numeration in display is from 0)

    if(y_pos < 1)
    {
      y_pos = 1;
    }
    else
    {
      x_pos += 16;
    }

    buf_position = x_pos * y_pos;
    strncpy((char*) &sui.display_buffer[buf_position], p_set_screen->item[i]->p_text, strlen(p_set_screen->item[i]->p_text));
  }

  // clean-up all null-terminator characters
  for(i = 0; i < SUI_DISPLAY_BUFFER_SIZE; i++)
  {
    if(sui.display_buffer[i] == '\0')
    {
      sui.display_buffer[i] = ' ';
    }
  }
}


//--------------------------------------------------------------------------------------------------
static void system_user_interface_startup_screen(void)
{
  lcd216_puts(APPLICATION_NAME_POS_X, APPLICATION_NAME_POS_Y, APPLICATION_NAME);
  lcd216_puts(APPLICATION_VERSION_POS_X, APPLICATION_VERSION_POS_Y, APPLICATION_VERSION);

  vTaskDelay(2000);

  lcd216_clear();
}


//--------------------------------------------------------------------------------------------------
void system_user_interface_task(void *pvParameters)
{
  const TickType_t x_interface_task_cycle_ms = INTERFACE_TASK_CYCLE;
  TickType_t        x_last_wake_time;

  system_user_interface_init();

  system_user_interface_startup_screen();

  lcd216_cursor_on();
  lcd216_blink_on();
  lcd216_cursor_set(0,0);

  x_last_wake_time            = xTaskGetTickCount();

  for(;;)
  {
    // ***
    vTaskDelayUntil(&x_last_wake_time, x_interface_task_cycle_ms);

    // ***
    system_user_interface_update();
  }
}


//--------------------------------------------------------------------------------------------------
static void system_user_interface_update(void)
{
  uint8_t         index;
  static int8_t   prev_ticks    = 0;
  int8_t          current_ticks = 0;
  int8_t          delta_ticks = 0;

  index = sui.active_item_index;

  // *** Check Encoder rotation
  current_ticks = encoder_get_ticks();

  if(current_ticks != 0)
  {
    if(prev_ticks != current_ticks)
    {
      delta_ticks = prev_ticks - current_ticks;
    }

    if(!sui.active_display->item[index]->activated)
    {

      if(delta_ticks > 0)
      {
        sui.active_item_index += delta_ticks;

        if(sui.active_item_index >= sui.active_display->items_cnt)
        {
          sui.active_item_index = 0;//(sui.active_display->items_cnt);
        }
      }
      else if(delta_ticks < 0)
      {
        if(sui.active_item_index >= 1)
        {
          sui.active_item_index -= 1;
        }
        else
        {
          sui.active_item_index = sui.active_display->items_cnt - 1;
        }
      }
      else
      {
        // nothing
      }

      // Update index of currently active item
      index = sui.active_item_index;
      lcd216_cursor_set(  sui.active_display->item[index]->text_position.x,
                          sui.active_display->item[index]->text_position.y);
      vTaskDelay(250);
    }
  }

  // *** Check button status
  if(encoder_button_activated()
      && (sui.active_display->item[index]->action != NULL))
  {
    // Callback
    sui.active_display->item[index]->action(delta_ticks);
  }
  else
  {
    sui.active_display->item[index]->activated = false;
  }

  // if action was to change screen (and we don't know it forehand) than index must be updated!
  // because in screen change action pointer to active display is changed
  // and not updating the index causes Hard Fault!
  index = sui.active_item_index;
  if(sui.active_display->item[index]->activated)
  {
    growbox_set_control_mode(CONTROL_MODE_MANUAL);
  }
  else
  {
    growbox_set_control_mode(CONTROL_MODE_AUTOMATIC);
  }


  prev_ticks = current_ticks;

  system_user_update_display();
}


//--------------------------------------------------------------------------------------------------
static void system_user_update_display(void)
{
  ctrl_item_id_t    item_id;
  volatile display_screen_t* display;
  uint8_t           item_text_len = 0;
  char*             item_data_string;
  uint8_t           buff_pos;
  uint8_t           m, n;

  // ***
  display = sui.active_display;

  // clean display buffer
  for(uint8_t i = 0; i < SUI_DISPLAY_BUFFER_SIZE; i++)
  {
    sui.display_buffer[i] = ' ';
  }


  for(uint8_t index=0; index < display->items_cnt; index++)
  {
    // display buffer insert position
    buff_pos = 0;

    item_id = display->item[index]->id;

    // call update item's data function
    if( display->item[index]->update_data != NULL
        && !display->item[index]->activated)
    {
      display->item[index]->data = display->item[index]->update_data(item_id);
    }

    // Calculate buffer insert position
    buff_pos = display->item[index]->text_position.x;
    // correct buffer insert position according to item's position row
    (display->item[index]->text_position.y > 0) ? (buff_pos += 16) : (buff_pos);

    n = 0;
    m = strlen(display->item[index]->p_text);
    while(n < m)
    {
      sui.display_buffer[buff_pos + n] = display->item[index]->p_text[n];
      n++;
    }

    // ***
    // CONVERT NUMBERS TO CHARACTERS
    if(display->item[index]->type == DISPLAY_ITEM_NUMERIC)
    {
      if(display->item[index]->p_text != NULL)
      {
        // length of textual specifier of item (TMP, WTR etc.)
        item_text_len = strlen(display->item[index]->p_text);
      }
      else
      {
        item_text_len = 0;
      }

      // string with item data(numeric) converted to text
      item_data_string = convert_num_to_str(display->item[index]->data);

      buff_pos = 0;
      buff_pos = display->item[index]->text_position.x + item_text_len;
      // correct buffer insert position according to item's position row
      (display->item[index]->text_position.y > 0) ? (buff_pos += 16) : (buff_pos);

      // zeroing place for new data
      sui.display_buffer[buff_pos] = '0';
      sui.display_buffer[buff_pos+1] = '0';

      // shift character 1 position right if it is less than 0
      if(display->item[index]->data < 10)
      {
        buff_pos += 1;
      }

      m = 0;
      while(item_data_string[m])
      {
        sui.display_buffer[buff_pos + m] = item_data_string[m];
        m++;
      }
    }
    else
    {
      // Display item type is TEXTUAL
      // The text of such item will be changed
      // in item's action function and
      // copied to display buffer in the code above
    }
  }

  // clean-up all null-terminator characters
  for(uint8_t l = 0; l < SUI_DISPLAY_BUFFER_SIZE; l++)
  {
    if(sui.display_buffer[l] == '\0')
    {
      sui.display_buffer[l] = ' ';
    }
  }

  lcd216_puts(0, 0, (char*) sui.display_buffer);

}


//--------------------------------------------------------------------------------------------------
void sui_item_action(int16_t encoder_ticks)
{
  uint8_t         index     = sui.active_item_index;
  ctrl_item_id_t  parameter = sui.active_display->item[index]->id;
  uint32_t ul_tmp = 0;
  mcu_time_t      set_time;

  switch(parameter)
  {
    case CURRENT_TIME_HOURS:
      // get instant time
      // add/substract encoder ticks
      // set value
      sui.active_display->item[index]->activated = true;
      time_hours.data += encoder_ticks;
      if(time_hours.data > 23)
      {
        time_hours.data = 0;
      }

      set_time = mcu_rtc_get_time();
      set_time.hour = time_hours.data;

      mcu_rtc_set_time(set_time);
    break;

    case CURRENT_TIME_MINS:
      sui.active_display->item[index]->activated = true;
      time_mins.data += encoder_ticks;
      if(time_mins.data > 59)
      {
        time_mins.data = 0;
      }

      set_time = mcu_rtc_get_time();
      set_time.min = time_mins.data;

      mcu_rtc_set_time(set_time);
    break;

    case LIGHT_STATUS:
      sui.active_display->item[index]->activated = true;
      if(growbox_get_light_status())
      {
        growbox_set_light(DISABLE);
        main_screen_light.p_text  = LIGHT_OFF_TEXT;
      }
      else
      {
        growbox_set_light(ENABLE);
        main_screen_light.p_text  = LIGHT_ON_TEXT;
      }
      encoder_deactivate_button();
      break;

    case TIME_HOUR_SET_LIGHT_ON:
      sui.active_display->item[index]->activated = true;
      light_t_on_hour.data += encoder_ticks;
      if(light_t_on_hour.data > 23)
      {
        light_t_on_hour.data = 0;
      }
      schedule_set_light_t_on_hour(light_t_on_hour.data);
      growbox_set_light(DISABLE);
      schedule_light_update_expected_endtime();
      break;

    case TIME_MINUTE_SET_LIGHT_ON:
      sui.active_display->item[index]->activated = true;
      light_t_on_min.data += encoder_ticks;
      if(light_t_on_min.data > 59)
      {
        light_t_on_min.data = 0;
      }
      schedule_set_light_t_on_min(light_t_on_min.data);
      growbox_set_light(DISABLE);
      schedule_light_update_expected_endtime();
      break;

    case LIGHT_DURATION_HOURS:
      sui.active_display->item[index]->activated = true;
      light_duration_hours.data += encoder_ticks;
      if(light_duration_hours.data > GROWBOX_MAX_LIGHT_ON_HOURS)
      {
        light_duration_hours.data = 0;
      }
      schedule_set_light_duration_hours(light_duration_hours.data);
      growbox_set_light(DISABLE);
      schedule_light_update_expected_endtime();
      break;

    case CURRENT_AIR_TEMP:
      sui.active_display->item[index]->activated = true;
      temperature.data += encoder_ticks;
      main_screen_temperature.data = temperature.data;

      if(temperature.data > 35)   // @TODO: max temp make a parameter
      {
        temperature.data = 35;
        main_screen_temperature.data = temperature.data;
      }
      growbox_set_temperature(temperature.data);
      break;

    case AIR_HEATER:

      break;

    case AIR_OUTLET:

      break;

    case WATER_POWER:
      sui.active_display->item[index]->activated = true;
      ul_tmp = water_get_pump_power();
      ul_tmp += encoder_ticks;
      if(ul_tmp > 99 || ul_tmp < WATER_PUMP_DEFAULT_POWER)
      {
        ul_tmp = WATER_PUMP_DEFAULT_POWER;
      }
      water_pump_power.data = ul_tmp;
      water_set_pump_power(ul_tmp);
      break;

    case WATER_T_ON_TIME:
      // set water pump active time in seconds (up to 90)
      sui.active_display->item[index]->activated = true;
      ul_tmp = schedule_get_water_duration_sec();
      ul_tmp += encoder_ticks;
      if(ul_tmp > WATER_T_ON_SEC_MAX)
      {
        ul_tmp = WATER_T_ON_SEC_MAX;
      }
      else if(ul_tmp < WATER_T_ON_SEC_DEFAULT)
      {
        ul_tmp = WATER_T_ON_SEC_DEFAULT;
      }

      water_t_on.data = ul_tmp;
      schedule_set_water_duration_sec(ul_tmp);
      break;

    case WATER_CYCLE:
      // set water cycle in minutes (up to 90)
      sui.active_display->item[index]->activated = true;
      ul_tmp = schedule_get_water_interval_mins();
      ul_tmp += encoder_ticks;
      if(ul_tmp > WATER_INTERVAL_MINS_MAX)
      {
        ul_tmp = WATER_INTERVAL_MINS_MAX;
      }
      else if(ul_tmp < WATER_INTERVAL_MINS_DEFAULT)
      {
        ul_tmp = WATER_INTERVAL_MINS_DEFAULT;
      }
      water_t_cycle.data = ul_tmp;
      schedule_set_water_interval_mins(ul_tmp);
      break;

#if APPLICATION_USE_NETWORK
    case ONLINE_LINK_STATUS:

      break;


    case CONNECT_WIFI_AP:
      lcd216_clear();
      sui.active_item_index = 0;
      system_user_interface_set_active_display(&ap_name_display);
      encoder_deactivate_button();
      sui.active_display->item[index]->activated = false;
      break;

    case AP_NAME_SYMBOL_0:
      sui.active_display->item[index]->activated = true;

      ul_tmp = encoder_ticks + ASCII_CHARACTER_QUESTION_MARK;
      sui_check_character_range((char*) &ul_tmp);
      sui.active_display->item[index]->p_text = (char*) ul_tmp;
      sui.wifi_name[0] = (char) ul_tmp;
      break;

    case AP_NAME_SYMBOL_1:
      sui.active_display->item[index]->activated = true;

      ul_tmp = encoder_ticks + ASCII_CHARACTER_QUESTION_MARK;
      sui_check_character_range((char*) &ul_tmp);
      sui.active_display->item[index]->p_text = (char*) ul_tmp;
      sui.wifi_name[1] = (char) ul_tmp;
      break;

    case AP_NAME_SYMBOL_2:
      sui.active_display->item[index]->activated = true;

      ul_tmp = encoder_ticks + ASCII_CHARACTER_QUESTION_MARK;
      sui_check_character_range((char*) &ul_tmp);
      sui.active_display->item[index]->p_text = (char*) ul_tmp;
      sui.wifi_name[2] = (char) ul_tmp;
      break;

    case AP_NAME_SYMBOL_3:
      sui.active_display->item[index]->activated = true;

      ul_tmp = encoder_ticks + ASCII_CHARACTER_QUESTION_MARK;
      sui_check_character_range((char*) &ul_tmp);
      sui.active_display->item[index]->p_text = (char*) ul_tmp;
      sui.wifi_name[3] = (char) ul_tmp;
      break;

    case AP_NAME_SYMBOL_4:
      sui.active_display->item[index]->activated = true;

      ul_tmp = encoder_ticks + ASCII_CHARACTER_QUESTION_MARK;
      sui_check_character_range((char*) &ul_tmp);
      sui.active_display->item[index]->p_text = (char*) ul_tmp;
      sui.wifi_name[4] = (char) ul_tmp;
      break;

    case AP_NAME_SYMBOL_5:
      sui.active_display->item[index]->activated = true;

      ul_tmp = encoder_ticks + ASCII_CHARACTER_QUESTION_MARK;
      sui_check_character_range((char*) &ul_tmp);
      sui.active_display->item[index]->p_text = (char*) ul_tmp;
      sui.wifi_name[5] = (char) ul_tmp;
      break;

    case AP_NAME_SYMBOL_6:
      sui.active_display->item[index]->activated = true;

      ul_tmp = encoder_ticks + ASCII_CHARACTER_QUESTION_MARK;
      sui_check_character_range((char*) &ul_tmp);
      sui.active_display->item[index]->p_text = (char*) ul_tmp;
      sui.wifi_name[6] = (char) ul_tmp;
      break;

    case AP_NAME_SYMBOL_7:
      sui.active_display->item[index]->activated = true;

      ul_tmp = encoder_ticks + ASCII_CHARACTER_QUESTION_MARK;
      sui_check_character_range((char*) &ul_tmp);
      sui.active_display->item[index]->p_text = (char*) ul_tmp;
      sui.wifi_name[7] = (char) ul_tmp;
      break;

    case AP_NAME_SYMBOL_8:
      sui.active_display->item[index]->activated = true;

      ul_tmp = encoder_ticks + ASCII_CHARACTER_QUESTION_MARK;
      sui_check_character_range((char*) &ul_tmp);
      sui.active_display->item[index]->p_text = (char*) ul_tmp;
      sui.wifi_name[8] = (char) ul_tmp;
      break;

    case AP_NAME_SYMBOL_9:
      sui.active_display->item[index]->activated = true;

      ul_tmp = encoder_ticks + ASCII_CHARACTER_QUESTION_MARK;
      sui_check_character_range((char*) &ul_tmp);
      sui.active_display->item[index]->p_text = (char*) ul_tmp;
      sui.wifi_name[9] = (char) ul_tmp;
      break;

    case AP_NAME_SYMBOL_10:
      sui.active_display->item[index]->activated = true;

      ul_tmp = encoder_ticks + ASCII_CHARACTER_QUESTION_MARK;
      sui_check_character_range((char*) &ul_tmp);
      sui.active_display->item[index]->p_text = (char*) ul_tmp;
      sui.wifi_name[10] = (char) ul_tmp;
      break;

    case AP_NAME_SYMBOL_11:
      sui.active_display->item[index]->activated = true;

      ul_tmp = encoder_ticks + ASCII_CHARACTER_QUESTION_MARK;
      sui_check_character_range((char*) &ul_tmp);
      sui.active_display->item[index]->p_text = (char*) ul_tmp;
      sui.wifi_name[11] = (char) ul_tmp;
      break;

    case AP_NAME_SYMBOL_12:
      sui.active_display->item[index]->activated = true;

      ul_tmp = encoder_ticks + ASCII_CHARACTER_QUESTION_MARK;
      sui_check_character_range((char*) &ul_tmp);
      sui.active_display->item[index]->p_text = (char*) ul_tmp;
      sui.wifi_name[12] = (char) ul_tmp;
      break;

    case AP_NAME_SYMBOL_13:
      sui.active_display->item[index]->activated = true;

      ul_tmp = encoder_ticks + ASCII_CHARACTER_QUESTION_MARK;
      sui_check_character_range((char*) &ul_tmp);
      sui.active_display->item[index]->p_text = (char*) ul_tmp;
      sui.wifi_name[13] = (char) ul_tmp;
      break;

    case AP_NAME_SYMBOL_14:
      sui.active_display->item[index]->activated = true;

      ul_tmp = encoder_ticks + ASCII_CHARACTER_QUESTION_MARK;
      sui_check_character_range((char*) &ul_tmp);
      sui.active_display->item[index]->p_text = (char*) ul_tmp;
      sui.wifi_name[14] = (char) ul_tmp;
      break;

    case AP_NAME_SYMBOL_15:
      sui.active_display->item[index]->activated = true;

      ul_tmp = encoder_ticks + ASCII_CHARACTER_QUESTION_MARK;
      sui_check_character_range((char*) &ul_tmp);
      sui.active_display->item[index]->p_text = (char*) ul_tmp;
      sui.wifi_name[15] = (char) ul_tmp;
      break;

    case AP_NAME_ENTERED:
      sui_clear_user_credentials((char*) sui.wifi_name);
      sui.active_item_index = 0;

      system_user_interface_set_active_display(&ap_pswd_display);
      encoder_deactivate_button();
      sui.active_display->item[index]->activated = false;
      break;

    case AP_NAME_ESCAPE:
      //  @todo:
      //  renew Question mark symbols on AP Name display
      sui.active_item_index = 0;
      system_user_interface_set_active_display(&main_display);
      encoder_deactivate_button();
      break;

    case PSWD_SYMBOL_0:

      break;

    case PSWD_SYMBOL_1:

      break;

    case PSWD_SYMBOL_2:

      break;

    case PSWD_SYMBOL_3:

      break;

    case PSWD_SYMBOL_4:

      break;

    case PSWD_SYMBOL_5:

      break;

    case PSWD_SYMBOL_6:

      break;

    case PSWD_SYMBOL_7:

      break;

    case PSWD_SYMBOL_8:

      break;

    case PSWD_SYMBOL_9:

      break;

    case PSWD_SYMBOL_10:

      break;

    case PSWD_SYMBOL_11:

      break;

    case PSWD_SYMBOL_12:

      break;

    case PSWD_SYMBOL_13:

      break;

    case PSWD_SYMBOL_14:

      break;


    case PSWD_SYMBOL_15:

      break;

    case PSWD_ENTERED:
      // @todo: cleanup wifi name buffer from "?"
      sui_clear_user_credentials((char*) sui.wifi_pswd);
      sui.active_item_index = 0;

      system_user_interface_set_active_display(&main_display);
      encoder_deactivate_button();

      sui.wifi_enter_done = true;
      break;

    case PSWD_BREAK:

      break;

#endif

    default:
    break;
  }
}


#if APPLICATION_USE_NETWORK
//--------------------------------------------------------------------------------------------------
static void sui_check_character_range(char* character)
{
  if(*character < SPACE_SYMBOL)
  {
    *character = TILDA_SYMBOL;
  }
  else if(*character >= TILDA_SYMBOL)
  {
    *character = SPACE_SYMBOL;
  }
  else
  {
    // PASS
    // In range of allowed symbols
  }
}


//--------------------------------------------------------------------------------------------------
char const* sui_get_user_ssid(void)
{
  return (char const *) sui.wifi_name;
}


//--------------------------------------------------------------------------------------------------
char const * sui_get_user_pswd(void)
{
  return (char const *) sui.wifi_pswd;
}


//--------------------------------------------------------------------------------------------------
static void sui_clear_user_credentials(char* credential)
{
  for(uint8_t i = 0; i < SUI_DISPLAY_BUFFER_SIZE/2; i++)
  {
    if(credential[i] == '?')
    {
      credential[i] = '\0';
    };
  }
}


//--------------------------------------------------------------------------------------------------
bool sui_user_enter_credentials(void)
{
  return sui.wifi_enter_done;
}

#endif

//--------------------------------------------------------------------------------------------------
static uint16_t sui_update_screen_item_data(ctrl_item_id_t item_id)
{
  uint16_t    updated_data = 0;

  switch(item_id)
  {
    case CURRENT_TIME_HOURS:
      updated_data = mcu_rtc_get_hour();
    break;

    case CURRENT_TIME_MINS:
      updated_data = mcu_rtc_get_minutes();
    break;

    case LIGHT_STATUS:
      if(growbox_get_light_status())
      {
        main_screen_light.p_text  = LIGHT_ON_TEXT;
      }
      else
      {
        main_screen_light.p_text  = LIGHT_OFF_TEXT;
      }
    break;

    case TIME_HOUR_SET_LIGHT_ON:
      updated_data = schedule_get_light_t_on_hour();
    break;

    case TIME_MINUTE_SET_LIGHT_ON:
      updated_data = schedule_get_light_t_on_min();
    break;

    case LIGHT_DURATION_HOURS:
      updated_data = schedule_get_light_duration_hours();
    break;

    case CURRENT_AIR_TEMP:
      updated_data = growbox_get_mixed_air_temp();
    break;

    case AIR_HEATER:
      updated_data = (uint16_t) growbox_get_heater_status();
      if(updated_data == ENABLE)
      {
        heater_status.p_text  = "HTR:ON";
      }
      else
      {
        heater_status.p_text  = "HTR:OFF";
      }
    break;

    case AIR_OUTLET:

    break;

    case WATER_POWER:
      updated_data = water_get_pump_power();
    break;

    case WATER_LEVEL:
      updated_data = (uint16_t) growbox_get_water_level();
    break;

    case WATER_T_ON_TIME:
      updated_data = (uint8_t) schedule_get_water_duration_sec();
    break;

    case WATER_CYCLE:
      updated_data = (uint8_t) schedule_get_water_interval_mins();
    break;

#if APPLICATION_USE_NETWORK
    case ONLINE_LINK_STATUS:
      // @todo: request to network manager
      online_status.p_text = "OFL";
      online_connection_status.p_text = "STS:OFL";
    break;

    case CONNECT_WIFI_AP:
      // if static IP obtained - device connected to Wi-Fi
      //  change text "Select AP" to name of Wi-Fi network connected to
    break;
#endif

    default:
      // empty
    break;
  }

  return updated_data;
}


//--------------------------------------------------------------------------------------------------
static void sui_init_user_menu(void)
{
  sui_main_screen_init();

  sui_temperature_screen_init();

  sui_water_screen_init();

  sui_light_screen_init();

#if APPLICATION_USE_NETWORK
  sui_network_screen_init();
  sui_ap_name_screen_init();
  sui_ap_pswd_screen_init();
#endif
}


//--------------------------------------------------------------------------------------------------
static void sui_main_screen_init(void)
{
  // *** Main display
  main_screen_temperature.id                = CURRENT_AIR_TEMP;
  main_screen_temperature.type              = DISPLAY_ITEM_NUMERIC;
  main_screen_temperature.p_text            = "TMP:";
  main_screen_temperature.text_position.x   = 0;
  main_screen_temperature.text_position.y   = LCD216_FIRST_ROW;
  main_screen_temperature.update_data       = sui_update_screen_item_data;
  main_screen_temperature.data              = sui_update_screen_item_data(main_screen_temperature.id);
  main_screen_temperature.action            = sui_item_action;
  main_screen_temperature.activated         = false;

  main_screen_light.id                      = LIGHT_STATUS;
  main_screen_light.type                    = DISPLAY_ITEM_TEXTUAL;
  main_screen_light.p_text                  = NULL;
  main_screen_light.text_position.x         = 7;
  main_screen_light.text_position.y         = LCD216_FIRST_ROW;
  main_screen_light.update_data             = sui_update_screen_item_data;
  main_screen_light.data                    = sui_update_screen_item_data(main_screen_light.id);
  main_screen_light.action                  = sui_item_action;
  main_screen_light.activated               = false;

  water_level.id                            = WATER_LEVEL;
  water_level.type                          = DISPLAY_ITEM_NUMERIC;
  water_level.p_text                        = "WTR:";
  water_level.text_position.x               = 0;
  water_level.text_position.y               = LCD216_SECOND_ROW;
  water_level.update_data                   = sui_update_screen_item_data;
  water_level.data                          = sui_update_screen_item_data(water_level.id);
  water_level.action                        = sui_item_action;
  water_level.activated                     = false;

  time_hours.id                             = CURRENT_TIME_HOURS;
  time_hours.type                           = DISPLAY_ITEM_NUMERIC;       // @todo: change to DISPLAY_ITEM_NUMERIC!!!!!
  time_hours.p_text                         = NULL;                      // @todo: leave empty place holder. Set numeric value in update function
  time_hours.text_position.x                = 7;
  time_hours.text_position.y                = LCD216_SECOND_ROW;
  time_hours.update_data                    = sui_update_screen_item_data;
  time_hours.data                           = sui_update_screen_item_data(time_hours.id);
  time_hours.action                         = sui_item_action;
  time_hours.activated                      = false;

  time_mins.id                              = CURRENT_TIME_MINS;
  time_mins.type                            = DISPLAY_ITEM_NUMERIC;       // @todo: change to DISPLAY_ITEM_NUMERIC!!!!!
  time_mins.p_text                          = NULL;                       // @todo: leave empty place holder. Set numeric value in update function
  time_mins.text_position.x                 = 10;
  time_mins.text_position.y                 = LCD216_SECOND_ROW;
  time_mins.update_data                     = sui_update_screen_item_data;
  time_mins.data                            = sui_update_screen_item_data(time_mins.id);
  time_mins.action                          = sui_item_action;
  time_mins.activated                       = false;

#if APPLICATION_USE_NETWORK
  online_status.id                          = ONLINE_LINK_STATUS;
  online_status.type                        = DISPLAY_ITEM_TEXTUAL;
  online_status.p_text                      = NULL;
  online_status.text_position.x             = 13;
  online_status.text_position.y             = LCD216_SECOND_ROW;
  online_status.update_data                 = sui_update_screen_item_data;
  online_status.data                        = sui_update_screen_item_data(online_status.id);
  online_status.action                      = sui_item_action;
  online_status.activated                   = false;
#else
  word_grow.id                              = WORD_GROW;
  word_grow.type                            = DISPLAY_ITEM_TEXTUAL;
  word_grow.p_text                          = "GRW";
  word_grow.text_position.x                 = 13;
  word_grow.text_position.y                 = LCD216_SECOND_ROW;
  word_grow.update_data                     = NULL;
  word_grow.data                            = 0;
  word_grow.action                          = NULL;
  word_grow.activated                       = false;

#endif

  // Main screen items array init
  main_screen_items[0]                      = &main_screen_temperature;
  main_screen_items[1]                      = &main_screen_light;
  main_screen_items[2]                      = &water_level;
  main_screen_items[3]                      = &time_hours;
  main_screen_items[4]                      = &time_mins;
  main_screen_items[5]                      = &go_to_prev_screen;
  main_screen_items[6]                      = &go_to_next_screen;
#if APPLICATION_USE_NETWORK
  main_screen_items[7]                      = &online_status;
#else
  main_screen_items[7]                      = &word_grow;
#endif

  // Init main display object
  main_display.item       = main_screen_items;
  main_display.items_cnt  = MAIN_SCREEN_ELEMENTS_NUMBER;
  main_display.next       = &temperature_display;
#if APPLICATION_USE_NETWORK
  main_display.prev       = &network_display;
#else
  main_display.prev       = &light_display;
#endif
}


//--------------------------------------------------------------------------------------------------
static void sui_temperature_screen_init(void)
{
  temperature                       = main_screen_temperature;
  temperature.text_position.x       = 0;
  temperature.text_position.y       = LCD216_FIRST_ROW;

  heater_status.id                  = AIR_HEATER;
  heater_status.type                = DISPLAY_ITEM_TEXTUAL;
  heater_status.p_text              = NULL;
  heater_status.text_position.x     = 0;
  heater_status.text_position.y     = LCD216_SECOND_ROW;
  heater_status.update_data         = sui_update_screen_item_data;
  heater_status.data                = sui_update_screen_item_data(heater_status.id);
  heater_status.action              = sui_item_action;
  heater_status.activated           = false;

  air_outlet_status.id              = AIR_OUTLET;
  air_outlet_status.type            = DISPLAY_ITEM_TEXTUAL;
  air_outlet_status.p_text          = "OUT:";
  air_outlet_status.text_position.x = 8;
  air_outlet_status.text_position.y = LCD216_SECOND_ROW;
  air_outlet_status.update_data     = sui_update_screen_item_data;
  air_outlet_status.data            = sui_update_screen_item_data(air_outlet_status.id);
  air_outlet_status.action          = sui_item_action;
  air_outlet_status.activated       = false;

  // *** Temperature display
  temperature_screen_items[0] = &temperature;
  temperature_screen_items[1] = &heater_status;
  temperature_screen_items[2] = &air_outlet_status;
  temperature_screen_items[3] = &go_to_prev_screen;
  temperature_screen_items[4] = &go_to_next_screen;

  temperature_display.item      = temperature_screen_items;
  temperature_display.items_cnt = TEMPERATURE_SCREEN_ELEMENTS_NUMBER;
  temperature_display.next      = &water_display;
  temperature_display.prev      = &main_display;
}


//--------------------------------------------------------------------------------------------------
static void sui_water_screen_init(void)
{
  water_pump_power.id              = WATER_POWER;
  water_pump_power.type            = DISPLAY_ITEM_NUMERIC;
  water_pump_power.p_text          = "WTR PWR:";
  water_pump_power.text_position.x = 0;
  water_pump_power.text_position.y = LCD216_FIRST_ROW;
  water_pump_power.update_data     = sui_update_screen_item_data;
  water_pump_power.data            = sui_update_screen_item_data(water_level.id);
  water_pump_power.action          = sui_item_action;
  water_pump_power.activated       = false;

  water_t_on.id               = WATER_T_ON_TIME;
  water_t_on.type             = DISPLAY_ITEM_NUMERIC;
  water_t_on.p_text           = "TON:";
  water_t_on.text_position.x  = 0;
  water_t_on.text_position.y  = LCD216_SECOND_ROW;
  water_t_on.update_data      = sui_update_screen_item_data;
  water_t_on.data             = sui_update_screen_item_data(water_t_on.id);
  water_t_on.action           = sui_item_action;
  water_t_on.activated        = false;

  water_t_cycle.id                = WATER_CYCLE;
  water_t_cycle.type              = DISPLAY_ITEM_NUMERIC;
  water_t_cycle.p_text            = "CYCLE:";
  water_t_cycle.text_position.x   = 8;
  water_t_cycle.text_position.y   = LCD216_SECOND_ROW;
  water_t_cycle.update_data       = sui_update_screen_item_data;
  water_t_cycle.data              = sui_update_screen_item_data(water_t_cycle.id);
  water_t_cycle.action            = sui_item_action;
  water_t_cycle.activated         = false;

  // *** Water display
  water_screen_items[0] = &water_pump_power;
  water_screen_items[1] = &water_t_on;
  water_screen_items[2] = &water_t_cycle;
  water_screen_items[3] = &go_to_prev_screen;
  water_screen_items[4] = &go_to_next_screen;

  water_display.item      = water_screen_items;
  water_display.items_cnt = WATER_SCREEN_ELEMENTS_NUMBER;
  water_display.next      = &light_display;
  water_display.prev      = &temperature_display;
}


//--------------------------------------------------------------------------------------------------
static void sui_light_screen_init(void)
{
  light_text.id                 = LIGHT_SCREEN_TEXT;
  light_text.type               = DISPLAY_ITEM_TEXTUAL;
  light_text.p_text             = "LT ON@";
  light_text.text_position.x    = 0;
  light_text.text_position.y    = LCD216_FIRST_ROW;
  light_text.update_data        = NULL;
  light_text.action             = NULL;
  light_text.activated          = false;

  light_t_on_hour.id                 = TIME_HOUR_SET_LIGHT_ON;
  light_t_on_hour.type               = DISPLAY_ITEM_NUMERIC;
  light_t_on_hour.p_text             = NULL;
  light_t_on_hour.text_position.x    = 6;
  light_t_on_hour.text_position.y    = LCD216_FIRST_ROW;
  light_t_on_hour.update_data        = sui_update_screen_item_data;
  light_t_on_hour.data               = sui_update_screen_item_data(light_t_on_hour.id);
  light_t_on_hour.action             = sui_item_action;
  light_t_on_hour.activated          = false;

  light_t_on_min.id                 = TIME_MINUTE_SET_LIGHT_ON;
  light_t_on_min.type               = DISPLAY_ITEM_NUMERIC;
  light_t_on_min.p_text             = NULL;
  light_t_on_min.text_position.x    = 9;
  light_t_on_min.text_position.y    = LCD216_FIRST_ROW;
  light_t_on_min.update_data        = sui_update_screen_item_data;
  light_t_on_min.data               = sui_update_screen_item_data(light_t_on_min.id);
  light_t_on_min.action             = sui_item_action;
  light_t_on_min.activated          = false;


  light_duration_text.id                = LIGHT_DURATION_TEXT;
  light_duration_text.type              = DISPLAY_ITEM_TEXTUAL;
  light_duration_text.p_text            = "BE ON    HOURS";
  light_duration_text.text_position.x   = 0;
  light_duration_text.text_position.y   = LCD216_SECOND_ROW;
  light_duration_text.update_data       = NULL;
  light_duration_text.action            = NULL;
  light_duration_text.activated         = false;

  light_duration_hours.id               = LIGHT_DURATION_HOURS;
  light_duration_hours.type             = DISPLAY_ITEM_NUMERIC;
  light_duration_hours.p_text           = NULL;
  light_duration_hours.text_position.x  = 6;
  light_duration_hours.text_position.y  = LCD216_SECOND_ROW;
  light_duration_hours.update_data      = sui_update_screen_item_data;
  light_duration_hours.data             = sui_update_screen_item_data(light_duration_hours.id);
  light_duration_hours.action           = sui_item_action;
  light_duration_hours.activated        = false;


  // *** Light display
  light_screen_items[0] = &light_text;
  light_screen_items[1] = &light_t_on_hour;
  light_screen_items[2] = &light_t_on_min;
  light_screen_items[3] = &light_duration_text;
  light_screen_items[4] = &light_duration_hours;
  light_screen_items[5] = &go_to_prev_screen;
  light_screen_items[6] = &go_to_next_screen;

  light_display.item      = light_screen_items;
  light_display.items_cnt = LIGHT_SCREEN_ELEMENTS_NUMBER;
#if APPLICATION_USE_NETWORK
  light_display.next      = &network_display;
#else
  light_display.next      = &main_display;
#endif
  light_display.prev      = &water_display;
}


#if APPLICATION_USE_NETWORK
//--------------------------------------------------------------------------------------------------
static void sui_network_screen_init(void)
{
  online_connection_status.id               = ONLINE_LINK_STATUS;
  online_connection_status.type             = DISPLAY_ITEM_TEXTUAL;
  online_connection_status.p_text           = NULL;
  online_connection_status.text_position.x  = 0;
  online_connection_status.text_position.y  = LCD216_FIRST_ROW;
  online_connection_status.update_data      = sui_update_screen_item_data;
  online_connection_status.data             = sui_update_screen_item_data(online_connection_status.id);
  online_connection_status.action           = sui_item_action;
  online_connection_status.activated        = false;

  select_wi_fi_ap.id                        = CONNECT_WIFI_AP;
  select_wi_fi_ap.type                      = DISPLAY_ITEM_TEXTUAL;
  select_wi_fi_ap.p_text                    = "CONNECT AP";
  select_wi_fi_ap.text_position.x           = 0;
  select_wi_fi_ap.text_position.y           = LCD216_SECOND_ROW;
  select_wi_fi_ap.update_data               = NULL;
  select_wi_fi_ap.action                    = sui_item_action;
  select_wi_fi_ap.activated                 = false;

  // *** Connection display
  network_screen_items[0] = &online_connection_status;
  network_screen_items[1] = &select_wi_fi_ap;
  network_screen_items[2] = &go_to_prev_screen;
  network_screen_items[3] = &go_to_next_screen;

  network_display.item       = network_screen_items;
  network_display.items_cnt  = NETWORK_SCREEN_ELEMENTS_NUMBER;
  network_display.next       = &main_display;
  network_display.prev       = &light_display;
}


//--------------------------------------------------------------------------------------------------
static void sui_ap_name_screen_init(void)
{
  ap_name_text.id               = AP_NAME_TEXT;
  ap_name_text.type             = DISPLAY_ITEM_TEXTUAL;
  ap_name_text.p_text           = "WIFI:";
  ap_name_text.text_position.x  = 0;
  ap_name_text.text_position.y  = LCD216_FIRST_ROW;
  ap_name_text.update_data      = sui_update_screen_item_data;
  ap_name_text.data             = 0;
  ap_name_text.action           = NULL;
  ap_pswd_text.activated        = false;

  for(uint8_t i = 0; i < 16; i++)
  {
    ap_name[i].id               = AP_NAME_SYMBOL_0 + i;
    ap_name[i].type             = DISPLAY_ITEM_TEXTUAL;
    ap_name[i].p_text           = "?";
    ap_name[i].text_position.x  = i;
    ap_name[i].text_position.y  = LCD216_SECOND_ROW;
    ap_name[i].update_data      = NULL;
    ap_name[i].data             = 0;
    ap_name[i].action           = sui_item_action;
    ap_name[i].activated        = false;
  }

  ap_name_escape.id               = AP_NAME_ESCAPE;
  ap_name_escape.type             = DISPLAY_ITEM_TEXTUAL;
  ap_name_escape.p_text           = "ESC";
  ap_name_escape.text_position.x  = 12;
  ap_name_escape.text_position.y  = LCD216_FIRST_ROW;
  ap_name_escape.update_data      = NULL;
  ap_name_escape.data             = 0;
  ap_name_escape.action           = sui_item_action;
  ap_name_escape.activated        = false;

  ap_name_done.id                 = AP_NAME_ENTERED;
  ap_name_done.type               = DISPLAY_ITEM_TEXTUAL;
  ap_name_done.p_text             = "OK";
  ap_name_done.text_position.x    = 9;
  ap_name_done.text_position.y    = LCD216_FIRST_ROW;
  ap_name_done.update_data        = NULL;
  ap_name_done.data               = 0;
  ap_name_done.action             = sui_item_action;
  ap_name_done.activated          = false;

  ap_name_screen_items[0] = &ap_name_text;

  for(uint8_t i = 0; i < WIFI_NAME_LEN_MAX; i++)
  {
    ap_name_screen_items[i+1] = &ap_name[i];
  }

  ap_name_screen_items[17] = &ap_name_done;
  ap_name_screen_items[18] = &ap_name_escape;

  ap_name_display.item      = ap_name_screen_items;
  ap_name_display.items_cnt = AP_NAME_SCREEN_ELEMENTS_NUMBER;
  ap_name_display.next      = NULL;
  ap_name_display.prev      = NULL;
}


//--------------------------------------------------------------------------------------------------
static void sui_ap_pswd_screen_init(void)
{
  ap_pswd_text.id               = PSWD_TEXT;
  ap_pswd_text.type             = DISPLAY_ITEM_TEXTUAL;
  ap_pswd_text.p_text           = "PSWD:";
  ap_pswd_text.text_position.x  = 0;
  ap_pswd_text.text_position.y  = LCD216_FIRST_ROW;
  ap_pswd_text.update_data      = NULL;
  ap_pswd_text.data             = 0;
  ap_pswd_text.action           = NULL;
  ap_pswd_text.activated        = false;

  for(uint8_t i = 0; i < 16; i++)
  {
    ap_pswd[i].id               = PSWD_SYMBOL_0 + i;
    ap_pswd[i].type             = DISPLAY_ITEM_TEXTUAL;
    ap_pswd[i].p_text           = "?";
    ap_pswd[i].text_position.x  = i;
    ap_pswd[i].text_position.y  = LCD216_SECOND_ROW;
  }

  ap_pswd_done.id = PSWD_ENTERED;
  ap_pswd_done.type = DISPLAY_ITEM_TEXTUAL;
  ap_pswd_done.p_text = "CONNECT";


  ap_pswd_cancel.id = PSWD_BREAK;
  ap_pswd_cancel.type = DISPLAY_ITEM_TEXTUAL;
  ap_pswd_cancel.p_text = "ESC";
  ap_pswd_cancel.action = sui_item_action;
}
#endif


//--------------------------------------------------------------------------------------------------
static void sui_go_to_prev_screen(int16_t dummy)
{
  (void) dummy;
  lcd216_clear();
  sui.active_item_index = 0;
  system_user_interface_set_active_display(sui.active_display->prev);
  encoder_deactivate_button();
  go_to_prev_screen.activated = false;
}


//--------------------------------------------------------------------------------------------------
static void sui_go_to_next_screen(int16_t dummy)
{
  (void) dummy;
  lcd216_clear();
  sui.active_item_index = 0;
  system_user_interface_set_active_display(sui.active_display->next);
  encoder_deactivate_button();
  go_to_next_screen.activated = false;
}

