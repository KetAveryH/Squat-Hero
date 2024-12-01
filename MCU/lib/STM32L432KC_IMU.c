// ]LSM6DSO32] IMU.c file
// IMU functions


#include "STM32L432KC_IMU.h"
#include "STM32L432KC_I2C.h"

// This will configure for accel data and check functionality of I2C
void IMU_config(uint8_t IMU_ADDRESS, uint8_t reg_address) { // congif accel (CTRL1_XL), read and check WHO_AM_I
  write_I2C(IMU_ADDRESS, reg_address, 1, 0);
  write_I2C(IMU_ADDRESS, 0b01001100, 1, 1); //writing this value to CTRL1_XL, which enables it and configures it to be 104kHz - +/- 16g
}

uint16_t read_accel(uint8_t IMU_ADDRESS, uint8_t reg_address) {

  // send where I want to read from x accel values
  write_I2C(IMU_ADDRESS, reg_address, 1, 0);
  char accel_data_lsb = read_I2C(IMU_ADDRESS, reg_address, 1);

  write_I2C(IMU_ADDRESS, (reg_address + 0x01), 1, 0);
  char accel_data_msb = read_I2C(IMU_ADDRESS, (reg_address + 0x01), 1);

  //combine accel data
  uint16_t raw_accel = (uint16_t)((accel_data_msb << 8) | accel_data_lsb);

  uint16_t acceleration = raw_accel;

  return acceleration;
}

