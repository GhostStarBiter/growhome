#include "sui_priv.h"

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
    .action                   = sui_go_to_prev_screen
};

screen_item_t go_to_next_screen = {
    .p_text                   = ">",
    .type                     = DISPLAY_ITEM_TEXTUAL,
    .text_position.x          = 15,
    .text_position.y          = LCD216_FIRST_ROW,
    .action                   = sui_go_to_next_screen
};

//------------------------------------------------------------------------------
//  *** MAIN SCREEN
screen_item_t main_screen_temperature,
              main_screen_light,
              water_level,
              time_hours,
              time_mins,
              online_status;

screen_item_t main_screen_items[MAIN_SCREEN_ELEMENTS_NUMBER];

//------------------------------------------------------------------------------
//  *** TEMPERATURE SCREEN
screen_item_t temperature,
              heater_status,
              air_outlet_status;

screen_item_t temperature_screen_items[TEMPERATURE_SCREEN_ELEMENTS_NUMBER];

//------------------------------------------------------------------------------
//  *** WATER SCREEN
screen_item_t water_screen_level,
              water_t_on,
              water_t_off;

screen_item_t water_screen_items[WATER_SCREEN_ELEMENTS_NUMBER];

//------------------------------------------------------------------------------
//  *** LIGHT SCREEN
screen_item_t light_status,
              light_t_on,
              light_t_off;

screen_item_t light_screen_items[LIGHT_SCREEN_ELEMENTS_NUMBER];

//------------------------------------------------------------------------------
//  *** CONNECT TO IP SCREEN
screen_item_t online_connection_status,
              select_wi_fi_ap;

screen_item_t connection_screen_items[CONNECTION_SCREEN_ELEMENTS_NUMBER];

//------------------------------------------------------------------------------
//  *** SET TIME SCREEN
screen_item_t set_time_screen_text,
              set_time_screen_hours,
              set_time_screen_minutes;

screen_item_t set_time_screen_items[SET_TIME_SCREEN_ELEMENTS_NUMBER];


// *** User menu displays
display_screen_t  main_display,
                  temperature_display,
                  water_display,
                  light_display,
                  connection_display,
                  set_time_display;



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
    x_pos = p_set_screen->item[i].text_position.x;
    y_pos = p_set_screen->item[i].text_position.y;

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
    strncpy((char*) &sui.display_buffer[buf_position], p_set_screen->item[i].p_text, strlen(p_set_screen->item[i].p_text));
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
  const TickType_t x_interface_task_timeout_ms = INTERFACE_TASK_TIMEOUT;
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
    vTaskDelayUntil(&x_last_wake_time, x_interface_task_timeout_ms);

    // ***
    system_user_interface_update();
  }
}


//--------------------------------------------------------------------------------------------------
static void system_user_interface_update(void)
{
  uint8_t index;
  static int8_t  prev_ticks = 0;

  int8_t current_ticks = encoder_get_ticks();
  index = sui.active_item_index;

  if(encoder_button_activated())
  {
    growbox_set_control_mode(CONTROL_MODE_MANUAL);                            // NOTE: growbox module will go back to AUTOMATIC after internal timeout (1 min)
    sui.active_display->item[index].activated = true;
    sui.active_display->item[index].action(current_ticks);
  }
  else
  {
    sui.active_display->item[index].activated = false;
    if(current_ticks != 0)
    {
      if(prev_ticks != current_ticks)
      {
        sui.active_item_index += (prev_ticks - current_ticks);
      }

      while(sui.active_item_index > sui.active_display->items_cnt)
      {
        sui.active_item_index %= (sui.active_display->items_cnt);
      }

      index = sui.active_item_index;
      lcd216_cursor_set(  sui.active_display->item[index].text_position.x,
                          sui.active_display->item[index].text_position.y);
      prev_ticks = current_ticks;
    }
  }

  system_user_update_display();
}


