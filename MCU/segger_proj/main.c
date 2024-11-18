// WRITTEN WITH CHAT

// main.c

#include "../lib/STM32L432KC_GPIO.h"
#include "../lib/STM32L432KC_I2C.h"
#include "../lib/STM32L432KC_RCC.h"
#include "../lib/STM32L432KC_FLASH.h"
#include <stdio.h>

// LSM6DSO32 I2C address and registers
#define LSM6DSO32_ADDR      0x6A  // 7-bit I2C address for LSM6DSO32 (0xD4 for write, 0xD5 for read)
#define LSM6DSO32_CTRL1_XL  0x10  // Accelerometer control register
#define LSM6DSO32_OUTX_L    0x28  // Accelerometer X-axis low byte register
#define LSM6DSO32_OUTX_H    0x29  // Accelerometer X-axis high byte register

// Function to initialize the LSM6DSO32 IMU
void init_LSM6DSO32() {
    // Set the accelerometer to a suitable range (e.g., ±2g, 16-bit data)
    // Writing to the CTRL1_XL register to configure accelerometer
    write_I2C(LSM6DSO32_ADDR, LSM6DSO32_CTRL1_XL, 0x60);  // 0x60 sets ODR = 104 Hz and scale = ±2g
}

// Function to read the X-axis accelerometer data from LSM6DSO32
int16_t readAccelX() {
    uint8_t low_byte, high_byte;
    uint16_t raw_data;

    // Read the X-axis accelerometer data (2 bytes)
    read_I2C(LSM6DSO32_ADDR, LSM6DSO32_OUTX_L, &low_byte);
    read_I2C(LSM6DSO32_ADDR, LSM6DSO32_OUTX_H, &high_byte);

    // Combine the high and low bytes into a 16-bit signed value
    raw_data = (high_byte << 8) | low_byte;

    // If the MSB (bit 15) is set, it's a negative value
    if (raw_data & 0x8000) {
        raw_data |= 0xFFFF0000;  // Sign extend to 32-bit
    }

    return (int16_t)raw_data;
}

int main(void) {
    // Step 1: System Initialization
    configureClock();  // Configure the system clock
    init_I2C();        // Initialize the I2C peripheral
    configureFlash();   // Configure flash memory access

    // Step 2: Initialize the LSM6DSO32 IMU
    init_LSM6DSO32();

    // Step 3: Read and display accelerometer data
    int16_t accel_x;
    while (1) {
        // Read X-axis data
        accel_x = readAccelX();

        // Print the X-axis accelerometer value (this assumes you have some serial or debug output setup)
        printf("Accelerometer X-axis: %d\n", accel_x);

        // Delay (for demonstration purposes, adjust as needed)
        for (volatile int i = 0; i < 1000000; i++); // Simple delay
    }

    return 0;
}

