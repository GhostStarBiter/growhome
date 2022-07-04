#ifndef MCU_I2C_H
#define MCU_I2C_H

typedef volatile struct {
  uint8_t addr;
  uint8_t sub_addr;
  uint8_t recvd_flag;
  uint8_t *buf;
  uint8_t len;
} i2c_slave_t;


//------------------------------------------------------------------------------
/// @brief  MCU I2C module init
//------------------------------------------------------------------------------
void mcu_i2c_init(void);


//------------------------------------------------------------------------------
/// @brief  Initiate read sequence from slave specified by argument of
///         structure pointer
///
/// @return 0 - on success; NON-ZERO - on failure
//------------------------------------------------------------------------------
uint32_t mcu_i2c_transfer(i2c_slave_t* ptr);


#endif  // MCU_I2C_H
