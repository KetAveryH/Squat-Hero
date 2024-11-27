#include "STM32L432KC_I2C.h"
#include "stm32l432xx.h"

// Function to recover the I2C bus if it is stuck
void recover_I2C_bus(void) {
    // Set SCL and SDA as GPIO outputs
    GPIOA->MODER &= ~(GPIO_MODER_MODE9 | GPIO_MODER_MODE10); // Clear mode
    GPIOA->MODER |= (GPIO_MODER_MODE9_0 | GPIO_MODER_MODE10_0); // Set as outputs

    for (int i = 0; i < 9; i++) {
        GPIOA->BSRR = (1 << 9); // Set SCL high
        for (volatile int j = 0; j < 1000; j++); // Short delay
        GPIOA->BRR = (1 << 9);  // Set SCL low
        for (volatile int j = 0; j < 1000; j++); // Short delay
    }

    // Generate a stop condition to release the bus
    GPIOA->BSRR = (1 << 10); // SDA high
    GPIOA->BSRR = (1 << 9);  // SCL high

    // Restore alternate function mode
    GPIOA->MODER &= ~(GPIO_MODER_MODE9 | GPIO_MODER_MODE10); // Reset GPIO
}

void init_I2C(void) {
    // Enable GPIOA clock
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;

    // Enable I2C1 clock
    RCC->APB1ENR1 |= RCC_APB1ENR1_I2C1EN;

    // Set SDA and SCL pins to alternate function mode
    GPIOA->MODER &= ~(GPIO_MODER_MODE9 | GPIO_MODER_MODE10); // Clear
    GPIOA->MODER |= (GPIO_MODER_MODE9_1 | GPIO_MODER_MODE10_1); // AF mode

    // Set alternate function to AF4 for SDA and SCL
    GPIOA->AFR[1] |= (4 << GPIO_AFRH_AFSEL9_Pos) | (4 << GPIO_AFRH_AFSEL10_Pos);

    // Open-drain configuration and high-speed GPIO
    GPIOA->OTYPER |= (GPIO_OTYPER_OT_9 | GPIO_OTYPER_OT_10);
    GPIOA->OSPEEDR |= (GPIO_OSPEEDR_OSPEED9 | GPIO_OSPEEDR_OSPEED10);

    // Reset and configure I2C1
    RCC->APB1RSTR1 |= RCC_APB1RSTR1_I2C1RST;
    RCC->APB1RSTR1 &= ~RCC_APB1RSTR1_I2C1RST;

    // Set timing register (100kHz I2C clock with 8MHz SYSCLK)
    I2C1->TIMINGR = 0x00303D5B;

    // Enable I2C1 peripheral
    I2C1->CR1 |= I2C_CR1_PE;
}

void write_I2C(int address, char reg, int num_bytes, int stop) {
    // Recover the bus if stuck
    if (I2C1->ISR & I2C_ISR_BUSY) {
        recover_I2C_bus();
    }

    // Wait until the bus is ready
    while (I2C1->ISR & I2C_ISR_BUSY);

    // Configure CR2 for write
    I2C1->CR2 = (address << 1) & ~I2C_CR2_RD_WRN; // Write mode
    I2C1->CR2 |= (num_bytes << I2C_CR2_NBYTES_Pos);

    // Auto-end condition configuration
    if (stop) {
        I2C1->CR2 |= I2C_CR2_AUTOEND;
    } else {
        I2C1->CR2 &= ~I2C_CR2_AUTOEND;
    }

    // Start condition
    I2C1->CR2 |= I2C_CR2_START;

    // Transmit register address
    while (!(I2C1->ISR & I2C_ISR_TXIS));
    I2C1->TXDR = reg;

    // Transmit data
    for (int i = 0; i < num_bytes; i++) {
        while (!(I2C1->ISR & I2C_ISR_TXIS)); // Wait until TX buffer is empty
        I2C1->TXDR = 0;                     // Replace with actual data if needed
    }

    // Wait for transfer to complete
    while (!(I2C1->ISR & I2C_ISR_TC));

    // Stop condition
    if (stop) {
        I2C1->CR2 |= I2C_CR2_STOP;
    }
}

char read_I2C(int address, char reg, int num_bytes) {
    char data = 0;

    // Recover the bus if stuck
    if (I2C1->ISR & I2C_ISR_BUSY) {
        recover_I2C_bus();
    }

    // Wait until the bus is ready
    while (I2C1->ISR & I2C_ISR_BUSY);

    // Send the register address
    I2C1->CR2 = (address << 1) & ~I2C_CR2_RD_WRN; // Write mode
    I2C1->CR2 |= (1 << I2C_CR2_NBYTES_Pos);
    I2C1->CR2 |= I2C_CR2_START;

    while (!(I2C1->ISR & I2C_ISR_TXIS));
    I2C1->TXDR = reg;

    while (!(I2C1->ISR & I2C_ISR_TC));

    // Read data
    I2C1->CR2 = (address << 1) | I2C_CR2_RD_WRN; // Read mode
    I2C1->CR2 |= (num_bytes << I2C_CR2_NBYTES_Pos);
    I2C1->CR2 |= I2C_CR2_START;

    while (!(I2C1->ISR & I2C_ISR_RXNE)); // Wait until RXNE
    data = I2C1->RXDR;

    I2C1->CR2 |= I2C_CR2_STOP; // Stop condition
    return data;
}
