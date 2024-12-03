// STM32L432KC_I2C.c
// Source Code for I2C functions

#ifndef STM32L4_I2C_H
#define STM32L4_I2C_H

//#include <stdint.h>
#include <stm32l432xx.h>
#include "STM32L432KC_GPIO.h"

// for I2C1 peripheral
#define SCL1 PA9
#define SDA1 PA10

// for I2C3 peripheral
#define SCL2 PA7
#define SDA2 PB4

// Initialize I2C
void init_I2C1(void);
void init_I2C3(void);

// Write one byte of data
void write_I2C1(int address, uint8_t reg, uint8_t data, int num_bytes, int stop);
char read_I2C1(int address, char reg, int num_bytes);

void write_I2C3(int address, uint8_t reg, uint8_t data, int num_bytes, int stop);
char read_I2C3(int address, char reg, int num_bytes);

#endif