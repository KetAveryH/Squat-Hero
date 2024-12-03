// ]LSM6DSO32] IMU.c file
// IMU functions


#include "STM32L432KC_IMU.h"
#include "STM32L432KC_I2C.h"

/********************************************************************************
 *                             I2C1 Functions                                   *
 *      IMU_config_I2C1: configure accel control reg to desired function        *
 *  read_accel_I2C1: read x,y,z accel given lsb register of respective x,y,z    *
 ********************************************************************************/
void IMU_config_I2C1(uint8_t IMU_ADDRESS, uint8_t reg_address) {

    // configure accel control register with the following value for 104kHz, +/- 16g in CTRL1_XL register
    uint8_t config_value = 0b01001010;  // Value for 104kHz, +/- 16g in CTRL1_XL register
    write_I2C1(IMU_ADDRESS, reg_address, config_value, 2, 0); // Write the configuration value with AUTOEND disabled
}

uint16_t read_accel_I2C1(uint8_t IMU_ADDRESS, uint8_t lsb_reg_address) {

    // Step 1: Read the lower byte (LSB) from the register
    volatile uint8_t accel_data_lsb = read_I2C1(IMU_ADDRESS, lsb_reg_address, 1);

    // Step 2: Read the higher byte (MSB) from the next register (e.g., OUTX_H_A)
    volatile uint8_t accel_data_msb = read_I2C1(IMU_ADDRESS, (lsb_reg_address + 0x01), 1);

    // Step 3: Combine the MSB and LSB to form the 16-bit raw data
    volatile uint16_t raw_accel = (uint16_t)((accel_data_msb << 8) | accel_data_lsb);

    return raw_accel; // Return the combined raw value
}

/********************************************************************************
 *                             I2C3 Functions                                   *
 *      IMU_config_I2C3: configure accel control reg to desired function        *
 *  read_accel_I2C3: read x,y,z accel given lsb register of respective x,y,z    *
 ********************************************************************************/

void IMU_config_I2C3(uint8_t IMU_ADDRESS, uint8_t reg_address) {

    // configure accel control register with the following value for 104kHz, +/- 16g in CTRL1_XL register
    uint8_t config_value = 0b01001010;  // Value for 104kHz, +/- 16g in CTRL1_XL register
    write_I2C3(IMU_ADDRESS, reg_address, config_value, 2, 0); // Write the configuration value with AUTOEND disabled
}

uint16_t read_accel_I2C3(uint8_t IMU_ADDRESS, uint8_t lsb_reg_address) {

    // Step 1: Read the lower byte (LSB) from the register
    volatile uint8_t accel_data_lsb = read_I2C3(IMU_ADDRESS, lsb_reg_address, 1);

    // Step 2: Read the higher byte (MSB) from the next register (e.g., OUTX_H_A)
    volatile uint8_t accel_data_msb = read_I2C3(IMU_ADDRESS, (lsb_reg_address + 0x01), 1);

    // Step 3: Combine the MSB and LSB to form the 16-bit raw data
    volatile uint16_t raw_accel = (uint16_t)((accel_data_msb << 8) | accel_data_lsb);

    return raw_accel; // Return the combined raw value
}


