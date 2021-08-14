#ifndef ADC_CONFIG_H
#define ADC_CONFIG_H

#define ADC_MAX_INPUT_VOLTAGE               3300  // mV
#define ADC_MAX_OUTPUT_RAW_VALUE            4096  // n/a

#define ANALOG_TEMP_1_CHANNEL               ADC_Channel_2
#define TEMP_1_CONVERSION_ORDER             1
#define CHANNEL_TEMP_1                      (TEMP_1_CONVERSION_ORDER - 1)

#define TEMP_2_CONVERSION_ORDER             2
#define CHANNEL_TEMP_2                      (TEMP_2_CONVERSION_ORDER - 1)
#define ANALOG_TEMP_2_CHANNEL               ADC_Channel_3

#define WATER_TANK_LEVEL_CONVERSION_ORDER   3
#define CHANNEL_WATER_TANK                  (WATER_TANK_LEVEL_CONVERSION_ORDER - 1)
#define WATER_TANK_LEVEL_CHANNEL            ADC_Channel_4

#define MEASUREMENTS_BUFFER_SIZE            8
#define WATER_TANK_LEVEL_MEAS_BUFF_SIZE     16

#define ADC_CONVERSION_CHANNELS             3

#define OPAMP_AMPLIFICATION_RATIO           3
#define LM_60_SW_AMPLIFICATION_RATIO        10
#define LM60_ZERO_DEGREES_OFFSET_CONVERTED  (424 * OPAMP_AMPLIFICATION_RATIO)
#define LM60_TEMP_SENSOR_OPAMP_MV_PER_DEG   21    // 1000 mv [sensor@100C] * 3 [opamp ratio]/ 100 C
#define RAW_ADC_TO_MV(raw)                  (float) ((raw)*ADC_MAX_INPUT_VOLTAGE/ADC_MAX_OUTPUT_RAW_VALUE)

#endif // ADC_CONFIG_H