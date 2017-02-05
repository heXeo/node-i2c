#ifndef _I2C_H_
#define _I2C_H_

  #include <stdint.h>

  int32_t i2c_open(const char* device, uint8_t address);
  int32_t i2c_close(int32_t fd);
  int32_t i2c_read(int32_t fd, uint8_t* out);
  int32_t i2c_read_reg8(int32_t fd, uint8_t reg, uint8_t* out);
  int32_t i2c_read_reg16(int32_t fd, uint8_t reg, uint16_t* out);
  int32_t i2c_write(int32_t fd, uint8_t in);
  int32_t i2c_write_reg8(int32_t fd, uint8_t reg, uint8_t in);
  int32_t i2c_write_reg16(int32_t fd, uint8_t reg, uint16_t in);

#endif
