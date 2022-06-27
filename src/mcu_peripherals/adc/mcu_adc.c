#include "stm32f10x_adc.h"

#include "mcu_adc.h"

#include "configuration/adc_config.h"


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


mcu_adc_t mcu_adc;


//--------------------------------------------------------------------------------------------------
void mcu_adc_init(void)
{
  mcu_adc_config_dma();
  mcu_adc_config();
  mcu_adc_config_conversion_order();

  ADC_Cmd(ADC1, ENABLE);
  ADC_DMACmd(ADC1, ENABLE);

  /* Enable ADC1 reset calibration register */
  ADC_ResetCalibration(ADC1);
  /* Check the end of ADC1 reset calibration register */
  while(ADC_GetResetCalibrationStatus(ADC1));

  /* Start ADC1 calibration */
  ADC_StartCalibration(ADC1);
  /* Check the end of ADC1 calibration */
  while(ADC_GetCalibrationStatus(ADC1));
}


//--------------------------------------------------------------------------------------------------
static void mcu_adc_config_dma(void)
{
  DMA_InitTypeDef DMA_InitStructure;

  /* DMA1 channel1 configuration ----------------------------------------------*/
  DMA_DeInit(DMA1_Channel1);
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t) &mcu_adc.u16_raw_data;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
  DMA_InitStructure.DMA_BufferSize = ADC_CONVERSION_CHANNELS;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  DMA_Init(DMA1_Channel1, &DMA_InitStructure);
  /* Enable DMA1 channel1 */
  DMA_Cmd(DMA1_Channel1, ENABLE);
}


//--------------------------------------------------------------------------------------------------
static void mcu_adc_config(void)
{
  ADC_InitTypeDef sADCInit;

  sADCInit.ADC_Mode               = ADC_Mode_Independent;
  sADCInit.ADC_NbrOfChannel       = ADC_CONVERSION_CHANNELS;
  sADCInit.ADC_ScanConvMode       = ENABLE;
  sADCInit.ADC_ContinuousConvMode = ENABLE;
  sADCInit.ADC_ExternalTrigConv   = ADC_ExternalTrigConv_None;
  sADCInit.ADC_DataAlign          = ADC_DataAlign_Right;

  ADC_Init(ADC1, &sADCInit);
}


//--------------------------------------------------------------------------------------------------
static void mcu_adc_config_conversion_order(void)
{
  ADC_RegularChannelConfig( ADC1,
                            ANALOG_TEMP_1_CHANNEL,
                            TEMP_1_CONVERSION_ORDER,
                            ADC_SampleTime_239Cycles5);

  ADC_RegularChannelConfig( ADC1,
                            ANALOG_TEMP_2_CHANNEL,
                            TEMP_2_CONVERSION_ORDER,
                            ADC_SampleTime_239Cycles5);

  ADC_RegularChannelConfig( ADC1,
                            WATER_TANK_LEVEL_CHANNEL,
                            WATER_TANK_LEVEL_CONVERSION_ORDER,
                            ADC_SampleTime_239Cycles5);
}


//--------------------------------------------------------------------------------------------------
void mcu_adc_start(void)
{
  ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}


//--------------------------------------------------------------------------------------------------
void mcu_adc_stop(void)
{
  ADC_SoftwareStartConvCmd(ADC1, DISABLE);
}


//--------------------------------------------------------------------------------------------------
uint16_t mcu_adc_get_raw_data_channel_temp_1(void)
{
  return mcu_adc.u16_raw_data[CHANNEL_TEMP_1];
}


//--------------------------------------------------------------------------------------------------
uint16_t mcu_adc_get_raw_data_channel_temp_2(void)
{
  return mcu_adc.u16_raw_data[CHANNEL_TEMP_2];
}


//--------------------------------------------------------------------------------------------------
uint16_t mcu_adc_get_water_tank_level_percents(void)
{
  return (mcu_adc.u16_raw_data[CHANNEL_WATER_TANK] * 100 / ADC_MAX_OUTPUT_RAW_VALUE);
}


//--------------------------------------------------------------------------------------------------
void ADC1_2_IRQHandler(void)
{
  ADC_ClearFlag(ADC1, ADC_FLAG_EOC);
  ADC_ClearITPendingBit(ADC1, ADC_IT_EOC);
}

