// WRITTEN WITH CHAT

// main.c

//#include "../lib/STM32L432KC_GPIO.h"
#include "../lib/STM32L432KC_I2C.h"
#include "../lib/STM32L432KC_RCC.h"
#include "../lib/STM32L432KC_FLASH.h"
#include "../lib/STM32L432KC_IMU.h"
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


int main(void) {
    // Step 1: System Initialization
    configureFlash();   // Configure flash memory access
    configureClock();  // Configure the system clock
    init_I2C();        // Initialize the I2C peripheral

    // Step 2: Initialize the LSM6DSO32 IMU
    IMU_config();

    // Step 3: Read and display accelerometer data
    while (1) {
        // Read X-axis data
        int acceleration = read_accel_IMU();

        // Print the X-axis accelerometer value (this assumes you have some serial or debug output setup)
        printf("Accelerometer X-axis: %d\n", acceleration);

        // Delay (for demonstration purposes, adjust as needed)
        for (volatile int i = 0; i < 100; i++); // Simple delay
    }

    return 0;
}

