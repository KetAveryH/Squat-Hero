// STM32L432KC_I2C.c
// Source Code for I2C functions

#ifndef STM32L4_I2C_H
#define STM32L4_I2C_H

#include <stdint.h>
#include <stm32l432xx.h>
#include "STM32L432KC_GPIO.h"

#define SCL1 PA9
#define SDA1 PA10

// Initialize I2C
void init_I2C(void);

// Write one byte of data
void write_I2C(char addr, char reg, char data);
char read_I2C(char addr, char reg);

#endif