// STM32L432KC_I2C.c
// Source Code for I2C functions

#ifndef STM32L4_I2C_H
#define STM32L4_I2C_H

#include <stdint.h>
#include <stm32l432xx.h>
#include "STM32L432KC_GPIO.h"

#define I2C_SCL PA9
#define I2C_SDA PA10

// Initialize I2C
void init_I2C();

// Write one byte of data
void write_I2C(char addr, char reg, char data);

#endif