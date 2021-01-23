#ifndef MCU_ADC_PRIV_H
#define MCU_ADC_PRIV_H

#include "stm32f10x_adc.h"

#include "mcu_adc.h"

#include "main/application_map.h"


typedef struct{
    volatile uint16_t  u16_raw_data[ADC_CONVERSION_CHANNELS];
} mcu_adc_t;


//--------------------------------------------------------------------------------------------------
/// @brief  Configuration of DMA channel for ADC module
//--------------------------------------------------------------------------------------------------
static void mcu_adc_config_dma(void);


//--------------------------------------------------------------------------------------------------
/// @brief  Configuration of ADC module
//--------------------------------------------------------------------------------------------------
static void mcu_adc_config(void);


//--------------------------------------------------------------------------------------------------
/// @brief  Configuration of ADC conversion sequence
//--------------------------------------------------------------------------------------------------
static void mcu_adc_config_conversion_order(void);

#endif  // MCU_ADC_PRIV_H
