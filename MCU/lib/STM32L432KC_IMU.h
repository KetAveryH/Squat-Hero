// LSM6DSO32 IMU

#ifndef STM32L4_IMU_H
#define STM32L4_IMU_H

#include <stdint.h> // Include stdint header
//#include "STM32L432KC_I2C.h"

///////////////////////////////////////////////////////////////////////////////
// Definitions
///////////////////////////////////////////////////////////////////////////////

#define IMU_ADDRESS_SHIN 0x6A   // [0] = 1 to read, [1] = 0 because address hardware not bridged

#define WHO_AM_I 0x0F      // used for I2C verification
#define CTRL1_XL 0x10      // 104kHz - +/- 16g
#define OUTX_L_A 0x28      // lsb x-axis accel
#define OUTX_H_A 0x29      // msb x-axis accel
#define OUTY_L_A 0x2A      // lsb y-axis accel
#define OUTY_H_A 0x2B      // msb y-axis accel
#define OUTZ_L_A 0x2C      // lsb z-axis accel
#define OUTZ_H_A 0x2D      // msb z-axis accel

///////////////////////////////////////////////////////////////////////////////
// Function prototypes
///////////////////////////////////////////////////////////////////////////////

void IMU_config(uint8_t IMU_ADDRESS, uint8_t reg_address);
uint16_t read_accel(uint8_t IMU_ADDRESS, uint8_t reg_address);

#endif