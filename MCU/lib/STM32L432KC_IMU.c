// ]LSM6DSO32] IMU.c file
// IMU functions


#include "STM32L432KC_IMU.h"
#include "STM32L432KC_I2C.h"

// This will configure for accel data and check functionality of I2C
// Configure the IMU - Writes to CTRL1_XL and ensures the stop bit is set after writing.
void IMU_config(uint8_t IMU_ADDRESS, uint8_t reg_address) {

    // configure accel control register with the following value for 104kHz, +/- 16g in CTRL1_XL register
    uint8_t config_value = 0b01001010;  // Value for 104kHz, +/- 16g in CTRL1_XL register
    write_I2C(IMU_ADDRESS, reg_address, config_value, 2, 0); // Write the configuration value with AUTOEND disabled
}


uint16_t read_accel(uint8_t IMU_ADDRESS, uint8_t lsb_reg_address) {

    // Step 1: Read the lower byte (LSB) from the register
    volatile uint8_t accel_data_lsb = read_I2C(IMU_ADDRESS, lsb_reg_address, 1);

    // Step 2: Read the higher byte (MSB) from the next register (e.g., OUTX_H_A)
    volatile uint8_t accel_data_msb = read_I2C(IMU_ADDRESS, (lsb_reg_address + 0x01), 1);

    // Step 3: Combine the MSB and LSB to form the 16-bit raw data
    volatile uint16_t raw_accel = (uint16_t)((accel_data_msb << 8) | accel_data_lsb);

    return raw_accel; // Return the combined raw value
}


