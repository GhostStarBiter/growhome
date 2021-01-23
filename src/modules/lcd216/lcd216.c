#include "lcd216_priv.h"

lcd216_t lcd216;


//==============================================================================
void lcd216_init(void)
{
    /* Set LCD width and height */
  lcd216.Rows = LCD216_ROWS;
  lcd216.Cols = LCD216_COLUMNS;
    /* Set cursor pointer to beginning for LCD */
  lcd216.currentX = 0;
  lcd216.currentY = 0;

  lcd216_reset_ctrl_pins();
  lcd216_reset_data_pins();
  vTaskDelay(50);

  /* set 4bit mode */
  lcd216_cmd4bit(0x03);
  vTaskDelay(50);

  lcd216_cmd4bit(0x03);
  vTaskDelay(50);

  lcd216_cmd4bit(0x03);
  vTaskDelay(50);

  lcd216_cmd4bit(0x02);
  vTaskDelay(50);

  //Function Set: 2-line, 5x8 matrix
  lcd216_cmd(LCD216_FUNCTIONSET|LCD216_2LINE|LCD216_5x8DOTS);
  vTaskDelay(50);

  //Display Off, Cursor Off, Blink Off
  lcd216_cmd(LCD216_DISPLAYCONTROL|LCD216_DISPLAY_OFF);
  vTaskDelay(50);
  //Clear Screen & Return Cursor Home
  lcd216_cmd(LCD216_CLEARDISPLAY);
  vTaskDelay(50);

  //Inc cursor to right then writing & don't shift screen
  lcd216_cmd(LCD216_ENTRYMODESET|LCD216_ENTRYLEFT|LCD216_ENTRYSHIFTDECREMENT);
  vTaskDelay(50);

  //Display On, Cursor On, Blink Off
  lcd216_cmd(LCD216_DISPLAYCONTROL|LCD216_DISPLAYON|LCD216_CURSORON);
  vTaskDelay(50);

  lcd216_clear();

  lcd216_display_on();
}


//==============================================================================
void lcd216_clear(void) {
	lcd216_cmd(LCD216_CLEARDISPLAY);
	vTaskDelay(10);
}


//==============================================================================
void lcd216_puts(int x, int y, char* str) {
  lcd216_cursor_set(x, y);
  while (*str) {
    if (lcd216.currentX >= lcd216.Cols)
    {
      lcd216.currentX = 0;
      lcd216.currentY++;
      lcd216_cursor_set(lcd216.currentX, lcd216.currentY);
    }

    if (*str == '\n')
    {
      lcd216.currentY++;
      lcd216_cursor_set(lcd216.currentX, lcd216.currentY);
    }
    else if (*str == '\r')
    {
      lcd216_cursor_set(0, lcd216.currentY);
    }
    else if (*str == '\0')
      return;
    else {
      lcd216_data(*str);
      lcd216.currentX++;
    }
    str++;
  }
}


//==============================================================================
void lcd216_display_on(void) {
	lcd216.DisplayControl |= LCD216_DISPLAYON;
	lcd216_cmd(LCD216_DISPLAYCONTROL | lcd216.DisplayControl);
}


//==============================================================================
void lcd216_display_off(void) {
	lcd216.DisplayControl &= ~LCD216_DISPLAYON;
	lcd216_cmd(LCD216_DISPLAYCONTROL | lcd216.DisplayControl);
}


//==============================================================================
void lcd216_blink_on(void) {
	lcd216.DisplayControl |= LCD216_BLINKON;
	lcd216_cmd(LCD216_DISPLAYCONTROL | lcd216.DisplayControl);
}


//==============================================================================
void lcd216_blink_off(void) {
	lcd216.DisplayControl &= ~LCD216_BLINKON;
	lcd216_cmd(LCD216_DISPLAYCONTROL | lcd216.DisplayControl);
}


//==============================================================================
void lcd216_cursor_on(void) {
	lcd216.DisplayControl |= LCD216_CURSORON;
	lcd216_cmd(LCD216_DISPLAYCONTROL | lcd216.DisplayControl);
}


