// LSM6DSO32 IMU.c file
// IMU functions

#include "STM32L432KC_IMU.h"
#include "STM32L432KC_I2C.h"

// This will configure for accel data and check functionality of I2C
void IMU_config(void) { // congif accel (CTRL1_XL), read and check WHO_AM_I
  write_I2C(IMU_ADDRESS, ACCELX_CTRL1_REG, 1, 0);
  write_I2C(IMU_ADDRESS, 0b01001100, 1, 1);
}

float read_accel_IMU(void) {

  char accel_data_lsb;
  char accel_data_msb;

  // send where I want to read from x accel values
  write_I2C(IMU_ADDRESS, ACCELX_LSB_REG, 1, 0);
  accel_data_lsb = read_I2C(IMU_ADDRESS, ACCELX_LSB_REG, 1);

  write_I2C(IMU_ADDRESS, ACCELX_MSB_REG, 1, 0);
  accel_data_msb = read_I2C(IMU_ADDRESS, ACCELX_MSB_REG, 1);

  //combine accel data
  int16_t raw_accel = (int16_t)((accel_data_msb << 8) | accel_data_lsb);

  float acceleration = raw_accel;

  return acceleration;

}

