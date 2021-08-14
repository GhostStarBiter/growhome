//==============================================================================
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ D O M A ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//==============================================================================
//
/// @file   display_2x16_static.h
///
/// @brief  HD44780 2x16 lcd display module static header
///
//==============================================================================
#ifndef LCD216_PRIV_H
#define LCD216_PRIV_H

#include "configuration/peripherals_config.h"
#include "stm32f10x_gpio.h"

#include "gpio/mcu_gpio.h"      // debug

#include "task.h"

#include "lcd216.h"


typedef struct {
  int DisplayControl;
  int DisplayFunction;
  int DisplayMode;
  int Rows;
  int Cols;
  int currentX;
  int currentY;
} lcd216_t;


/* Commands*/
#define LCD216_CLEARDISPLAY        0x01
#define LCD216_RETURNHOME          0x02
#define LCD216_ENTRYMODESET        0x04
#define LCD216_DISPLAYCONTROL      0x08
#define LCD216_CURSORSHIFT         0x10
#define LCD216_FUNCTIONSET         0x20
#define LCD216_SETCGRAMADDR        0x40
#define LCD216_SETDDRAMADDR        0x80

/* Flags for display entry mode */
#define LCD216_ENTRYRIGHT          0x00
#define LCD216_ENTRYLEFT           0x02
#define LCD216_ENTRYSHIFTINCREMENT 0x01
#define LCD216_ENTRYSHIFTDECREMENT 0x00

/* Flags for display on/off control */
#define LCD216_DISPLAY_OFF         0x00
#define LCD216_DISPLAYON           0x04
#define LCD216_CURSORON            0x02
#define LCD216_BLINKON             0x01

/* Flags for display/cursor shift */
#define LCD216_DISPLAYMOVE         0x08
#define LCD216_CURSORMOVE          0x00
#define LCD216_MOVERIGHT           0x04
#define LCD216_MOVELEFT            0x00

/* Flags for function set */
#define LCD216_8BITMODE            0x10
#define LCD216_4BITMODE            0x00
#define LCD216_2LINE               0x08
#define LCD216_1LINE               0x00
#define LCD216_5x10DOTS            0x04
#define LCD216_5x8DOTS             0x00

/* Pin definitions */
#define LCD216_RS_LOW              GPIO_ResetBits(LCD216_RS_PORT, LCD216_RS_PIN)
#define LCD216_RS_HIGH             GPIO_SetBits(LCD216_RS_PORT, LCD216_RS_PIN)
//#define LCD216_E_LOW               GPIO_ResetBits(LCD216_PORT, LCD216_E_PIN)
//#define LCD216_E_HIGH              GPIO_SetBits(LCD216_PORT, LCD216_E_PIN)
//
//#define LCD216_E_BLINK             (LCD216_E_HIGH; vTaskDelay(40); LCD216_E_LOW; vTaskDelay(40))


//==============================================================================
//==============================================================================
static void lcd216_cmd(int cmd);


//==============================================================================
//==============================================================================
static void lcd216_cmd4bit(int cmd);


//==============================================================================
//==============================================================================
static void lcd216_data(int data);


//==============================================================================
//==============================================================================
static void lcd216_reset_ctrl_pins(void);


//==============================================================================
//==============================================================================
static void lcd216_reset_data_pins(void);

#endif // LCD216_PRIV_H
