#include "mcu_uart_priv.h"


//--------------------------------------------------------------------------------------------------
void mcu_uart_init(void)
{
  USART_InitTypeDef sUART_Init;
  DMA_InitTypeDef   sDMA_Tx_Init;

  sUART_Init.USART_BaudRate            = ESP8266_UART_BAUDRATE;
  sUART_Init.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  sUART_Init.USART_Mode                = (USART_Mode_Rx | USART_Mode_Tx);
  sUART_Init.USART_Parity              = USART_Parity_No;
  sUART_Init.USART_StopBits            = USART_StopBits_1;
  sUART_Init.USART_WordLength          = USART_WordLength_8b;

  USART_Init(ESP_UART_INTERFACE, &sUART_Init);

  //        DMA Transmit channel configuration
  //  NOTE: BufferSize is not defined at this moment
  //  NOTE: MemoryBaseAddress is not defined at this moment
  sDMA_Tx_Init.DMA_DIR                = DMA_DIR_PeripheralDST;

  sDMA_Tx_Init.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;
  sDMA_Tx_Init.DMA_MemoryInc          = DMA_MemoryInc_Enable;

  sDMA_Tx_Init.DMA_PeripheralBaseAddr = (uint32_t) &ESP_UART_INTERFACE->DR;
  sDMA_Tx_Init.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  sDMA_Tx_Init.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;

  sDMA_Tx_Init.DMA_M2M                = DMA_M2M_Disable;
  sDMA_Tx_Init.DMA_Mode               = DMA_Mode_Normal;
  sDMA_Tx_Init.DMA_Priority           = DMA_Priority_High;

  DMA_Init(ESP_UART_DMA_TX_CHANNEL, &sDMA_Tx_Init);

  USART_DMACmd(ESP_UART_INTERFACE, USART_DMAReq_Tx, ENABLE);

  USART_ITConfig(ESP_UART_INTERFACE, USART_IT_IDLE, ENABLE);
  USART_ITConfig(ESP_UART_INTERFACE, USART_IT_RXNE, ENABLE);
  USART_Cmd(ESP_UART_INTERFACE, ENABLE);
}


//--------------------------------------------------------------------------------------------------
void  mcu_uart_dma_start_transmit(uint16_t transmission_size)
{
  while(!mcu_uart_is_transmission_complete())
  {
    // wait till current transmission is incomplete
  }
  DMA_Cmd(ESP_UART_DMA_TX_CHANNEL, DISABLE);
  ESP_UART_DMA_TX_CHANNEL->CNDTR = (uint32_t) transmission_size;
  DMA_Cmd(ESP_UART_DMA_TX_CHANNEL, ENABLE);
}


//--------------------------------------------------------------------------------------------------
void mcu_uart_set_dma_buffer_address(uint32_t dma_buffer_address)
{
  ESP_UART_DMA_TX_CHANNEL->CMAR = dma_buffer_address;
}


//--------------------------------------------------------------------------------------------------
FlagStatus mcu_uart_is_transmission_complete(void)
{
  return DMA_GetFlagStatus(ESP_UART_DMA_TX_COMPLETE);
}



