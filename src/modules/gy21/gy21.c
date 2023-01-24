#include "i2c/mcu_i2c.h"

#include "gy21.h"

#define GY21_READOUT_PERIOD   1000  // [ms] period between temp and humidity readout
#define GY21_DATA_SIZE_BYTES  2     // both temp and humidity are 16-bit values

typedef enum {
  READ_TEMP = GY21_READ_TEMP,
  READ_HUMIDITY = GY21_READ_HUMIDITY
} gy21_measure_t;

typedef volatile struct {
  i2c_slave_t         i2c;
  uint16_t            data_buf[2];
  uint32_t            period;
  gy21_measure_t      curr_measure;

  double        temperature;
  double        humidity;
} gy21_sensor_t;

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
static void gy21_calculate_temp(void);


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
static void gy21_calculate_humidity(void);


gy21_sensor_t gy21;


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
uint32_t gy21_sensor_init(void)
{
  gy21.i2c.addr       = GY21_BUS_ADDRESS;
  gy21.i2c.len        = GY21_DATA_SIZE_BYTES;
  gy21.i2c.buf        = NULL;                 // will be filled later
  gy21.i2c.sub_addr   = 0;                    // will be filled later

  gy21.period = GY21_READOUT_PERIOD + 1;

  gy21.i2c.recvd_flag = 0;
  gy21.curr_measure       = READ_TEMP;

  return 0;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void gy21_update(void)
{
  uint8_t meas_data_index = 0;

  if(gy21.period > GY21_READOUT_PERIOD)
  {
    switch(gy21.curr_measure)
    {
    case READ_TEMP:
      gy21.i2c.sub_addr = READ_TEMP;
      meas_data_index = TEMP_INDEX;
      break;

    case READ_HUMIDITY:
      gy21.i2c.sub_addr = READ_HUMIDITY;
      meas_data_index = HUMIDITY_INDEX;
      break;

    default:
      break;
    }

    gy21.i2c.buf = (uint8_t*) &gy21.data_buf[meas_data_index];
    gy21.i2c.recvd_flag = 0;    // changed in I2C DMA RX interrupt

    mcu_i2c_transfer(&gy21.i2c);

    gy21.period = 0;
  }
  else
  {
    gy21.period++;
  }

  if(gy21.i2c.recvd_flag)
  {
    switch(gy21.curr_measure)
    {
    case READ_TEMP:
      gy21_calculate_temp();
      gy21.curr_measure = READ_HUMIDITY;
      break;

    case READ_HUMIDITY:
      gy21_calculate_humidity();
      gy21.curr_measure = READ_TEMP;
      break;
    }

    gy21.i2c.recvd_flag = 0;
  }
}


//------------------------------------------------------------------------------
static void gy21_calculate_temp(void)
{
  uint16_t measured = gy21.data_buf[TEMP_INDEX];
  uint16_t tmp = 0;

  tmp = (measured >> 8) & 0xFF;
  measured = (measured << 8) & 0xFF00;
  measured |= tmp;

  gy21.temperature = (175.72*measured)/65536 - 46.85;
}


//------------------------------------------------------------------------------
static void gy21_calculate_humidity(void)
{
  uint16_t measured = gy21.data_buf[HUMIDITY_INDEX];
  uint16_t tmp = 0;

  tmp = (measured >> 8) & 0xFF;
  measured = (measured << 8) & 0xFF00;
  measured |= tmp;

  gy21.humidity = (125*measured)/65536 - 6;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
double gy21_get_temperature(void)
{
  return gy21.temperature;
};

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
double gy21_get_humidity(void)
{
  return gy21.humidity;
};