//==============================================================================
void lcd216_cursor_off(void) {
	lcd216.DisplayControl &= ~LCD216_CURSORON;
	lcd216_cmd(LCD216_DISPLAYCONTROL | lcd216.DisplayControl);
}


//==============================================================================
void lcd216_scroll_left(void) {
  lcd216_cmd(LCD216_CURSORSHIFT | LCD216_DISPLAYMOVE | LCD216_MOVELEFT);
}


//==============================================================================
void lcd216_scroll_right(void) {
  lcd216_cmd(LCD216_CURSORSHIFT | LCD216_DISPLAYMOVE | LCD216_MOVERIGHT);
}


//==============================================================================
void lcd216_create_char(int location, int *data) {
	short i;
	/* We have 8 locations available for custom characters */
	location &= 0x07;
	lcd216_cmd(LCD216_SETCGRAMADDR | (location << 3));
	
	for (i = 0; i < 8; i++) {
	  lcd216_data(data[i]);
	}
}


//==============================================================================
void lcd216_put_custom(int x, int y, int location) {
  lcd216_cursor_set(x, y);
	lcd216_data(location);
}


//==============================================================================
/* static functions */
static void lcd216_cmd(int cmd) {
	/* Command mode */
	LCD216_RS_LOW;
	/* High nibble */
	lcd216_cmd4bit(cmd >> 4);
	/* Low nibble */
	lcd216_cmd4bit(cmd & 0x0F);
	LCD216_RS_HIGH;
}


//==============================================================================
static void lcd216_data(int data) {
	/* Data mode */
  LCD216_RS_HIGH;
	/* High nibble */
    int high_nibble = data >> 4;
	lcd216_cmd4bit(high_nibble);
	/* Low nibble */
    int low_nibble = data & 0x0F;
	lcd216_cmd4bit(low_nibble);
}


//==============================================================================
static void lcd216_cmd4bit(int cmd) {
	/* Set output port */
	if((cmd & 0x08) == 8)
		GPIO_SetBits(LCD216_D7_PORT, LCD216_D7_PIN);
	if((cmd & 0x04) == 4)
		GPIO_SetBits(LCD216_D6_PORT, LCD216_D6_PIN);
	if((cmd & 0x02) == 2)
		GPIO_SetBits(LCD216_D5_PORT, LCD216_D5_PIN);
	if((cmd & 0x01) == 1)
		GPIO_SetBits(LCD216_D4_PORT, LCD216_D4_PIN);
	//LCD_EN pin strobe
	GPIO_SetBits(LCD216_EN_PORT, LCD216_EN_PIN);
	vTaskDelay(1);
	GPIO_ResetBits(LCD216_EN_PORT, LCD216_EN_PIN);
	vTaskDelay(1);
	lcd216_reset_data_pins();
}


//==============================================================================
void lcd216_cursor_set(int col, int row) {
	short row_offsets[] = {0x00, 0x40, 0x14, 0x54};
	
	/* Go to beginning */
	if (row >= lcd216.Rows) {
		row = 0;
	}
	
	/* Set current column and row */
	lcd216.currentX = col;
	lcd216.currentY = row;
	
	/* Set location address */
	lcd216_cmd(LCD216_SETDDRAMADDR | (col + row_offsets[row]));
}


//==============================================================================
static void lcd216_reset_ctrl_pins(void)
{
  GPIO_ResetBits(LCD216_RS_PORT, LCD216_RS_PIN);
  GPIO_ResetBits(LCD216_EN_PORT, LCD216_EN_PIN);
}


//==============================================================================
static void lcd216_reset_data_pins(void)
{
  GPIO_ResetBits(LCD216_D4_PORT, LCD216_D4_PIN);
  GPIO_ResetBits(LCD216_D5_PORT, LCD216_D5_PIN);
  GPIO_ResetBits(LCD216_D6_PORT, LCD216_D6_PIN);
  GPIO_ResetBits(LCD216_D7_PORT, LCD216_D7_PIN);
}

//==============================================================================
