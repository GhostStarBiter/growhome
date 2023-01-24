#include "mcu_spi.h"
#include "configuration/peripherals_config.h"
#include "mcu_peripherals/gpio/mcu_gpio.h"

#include "stm32f10x.h"

#define DAC_DMA_TX_BUF_SIZE 16

volatile uint8_t dac_tx_buf[DAC_DMA_TX_BUF_SIZE]; // 8 channels X 2 bytes per channel

void mcu_spi_init(void)
{
  SPI_InitTypeDef spi_init;
  DMA_InitTypeDef dma_spi_tx;

  dma_spi_tx.DMA_Mode = DMA_Mode_Normal;
  dma_spi_tx.DMA_PeripheralBaseAddr = (uint32_t) &(SPI_INTERFACE->DR);
  dma_spi_tx.DMA_MemoryBaseAddr = (uint32_t) &dac_tx_buf[0];
  dma_spi_tx.DMA_DIR = DMA_DIR_PeripheralDST;
  dma_spi_tx.DMA_BufferSize = DAC_DMA_TX_BUF_SIZE;
  dma_spi_tx.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  dma_spi_tx.DMA_MemoryInc = DMA_MemoryInc_Enable;
  dma_spi_tx.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  dma_spi_tx.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  dma_spi_tx.DMA_Priority = DMA_Priority_Low;
  dma_spi_tx.DMA_M2M = DMA_M2M_Disable;

  DMA_Init(DMA1_Channel5, &dma_spi_tx);

  spi_init.SPI_Direction = SPI_Direction_1Line_Tx;
  spi_init.SPI_Mode = SPI_Mode_Master;
  spi_init.SPI_DataSize = SPI_DataSize_16b;
  spi_init.SPI_CPOL = SPI_CPOL_High;
  spi_init.SPI_CPHA = SPI_CPHA_1Edge;
  spi_init.SPI_NSS = SPI_NSS_Hard;
  spi_init.SPI_FirstBit = SPI_FirstBit_MSB;
  spi_init.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
  spi_init.SPI_CRCPolynomial = 7;

  SPI_Init(SPI_INTERFACE, &spi_init);

  SPI_Cmd(SPI_INTERFACE, ENABLE);
  SPI_SSOutputCmd(SPI_INTERFACE, ENABLE);

  SPI_I2S_SendData(SPI_INTERFACE, 0x9000);

  while(SPI_I2S_GetFlagStatus(SPI_INTERFACE, SPI_I2S_FLAG_TXE) == RESET);
  while(SPI_I2S_GetFlagStatus(SPI_INTERFACE, SPI_I2S_FLAG_BSY) == SET);

  SPI_Cmd(SPI_INTERFACE, DISABLE);
}

//void mcu_spi_enable_communication(void)
//{
//  SPI_I2S_ITConfig(SPI_INTERFACE, SPI_I2S_IT_TXE, ENABLE);
//  SPI_I2S_DMACmd(SPI_INTERFACE, SPI_I2S_DMAReq_Tx, ENABLE);
//
//  DMA_SetCurrDataCounter(DMA1_Channel5, DAC_DMA_TX_BUF_SIZE);
//  DMA_ITConfig(DMA1_Channel5, DMA_IT_TC, ENABLE);
//  DMA_Cmd(DMA1_Channel5, ENABLE);
//  SPI_Cmd(SPI_INTERFACE, ENABLE);
//}


void mcu_spi_set_dac_channel_data(uint8_t channel, uint16_t data)
{
  dac_tx_buf[(channel<<1)] = (uint8_t) (data & 0x00FF);
  dac_tx_buf[(channel<<1) + 1] = (uint8_t) ((channel & 0x07) << 4) | ((data & 0xFF00) >> 8);
}

void mcu_spi_dma_send_data(void)
{
    for(uint8_t i = 0; i < 8; i++)
    {
      SPI_Cmd(SPI_INTERFACE, ENABLE);
      SPI_I2S_SendData(SPI_INTERFACE, (dac_tx_buf[i]) | (dac_tx_buf[i+1] << 8));
      //while(SPI_I2S_GetFlagStatus(SPI_INTERFACE, SPI_I2S_FLAG_TXE) == RESET);
      while(SPI_I2S_GetFlagStatus(SPI_INTERFACE, SPI_I2S_FLAG_BSY) == SET);

      SPI_Cmd(SPI_INTERFACE, DISABLE);

      for(uint16_t k = 0; k < 100; k++)
      {};
    }

  //DMA_SetCurrDataCounter(DMA1_Channel5, DAC_DMA_TX_BUF_SIZE);
//  SPI_Cmd(SPI_INTERFACE, ENABLE);
//  SPI_I2S_DMACmd(SPI_INTERFACE, SPI_I2S_DMAReq_Tx, ENABLE);
//  DMA_Cmd(DMA1_Channel5, ENABLE);
}


void SPI2_IRQHandler(void)
{
  if (SPI_I2S_GetITStatus(SPI_INTERFACE, SPI_I2S_IT_TXE) == SET)
  {
    SPI_I2S_ClearITPendingBit(SPI_INTERFACE, SPI_I2S_IT_TXE);
//    while(SPI_I2S_GetFlagStatus(SPI_INTERFACE, SPI_I2S_FLAG_BSY) == SET);
//
//    SPI_Cmd(SPI_INTERFACE, DISABLE);
//    DMA_Cmd(DMA1_Channel5, DISABLE);
  }
}

void DMA1_Channel5_IRQHandler(void)
{
  if (DMA_GetITStatus(DMA1_FLAG_TC5))
  {
    DMA_ClearITPendingBit(DMA1_FLAG_TC5);
    //DMA_SetCurrDataCounter(DMA1_Channel5, DAC_DMA_TX_BUF_SIZE);
  }
}

