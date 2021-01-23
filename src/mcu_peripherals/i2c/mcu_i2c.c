#include "mcu_i2c_priv.h"


//--------------------------------------------------------------------------------------------------
void mcu_i2c_init(void)
{
  I2C_InitTypeDef I2C_ConfigStruct;
  I2C_ConfigStruct.I2C_ClockSpeed = 400000;                                     // 400 kHz
  I2C_ConfigStruct.I2C_Mode = I2C_Mode_I2C;
  I2C_ConfigStruct.I2C_OwnAddress1 = 0x99;
  I2C_ConfigStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
  I2C_ConfigStruct.I2C_Ack = I2C_Ack_Enable;
  I2C_ConfigStruct.I2C_DutyCycle = I2C_DutyCycle_2;

  I2C_Init(I2C1, &(I2C_ConfigStruct));

  I2C_ITConfig(I2C1, I2C_IT_BUF, ENABLE);
  I2C_ITConfig(I2C1, I2C_IT_EVT, ENABLE);
  I2C_ITConfig(I2C1, I2C_IT_ERR, ENABLE);


}
