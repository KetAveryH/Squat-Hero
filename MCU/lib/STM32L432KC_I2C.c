// STM32L432KC_I2C.h
// Source code for I2C functions COPIED FROM TROY

#include "STM32L432KC.h"
#include "STM32L432KC_I2C.h"
#include "STM32L432KC_GPIO.h"
#include "STM32L432KC_RCC.h"

void init_I2C() {

    // Specify that the pclk will represent the I2C_CLK
    RCC->CCIPR |= _VAL2FLD(RCC_CCIPR_I2C1SEL, 0b00);

    // Set pclk to 8Mhz by settting the prescalar to 8 (sysclk is 64 Mhz)
    RCC->CFGR |= _VAL2FLD(RCC_CFGR_PPRE1, 0b110);

    // Enable the I2C CLOCK
    RCC->APB1ENR1 |= _VAL2FLD(RCC_APB1ENR1_I2C1EN, 1);

    // Enable the analog filter
    I2C1->CR1 &= ~I2C_CR1_ANFOFF;

    // Disable digital filter
    I2C1->CR1 &= ~I2C_CR1_DNF;

    // Disable clock stretching
    I2C1->CR1 &= ~I2C_CR1_NOSTRETCH;

    // Clear the ACK bit in the CR2 register to ensure ACK is sent by default
    I2C1->CR2 &= ~I2C_CR2_NACK; 

    // Set up autoend
    I2C1->CR2 |= I2C_CR2_AUTOEND;

    // Enable the TXIS interrupt
    I2C1->CR1 |= I2C_ISR_TXIS;

    // Reset I2C Peripheral
    I2C1->CR1 &= ~I2C_CR1_PE; // Disable I2C1
    I2C1->CR1 |= I2C_CR1_SWRST; // Reset I2C
    I2C1->CR1 &= ~I2C_CR1_SWRST; // Clear reset

    // Select Open Drain Output ... USE EXTERNAL PULL UPS (4.7k ~ 10K)
    GPIOA->OTYPER |= (GPIO_OTYPER_OT9);
    GPIOA->OTYPER |= (GPIO_OTYPER_OT10);

    // Configure the GPIO pins in ALT mode
    gpioEnable(GPIO_PORT_A);
    pinMode(I2C_SCL, GPIO_ALT); 
    pinMode(I2C_SDA, GPIO_ALT); 

    // Set up I2C ALT pin mode 
    GPIOA->AFR[1] |= _VAL2FLD(GPIO_AFRH_AFSEL10, 4);
    GPIOA->AFR[1] |= _VAL2FLD(GPIO_AFRH_AFSEL9, 4);

    //Program the peripheral input clock in I2C_CR2 Register in order to generate correct timings
    I2C1->TIMINGR |= _VAL2FLD(I2C_TIMINGR_PRESC, 1);
    I2C1->TIMINGR |= _VAL2FLD(I2C_TIMINGR_SCLDEL, 0x4);
    I2C1->TIMINGR |= _VAL2FLD(I2C_TIMINGR_SDADEL, 0x2);
    I2C1->TIMINGR |= _VAL2FLD(I2C_TIMINGR_SCLH, 0xF);
    I2C1->TIMINGR |= _VAL2FLD(I2C_TIMINGR_SCLL, 0x13);

    // Enable the I2C peripheral
    I2C1->CR1 |= I2C_CR1_PE;
    
}

void write_I2C(char addr, char reg, char data){
  
    // Clear CR2 settings from previous transactions
    I2C1->CR2 &= ~(I2C_CR2_SADD | I2C_CR2_NBYTES | I2C_CR2_RD_WRN);

    // Master operates in 7-bit addressing mode
    I2C1->CR2 &= ~I2C_CR2_ADD10;

    // Slave address for writing
    I2C1->CR2 |= _VAL2FLD(I2C_CR2_SADD, (addr << 1));

    // Master requests a write transfer
    I2C1->CR2 &= ~I2C_CR2_RD_WRN;

    // Number of bytes that will be sent
    I2C1->CR2 |= _VAL2FLD(I2C_CR2_NBYTES, 2);

    // Start I2C Master Write after tx buffer is cleared
    I2C1->CR2 |= I2C_CR2_START;  

    // Write the register's data address to be configured
    while (!(I2C1->ISR & I2C_ISR_TXIS));
        *(volatile char *) (&I2C1->TXDR) = reg;

    // Write a data value to the resister
    while (!(I2C1->ISR & I2C_ISR_TXIS));
        *(volatile char *) (&I2C1->TXDR) = data;

    //autoend will stop the process

}

// I2C read function WRITTEN WITH CHAT
void read_I2C(char addr, char reg, char *data) {
    // Clear previous settings in CR2
    I2C1->CR2 &= ~(I2C_CR2_SADD | I2C_CR2_NBYTES | I2C_CR2_RD_WRN);

    // Master operates in 7-bit addressing mode
    I2C1->CR2 &= ~I2C_CR2_ADD10;

    // Set the slave address for reading (shifted left by 1 for 7-bit addressing)
    I2C1->CR2 |= _VAL2FLD(I2C_CR2_SADD, (addr << 1));

    // Master requests a read transfer
    I2C1->CR2 |= I2C_CR2_RD_WRN;  // Set to read operation

    // Number of bytes to read (1 byte in this case)
    I2C1->CR2 |= _VAL2FLD(I2C_CR2_NBYTES, 1);

    // Start I2C read after clearing the TX buffer
    I2C1->CR2 |= I2C_CR2_START;

    // Send the register address to be read from
    while (!(I2C1->ISR & I2C_ISR_TXIS));  // Wait for TX buffer to be empty
    *(volatile char *)(&I2C1->TXDR) = reg;  // Write the register address

    // Wait until the start condition has been processed and the address has been sent
    while (!(I2C1->ISR & I2C_ISR_TC));  // Wait for transfer complete (TC flag)

    // Generate a repeated start for reading
    I2C1->CR2 |= I2C_CR2_START;

    // Wait for the data to be received (RXNE flag)
    while (!(I2C1->ISR & I2C_ISR_RXNE));  // Wait until RX buffer is not empty

    // Read the data from RXDR
    *data = *(volatile char *)(&I2C1->RXDR);

    // Wait for the transfer to complete (TC flag)
    while (!(I2C1->ISR & I2C_ISR_TC));  // Wait for transfer complete (TC flag)

    // End the I2C transfer (autoend mode)
    I2C1->CR2 |= I2C_CR2_STOP;  // Generate stop condition
}