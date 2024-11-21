// LSM6DSO32 IMU

#ifndef STM32L4_IMU_H
#define STM32L4_IMU_H

#include <stdint.h> // Include stdint header
//#include "STM32L432KC_I2C.h"

///////////////////////////////////////////////////////////////////////////////
// Definitions
///////////////////////////////////////////////////////////////////////////////

#define IMU_ADDRESS 0x6A      // [0] = 1 to read, [1] = 0 because address hardware not bridged
#define WHO_AM_I_REG 0b00001111     // used for I2C verification
#define ACCELX_LSB_REG 0b00101000 
#define ACCELX_MSB_REG 0b00101001
#define ACCELX_CTRL1_REG 0b00010000 // 104kHz - +/- 16g

///////////////////////////////////////////////////////////////////////////////
// Function prototypes
///////////////////////////////////////////////////////////////////////////////

void IMU_config(void);
float read_accel_IMU(void);

#endif