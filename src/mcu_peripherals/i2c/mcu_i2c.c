#include "stm32f10x_i2c.h"
#include "configuration/peripherals_config.h"
#include "mcu_i2c.h"

#include "gpio/mcu_gpio.h"

typedef volatile struct {
  i2c_slave_t* slave;
  uint8_t busy;
} mcu_i2c_manager_t;

mcu_i2c_manager_t mcu_i2c;


//--------------------------------------------------------------------------------------------------
void mcu_i2c_init(void)
{
  I2C_InitTypeDef I2C_ConfigStruct;
  DMA_InitTypeDef DMA_InitStructure;

  mcu_i2c.slave = NULL;
  mcu_i2c.busy = 0;

  I2C_DeInit(I2C1);
  DMA_DeInit(I2C1_DMA_Channel);

  I2C_ConfigStruct.I2C_ClockSpeed = 400000;                                     // 400 kHz
  I2C_ConfigStruct.I2C_Mode = I2C_Mode_I2C;
  I2C_ConfigStruct.I2C_OwnAddress1 = 0x77;
  I2C_ConfigStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
  I2C_ConfigStruct.I2C_Ack = I2C_Ack_Enable;
  I2C_ConfigStruct.I2C_DutyCycle = I2C_DutyCycle_2;

  I2C_Init(I2C1, &(I2C_ConfigStruct));

  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) &I2C1->DR;
  //DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)I2C_Rx_Buffer; //variable to store data  ||| SET LATER
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
  // DMA_InitStructure.DMA_BufferSize = 1;    //number of data to be transfered  ||| SET LATER
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;

  DMA_Init(I2C1_DMA_Channel, &DMA_InitStructure);
  DMA_ITConfig(I2C1_DMA_Channel, DMA_IT_TC, ENABLE);

  I2C_DMACmd(I2C1, ENABLE);
  DMA_Cmd(I2C1_DMA_Channel, ENABLE);
  I2C_Cmd(I2C1, ENABLE);
}


//------------------------------------------------------------------------------
uint32_t mcu_i2c_transfer(i2c_slave_t* ptr)
{
  if(mcu_i2c.busy)
  {
    return I2C_FLAG_BUSY;
  }

  while(!ptr);

  mcu_i2c.busy = 1;
  mcu_i2c.slave = ptr;

  // Data length set to 0 if we want to send data to sensor without reading data back
  if(mcu_i2c.slave->len != 0)
  {
    DMA_Cmd(I2C1_DMA_Channel, DISABLE);
    // set DMA channel receiving buffer address
    I2C1_DMA_Channel->CMAR = (uint32_t) (mcu_i2c.slave->buf);
    // set size in bytes
    I2C1_DMA_Channel->CNDTR = mcu_i2c.slave->len;
    DMA_Cmd(I2C1_DMA_Channel, ENABLE);
  }

  /* While the bus is busy */
  while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));

  /* Send START condition */
  I2C_GenerateSTART(I2C1, ENABLE);

  /* Test on EV5 and clear it */
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));

  /* Send MPU6050 address for write */
  I2C_Send7bitAddress(I2C1, (mcu_i2c.slave->addr << 1), I2C_Direction_Transmitter);

  /* Test on EV6 and clear it */
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

  /* Clear EV6 by setting again the PE bit */
  I2C_Cmd(I2C1, ENABLE);

  /* Send the internal address (IC specific) */
  I2C_SendData(I2C1, mcu_i2c.slave->sub_addr);

  /* Test on EV8 and clear it */
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

  if(mcu_i2c.slave->len != 0)
  {
    /* Send START condition a second time */
    I2C_GenerateSTART(I2C1, ENABLE);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));

    /* Enable DMA NACK automatic generation */
    I2C_DMALastTransferCmd(I2C1, ENABLE);                    //Note this one, very important

    I2C_Send7bitAddress(I2C1, (mcu_i2c.slave->addr << 1), I2C_Direction_Receiver);

    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
  }

  return 0;
}


//------------------------------------------------------------------------------
void DMA1_Channel7_IRQHandler(void)
{
  if (DMA_GetFlagStatus(DMA1_FLAG_TC7))
  {
    //mcu_gpio_set_debug_pin(ENABLE);
    /* Clear transmission complete flag */
    DMA_ClearFlag(DMA1_FLAG_TC7);

    /* Send I2C1 STOP Condition */
    I2C_GenerateSTOP(I2C1, ENABLE);
    /* Disable DMA channel*/
    DMA_Cmd(I2C1_DMA_Channel, DISABLE);

    mcu_i2c.busy = 0;
    mcu_i2c.slave->recvd_flag = 1;

    //mcu_gpio_set_debug_pin(DISABLE);
  }
}
