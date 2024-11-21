// WRITTEN WITH CHAT

// main.c

//#include "../lib/STM32L432KC_GPIO.h"
#include "../lib/STM32L432KC_I2C.h"
#include "../lib/STM32L432KC_RCC.h"
#include "../lib/STM32L432KC_FLASH.h"
#include <stdio.h>

// Necessary includes for printf to work///////////////////
#include "stm32l432xx.h"

// Function used by printf to send characters to the laptop
int _write(int file, char *ptr, int len) {
  int i = 0;
  for (i = 0; i < len; i++) {
    ITM_SendChar((*ptr++));
  }
  return len;
}
////////////////////////////////////////////////////////////

// LSM6DSO32 I2C address and registers
#define LSM6DSO32_ADDR      0x6A  // 7-bit I2C address for LSM6DSO32 (0xD4 for write, 0xD5 for read)
#define LSM6DSO32_CTRL1_XL  0x10  // Accelerometer control register
#define LSM6DSO32_OUTX_L    0x28  // Accelerometer X-axis low byte register
#define LSM6DSO32_OUTX_H    0x29  // Accelerometer X-axis high byte register

// Function to initialize the LSM6DSO32 IMU
void init_LSM6DSO32(void) {
    // Set the accelerometer to a suitable range (e.g., ±2g, 16-bit data)
    // Writing to the CTRL1_XL register to configure accelerometer
    write_I2C(LSM6DSO32_ADDR, LSM6DSO32_CTRL1_XL, 0b01000100);  // can be 0b01000110 to apply low pass filter
}

// Function to read the X-axis accelerometer data from LSM6DSO32
int16_t readAccelX(void) {
    uint8_t low_byte, high_byte;
    int16_t raw_data;

    // Read the low byte from the sensor
    low_byte = read_I2C(LSM6DSO32_ADDR, LSM6DSO32_OUTX_L);

    // Read the high byte from the sensor
    high_byte = read_I2C(LSM6DSO32_ADDR, LSM6DSO32_OUTX_H);

    // Combine the low and high byte into a 16-bit signed value
    raw_data = (high_byte << 8) | low_byte;  // Correct combination

    // Return the signed 16-bit value
    return raw_data;
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

