#ifndef LCD216_H
#define LCD216_H

/**
LCD   STM32Fxxx         DESCRIPTION

GND   GND               Ground
VCC   +5V               Power supply for LCD
V0    Potentiometer	    Contrast voltage. Connect to potentiometer
RS    PA7               Register select, can be overwritten
RW    GND               Read/write
E     PA6               Enable pin, can be overwritten in your project's defines.h file
D0    -                 Data 0 - doesn't care
D1    -                 Data 1 - doesn't care
D2    -                 Data 2 - doesn't care
D3    -                 Data 3 - doesn't  care
D4    PB12              Data 4, can be overwritten
D5    PB13              Data 5, can be overwritten
D6    PB14              Data 6, can be overwritten
D7    PB15              Data 7, can be overwritten
A     +3V3              Back light positive power
K     GND               Ground for back light

*/
#include <stdint.h>

#define LCD216_ROWS                         2
#define LCD216_COLUMNS                      16

#define LCD216_FIRST_ROW                    0
#define LCD216_SECOND_ROW                   (LCD216_ROWS - 1)


/**
 * @brief  Initializes HD44780 LCD
 * @retval None
 */
void lcd216_init(void);

/**
 * @brief  Turn display on
 * @param  None
 * @retval None
 */
void lcd216_display_on(void);

/**
 * @brief  Turn display off
 * @param  None
 * @retval None
 */
void lcd216_display_off(void);

/**
 * @brief  Clears entire LCD
 * @param  None
 * @retval None
 */
void lcd216_clear(void);

/**
 * @brief  Puts string on lcd
 * @param  x: X location where string will start
 * @param  y; Y location where string will start
 * @param  str : pointer to string to display
 * @retval None
 */
void lcd216_puts(int x, int y, char* str);

/**
 * @brief  Enables cursor blink
 * @param  None
 * @retval None
 */
void lcd216_blink_on(void);

/**
 * @brief  Disables cursor blink
 * @param  None
 * @retval None
 */
void lcd216_blink_off(void);

/**
 * @brief  Shows cursor
 * @param  None
 * @retval None
 */
void lcd216_cursor_on(void);

/**
 * @brief  Hides cursor
 * @param  None
 * @retval None
 */
void lcd216_cursor_off(void);

/**
 * @brief  Scrolls display to the left
 * @param  None
 * @retval None
 */
void lcd216_scroll_left(void);

/**
 * @brief  Scrolls display to the right
 * @param  None
 * @retval None
 */
void lcd216_scroll_right(void);

/**
 * @brief  Creates custom character
 * @param  location: Location where to save character on LCD. LCD supports up to 8 custom characters, so locations are 0 - 7
 * @param data: Pointer to 8-bytes of data for one character
 * @retval None
 */
void lcd216_create_char(int location, int* data);

/**
 * @brief  Puts custom created character on LCD
 * @param  x: X location where character will be shown
 * @param  y: Y location where character will be shown
 * @param  location: Location on LCD where character is stored, 0 - 7
 * @retval None
 */
void lcd216_put_custom(int x, int y, int location);


//==============================================================================
//==============================================================================
void lcd216_cursor_set(int col, int row);

#endif  // LCD216_H
