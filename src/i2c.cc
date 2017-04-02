#include <fcntl.h> // open
#include <unistd.h> // close
#include <sys/ioctl.h>
#include <asm/ioctl.h>

#include "i2c.h"

#define I2C_SLAVE 0x0703
#define I2C_SMBUS 0x0720
#define I2C_SMBUS_READ 1
#define I2C_SMBUS_WRITE 0
#define I2C_SMBUS_QUICK 0
#define I2C_SMBUS_BYTE 1
#define I2C_SMBUS_BYTE_DATA 2
#define I2C_SMBUS_WORD_DATA 3
#define I2C_SMBUS_PROC_CALL 4
#define I2C_SMBUS_BLOCK_DATA 5
#define I2C_SMBUS_I2C_BLOCK_BROKEN 6
#define I2C_SMBUS_BLOCK_PROC_CALL 7
#define I2C_SMBUS_I2C_BLOCK_DATA 8
#define I2C_SMBUS_BLOCK_MAX 32
#define I2C_SMBUS_I2C_BLOCK_MAX 32

union i2c_smbus_data {
  uint8_t byte;
  uint16_t word;
  // block [0] is used for length + one more for PEC
  uint8_t block[I2C_SMBUS_BLOCK_MAX + 2];
};

struct i2c_smbus_ioctl_data {
  uint8_t read_write;
  uint8_t command;
  uint32_t size;
  union i2c_smbus_data* data;
};

static inline int i2c_smbus_access(
  int32_t fd, int8_t rw, uint8_t command,
  uint32_t size, union i2c_smbus_data* data) {

  struct i2c_smbus_ioctl_data args;

  args.read_write = rw;
  args.command    = command;
  args.size       = size;
  args.data       = data;

  return ioctl(fd, I2C_SMBUS, &args);
}

int32_t i2c_open(const char* device, uint8_t address) {
  int32_t fd;

  if ((fd = open(device, O_RDWR)) < 0) {
    return -1;
  }

  if (ioctl(fd, I2C_SLAVE, address) < 0) {
    return -1;
  }

  return fd;
}

int32_t i2c_close(int32_t fd) {
  return close(fd);
}

int32_t i2c_read(int32_t fd, uint8_t* out) {
  union i2c_smbus_data data;

  if (i2c_smbus_access(fd, I2C_SMBUS_READ, 0, I2C_SMBUS_BYTE, &data) < 0) {
    return -1;
  }

  *out = data.byte & 0xFF;

  return 0;
}

int32_t i2c_read_reg8(int32_t fd, uint8_t reg, uint8_t* out) {
  union i2c_smbus_data data;

  if (i2c_smbus_access(fd, I2C_SMBUS_READ, reg, I2C_SMBUS_BYTE_DATA, &data) < 0) {
    return -1;
  }

  *out = data.byte & 0xFF;

  return 0;
}

int32_t i2c_read_reg16(int32_t fd, uint8_t reg, uint16_t* out) {
  union i2c_smbus_data data;

  if (i2c_smbus_access(fd, I2C_SMBUS_READ, reg, I2C_SMBUS_WORD_DATA, &data) < 0) {
    return -1;
  }

  *out = data.word & 0xFFFF;

  return 0;
}

int32_t i2c_write(int32_t fd, uint8_t in) {
  return i2c_smbus_access(fd, I2C_SMBUS_WRITE, in, I2C_SMBUS_BYTE, NULL);
}

int32_t i2c_write_reg8(int32_t fd, uint8_t reg, uint8_t in) {
  union i2c_smbus_data data;

  data.byte = in;

  return i2c_smbus_access(fd, I2C_SMBUS_WRITE, reg, I2C_SMBUS_BYTE_DATA, &data);
}

int32_t i2c_write_reg16(int32_t fd, uint8_t reg, uint16_t in) {
  union i2c_smbus_data data;

  data.word = in;

  return i2c_smbus_access(fd, I2C_SMBUS_WRITE, reg, I2C_SMBUS_WORD_DATA, &data);
}
