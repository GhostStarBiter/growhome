#ifndef MCU_SPI_H
#define MCU_SPI_H

void mcu_spi_init(void);

void mcu_spi_enable_communication(void);

void mcu_spi_dma_send_data(void);

void mcu_spi_set_dac_channel_data(uint8_t channel, uint16_t data);

#endif // MCU_SPI_H