//--------------------------------------------------------------------------------------------------
static void system_user_update_display(void)
{
  ctrl_item_id_t    item_id;
  display_screen_t* display;
  uint8_t           item_text_len = 0;
  char*             item_data_string;
  uint8_t           buff_pos;
  uint8_t           m;

  // ***
  display = sui.active_display;


  for(uint8_t index=0; index < display->items_cnt; index++)
  {
    // display buffer insert position
    buff_pos = 0;

    item_id = display->item[index].id;

    // call update item's data function
    if(display->item[index].update_data != NULL && !display->item[index].activated)
    {
      display->item[index].data = display->item[index].update_data(item_id);
    }

    // Calculate buffer insert position
    buff_pos = display->item[index].text_position.x;
    // correct buffer insert position according to item's position row
    (display->item[index].text_position.y > 0) ? (buff_pos += 16) : (buff_pos);

    // Copy text of item to buffer
    strncpy(  (char*) &sui.display_buffer[buff_pos],
              display->item[index].p_text,
              strlen(display->item[index].p_text));

    // ***
    // CONVERT NUMBERS TO CHARACTERS
    if(display->item[index].type == DISPLAY_ITEM_NUMERIC)
    {
      if(display->item[index].p_text != NULL)
      {
        // length of textual specifier of item (TMP, WTR etc.)
        item_text_len = strlen(display->item[index].p_text);
      }
      else
      {
        item_text_len = 0;
      }

      // string with item data(numeric) converted to text
      item_data_string = convert_num_to_str(display->item[index].data);

      buff_pos = 0;
      buff_pos = display->item[index].text_position.x + item_text_len;
      // correct buffer insert position according to item's position row
      (display->item[index].text_position.y > 0) ? (buff_pos += 16) : (buff_pos);

      // zeroing place for new data
      sui.display_buffer[buff_pos] = '0';
      sui.display_buffer[buff_pos+1] = '0';

      // shift character 1 position right if it is less than 0
      if(display->item[index].data < 10)
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
  ctrl_item_id_t  parameter = sui.active_display->item[index].id;
  uint16_t        tmp = 0;
  char*           data_text;

  switch(parameter)
  {
    case CURRENT_TIME_HOURS:
      // get instant time
      // add/substract encoder ticks
      // set value


      encoder_deactivate_button();  // temporary code
    break;

    case CURRENT_TIME_MINS:

      encoder_deactivate_button();  // temporary code
    break;

    case LIGHT_STATUS:
      // change light status
      if(growbox_get_light_status())
      {
        growbox_set_light(DISABLE);
      }
      else
      {
        growbox_set_light(ENABLE);
      }
      encoder_deactivate_button();
    break;

    case LIGHT_T_ON_TIME:

      encoder_deactivate_button();  // temporary code
    break;

    case LIGHT_T_OFF_TIME:

      encoder_deactivate_button();  // temporary code
    break;

    case CURRENT_AIR_TEMP:
      while(encoder_button_activated())
      {
        temperature.data += encoder_ticks;
        main_screen_temperature.data = temperature.data;

        tmp = strlen(temperature.p_text);

        if(temperature.data > 35)   // @TODO: max temp make a parameter
        {
          temperature.data = 35;
        }

        data_text = convert_num_to_str(temperature.data);
        lcd216_puts(temperature.text_position.x + tmp,
                    temperature.text_position.y,
                    data_text);

        encoder_reset_ticks();
        vTaskDelay(40);
      }
    break;

    case AIR_HEATER:

      encoder_deactivate_button();  // temporary code
    break;

    case AIR_OUTLET:

      encoder_deactivate_button();  // temporary code
    break;

    case WATERING_STATUS:

      encoder_deactivate_button();  // temporary code
    break;

    case WATER_T_ON_TIME:

      encoder_deactivate_button();  // temporary code
    break;

    case WATER_T_OFF_TIME:

      encoder_deactivate_button();  // temporary code
    break;

    case ONLINE_LINK_STATUS:

      encoder_deactivate_button();  // temporary code
    break;

    case SELECT_WIFI_AP:

      encoder_deactivate_button();  // temporary code
    break;

    case WATER_LEVEL:
    default:
      encoder_deactivate_button();  // temporary code
      // empty
    break;

  }
}



//--------------------------------------------------------------------------------------------------
static uint16_t sui_update_screen_item_data(ctrl_item_id_t item_id)
{
  uint16_t    updated_data = 0;
  //uint8_t     tmp = 0;

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
        light_status.p_text       = LIGHT_ON_TEXT;
      }
      else
      {
        main_screen_light.p_text  = LIGHT_OFF_TEXT;
        light_status.p_text       = LIGHT_OFF_TEXT;
      }
    break;

    case LIGHT_T_ON_TIME:

    break;

    case LIGHT_T_OFF_TIME:

    break;

    case CURRENT_AIR_TEMP:
      updated_data = growbox_get_mixed_air_temp();
    break;

    case AIR_HEATER:

    break;

    case AIR_OUTLET:

    break;

    case WATERING_STATUS:

    break;

    case WATER_LEVEL:
      updated_data = growbox_get_water_level();
    break;

    case WATER_T_ON_TIME:
      // growbox_get_water_t_on_time()
    break;

    case WATER_T_OFF_TIME:
      // growbox_get_water_t_off_time()
    break;

    case ONLINE_LINK_STATUS:

    break;

    case SELECT_WIFI_AP:

    break;

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

  sui_connection_screen_init();

  sui_set_time_screen_init();
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

  online_status.id                          = ONLINE_LINK_STATUS;
  online_status.type                        = DISPLAY_ITEM_TEXTUAL;
  online_status.p_text                      = "OFL";
  online_status.text_position.x             = 13;
  online_status.text_position.y             = LCD216_SECOND_ROW;
  online_status.update_data                 = sui_update_screen_item_data;
  online_status.data                        = sui_update_screen_item_data(online_status.id);
  online_status.action                      = sui_item_action;
  online_status.activated                   = false;

  // Main screen items array init
  main_screen_items[0]                      = main_screen_temperature;
  main_screen_items[1]                      = main_screen_light;
  main_screen_items[2]                      = water_level;
  main_screen_items[3]                      = time_hours;
  main_screen_items[4]                      = time_mins;
  main_screen_items[5]                      = online_status;
  main_screen_items[6]                      = go_to_prev_screen;
  main_screen_items[7]                      = go_to_next_screen;

  // Init main display object
  main_display.item       = main_screen_items;
  main_display.items_cnt  = MAIN_SCREEN_ELEMENTS_NUMBER;
  main_display.next       = &temperature_display;
  main_display.prev       = &set_time_display;
}


//--------------------------------------------------------------------------------------------------
static void sui_temperature_screen_init(void)
{
  temperature                       = main_screen_temperature;
  temperature.text_position.x       = 0;
  temperature.text_position.y       = LCD216_FIRST_ROW;

  heater_status.id                  = AIR_HEATER;
  heater_status.type                = DISPLAY_ITEM_TEXTUAL;
  heater_status.p_text              = "HTR:";
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
  temperature_screen_items[0] = temperature;
  temperature_screen_items[1] = heater_status;
  temperature_screen_items[2] = air_outlet_status;
  temperature_screen_items[3] = go_to_prev_screen;
  temperature_screen_items[4] = go_to_next_screen;

  temperature_display.item      = temperature_screen_items;
  temperature_display.items_cnt = TEMPERATURE_SCREEN_ELEMENTS_NUMBER;
  temperature_display.next      = &water_display;
  temperature_display.prev      = &main_display;
}


//--------------------------------------------------------------------------------------------------
static void sui_water_screen_init(void)
{
  water_screen_level.id              = WATERING_STATUS;
  water_screen_level.type            = DISPLAY_ITEM_NUMERIC;
  water_screen_level.p_text          = "WTR:";
  water_screen_level.text_position.x = 0;
  water_screen_level.text_position.y = LCD216_FIRST_ROW;
  water_screen_level.update_data     = sui_update_screen_item_data;
  water_screen_level.data            = sui_update_screen_item_data(water_level.id);
  water_screen_level.action          = sui_item_action;
  water_screen_level.activated       = false;

  water_t_on.id               = WATER_T_ON_TIME;
  water_t_on.type             = DISPLAY_ITEM_NUMERIC;
  water_t_on.p_text           = "TON:";
  water_t_on.text_position.x  = 0;
  water_t_on.text_position.y  = LCD216_SECOND_ROW;
  water_t_on.update_data      = sui_update_screen_item_data;
  water_t_on.data             = sui_update_screen_item_data(water_t_on.id);
  water_t_on.action           = sui_item_action;
  water_t_on.activated        = false;

  water_t_off.id                = WATER_T_OFF_TIME;
  water_t_off.type              = DISPLAY_ITEM_NUMERIC;
  water_t_off.p_text            = "TOFF:";
  water_t_off.text_position.x   = 8;
  water_t_off.text_position.y   = LCD216_SECOND_ROW;
  water_t_off.update_data       = sui_update_screen_item_data;
  water_t_off.data              = sui_update_screen_item_data(water_t_off.id);
  water_t_off.action            = sui_item_action;
  water_t_off.activated         = false;

  // *** Water display
  water_screen_items[0] = water_screen_level;
  water_screen_items[1] = water_t_on;
  water_screen_items[2] = water_t_off;
  water_screen_items[3] = go_to_prev_screen;
  water_screen_items[4] = go_to_next_screen;

  water_display.item      = water_screen_items;
  water_display.items_cnt = WATER_SCREEN_ELEMENTS_NUMBER;
  water_display.next      = &light_display;
  water_display.prev      = &temperature_display;
}


//--------------------------------------------------------------------------------------------------
static void sui_light_screen_init(void)
{
  light_status.id               = LIGHT_STATUS;
  light_status.type             = DISPLAY_ITEM_TEXTUAL;
  light_status.p_text           = NULL;
  light_status.text_position.x  = 0;
  light_status.text_position.y  = LCD216_FIRST_ROW;
  light_status.update_data      = sui_update_screen_item_data;
  light_status.data             = sui_update_screen_item_data(light_status.id);
  light_status.action           = sui_item_action;
  light_status.activated        = false;

  light_t_on.id                 = LIGHT_T_ON_TIME;
  light_t_on.type               = DISPLAY_ITEM_NUMERIC;
  light_t_on.p_text             = "TON:";
  light_t_on.text_position.x    = 0;
  light_t_on.text_position.y    = LCD216_SECOND_ROW;
  light_t_on.update_data        = sui_update_screen_item_data;
  light_t_on.data               = sui_update_screen_item_data(light_t_on.id);
  light_t_on.action             = sui_item_action;
  light_t_on.activated          = false;

  light_t_off.id                = LIGHT_T_OFF_TIME;
  light_t_off.type              = DISPLAY_ITEM_NUMERIC;
  light_t_off.p_text            = "TOFF:";
  light_t_off.text_position.x   = 8;
  light_t_off.text_position.y   = LCD216_SECOND_ROW;
  light_t_off.update_data       = sui_update_screen_item_data;
  light_t_off.data              = sui_update_screen_item_data(light_t_off.id);
  light_t_off.action            = sui_item_action;
  light_t_off.activated         = false;


  // *** Light display
  light_screen_items[0] = light_status;
  light_screen_items[1] = light_t_on;
  light_screen_items[2] = light_t_off;
  light_screen_items[3] = go_to_prev_screen;
  light_screen_items[4] = go_to_next_screen;

  light_display.item      = light_screen_items;
  light_display.items_cnt = LIGHT_SCREEN_ELEMENTS_NUMBER;
  light_display.next      = &connection_display;
  light_display.prev      = &water_display;
}


//--------------------------------------------------------------------------------------------------
static void sui_connection_screen_init(void)
{
  online_connection_status.id               = ONLINE_LINK_STATUS;
  online_connection_status.type             = DISPLAY_ITEM_TEXTUAL;
  online_connection_status.p_text           = "STS:";
  online_connection_status.text_position.x  = 0;
  online_connection_status.text_position.y  = LCD216_FIRST_ROW;
  online_connection_status.update_data      = sui_update_screen_item_data;
  online_connection_status.data             = sui_update_screen_item_data(online_connection_status.id);
  online_connection_status.action           = sui_item_action;
  online_connection_status.activated        = false;

  select_wi_fi_ap.id                        = SELECT_WIFI_AP;
  select_wi_fi_ap.type                      = DISPLAY_ITEM_TEXTUAL;
  select_wi_fi_ap.p_text                    = "SELECT AP";
  select_wi_fi_ap.text_position.x           = 0;
  select_wi_fi_ap.text_position.y           = LCD216_SECOND_ROW;
  select_wi_fi_ap.update_data               = NULL;
  select_wi_fi_ap.action                    = sui_item_action;
  select_wi_fi_ap.activated                 = false;

  // *** Connection display
  connection_screen_items[0] = online_connection_status;
  connection_screen_items[1] = select_wi_fi_ap;
  connection_screen_items[3] = go_to_prev_screen;
  connection_screen_items[4] = go_to_next_screen;

  connection_display.item       = connection_screen_items;
  connection_display.items_cnt  = CONNECTION_SCREEN_ELEMENTS_NUMBER;
  connection_display.next       = &set_time_display;
  connection_display.prev       = &light_display;
}


//--------------------------------------------------------------------------------------------------
static void sui_set_time_screen_init(void)
{
  set_time_screen_text.p_text             = "HOUR MIN";
  set_time_screen_text.type               = DISPLAY_ITEM_TEXTUAL;
  set_time_screen_text.text_position.x    = 0;
  set_time_screen_text.text_position.y    = LCD216_FIRST_ROW;
  set_time_screen_text.update_data        = NULL;
  set_time_screen_text.action             = NULL;

  set_time_screen_hours.id                = CURRENT_TIME_HOURS;
  set_time_screen_hours.type              = DISPLAY_ITEM_NUMERIC;
  set_time_screen_hours.p_text            = NULL;
  set_time_screen_hours.text_position.x   = 2;
  set_time_screen_hours.text_position.y   = LCD216_SECOND_ROW;
  set_time_screen_hours.update_data       = sui_update_screen_item_data;
  set_time_screen_hours.data              = sui_update_screen_item_data(set_time_screen_hours.id);
  set_time_screen_hours.action            = sui_item_action;
  set_time_screen_hours.activated         = false;

  set_time_screen_minutes.id              = CURRENT_TIME_MINS;
  set_time_screen_minutes.type            = DISPLAY_ITEM_NUMERIC;
  set_time_screen_minutes.p_text          = NULL;
  set_time_screen_minutes.text_position.x = 5;
  set_time_screen_minutes.text_position.y = LCD216_SECOND_ROW;
  set_time_screen_minutes.update_data     = sui_update_screen_item_data;
  set_time_screen_minutes.data            = sui_update_screen_item_data(set_time_screen_minutes.id);
  set_time_screen_minutes.action          = sui_item_action;
  set_time_screen_minutes.activated       = false;

  // *** Set time display
  set_time_screen_items[0] = set_time_screen_text;
  set_time_screen_items[1] = set_time_screen_hours;
  set_time_screen_items[2] = set_time_screen_minutes;
  set_time_screen_items[3] = go_to_prev_screen;
  set_time_screen_items[4] = go_to_next_screen;

  set_time_display.item       = set_time_screen_items;
  set_time_display.items_cnt  = SET_TIME_SCREEN_ELEMENTS_NUMBER;
  set_time_display.next       = &main_display;
  set_time_display.prev       = &connection_display;
}


//--------------------------------------------------------------------------------------------------
static void sui_go_to_prev_screen(int16_t dummy)
{
  (void) dummy;
  lcd216_clear();
  sui.active_item_index = 0;
  system_user_interface_set_active_display(sui.active_display->prev);
  encoder_deactivate_button();
}


//--------------------------------------------------------------------------------------------------
static void sui_go_to_next_screen(int16_t dummy)
{
  (void) dummy;
  lcd216_clear();
  sui.active_item_index = 0;
  system_user_interface_set_active_display(sui.active_display->next);
  encoder_deactivate_button();
}

