// STM32L432KC_I2C.h
// Takes care of initialization, read, and write functions for I2C

#include "STM32L432KC_I2C.h"
#include "STM32L432KC_GPIO.h"
#include "stm32l432xx.h"
#include "STM32L432KC_IMU.h"

/********************************************************************************
 *                             I2C1 Functions                                   *
 *  init_I2C1: initializes I2C1 peripheral and required GPIO pins (PA9 / PA10)  *
 *  write_I2C1: writes data to a addressed chip, and sub reg, with given data   *
 *       read_I2C1: reads data from requested sub reg in addressed chip         *
 ********************************************************************************/
void init_I2C1(void) {
  // initialize for I2C to work on 'B' I/Os
  gpioEnable(GPIO_PORT_A);
  RCC->APB1ENR1 |= RCC_APB1ENR1_I2C1EN; //enable the clock to run on I2C1 peripheral

  // set both of the I2C lines to be the correct alternate function mode
  pinMode(SDA1, GPIO_ALT);
  pinMode(SCL1, GPIO_ALT);

  // select high speed GPIO
  GPIOA->OSPEEDR |= (GPIO_OSPEEDR_OSPEED9 | GPIO_OSPEEDR_OSPEED10);
  // select for open drain I/O and
  GPIOA->OTYPER |= (GPIO_OTYPER_IDR_9 | GPIO_OTYPER_IDR_10);

  // set to I2C specific alternate functionality AF04
  GPIOA->AFR[1] |= _VAL2FLD(GPIO_AFRL_AFSEL1, 4);
  GPIOA->AFR[1] |= _VAL2FLD(GPIO_AFRL_AFSEL2, 4);
 
  //set I2C clk as the SYSCLK
  RCC->CCIPR &= ~RCC_CCIPR_I2C1SEL;
  RCC->CCIPR |= _VAL2FLD(RCC_CCIPR_I2C1SEL, 0b01);

  // clear PE bit
  I2C1->CR1 &= ~I2C_CR1_PE;

  // turn on TX interrupts
  I2C1->CR1 |= I2C_CR1_TXIE;
  I2C1->CR1 |= I2C_CR1_TCIE;

  // enable byte control
  I2C1->CR1 |= I2C_CR1_SBC;

  // Set the I2C Timing register for a 100kHz I2C clock with a 8 MHz system clock (LOOK AT PAGE 1159) in reference manual  
  I2C1->TIMINGR &= ~I2C_TIMINGR_PRESC;     // Clear the PRESC field
  I2C1->TIMINGR |= (1 << I2C_TIMINGR_PRESC_Pos); // Set PRESC

  I2C1->TIMINGR &= ~I2C_TIMINGR_SCLL;      // Clear SCL low period
  I2C1->TIMINGR |= (0x13 << I2C_TIMINGR_SCLL_Pos); // Set SCLL 

  I2C1->TIMINGR &= ~I2C_TIMINGR_SCLH;      // Clear SCL high period
  I2C1->TIMINGR |= (0xF << I2C_TIMINGR_SCLH_Pos); // Set SCLH

  I2C1->TIMINGR &= ~I2C_TIMINGR_SDADEL;      // Clear SCL high period
  I2C1->TIMINGR |= (0x2 << I2C_TIMINGR_SDADEL_Pos); // Set SCLH

  I2C1->TIMINGR &= ~I2C_TIMINGR_SCLDEL;      // Clear SCL high period
  I2C1->TIMINGR |= (0x4 << I2C_TIMINGR_SCLDEL_Pos); // Set SCLH

  I2C1->CR1 |= I2C_CR1_PE;  // Generate START

  volatile uint8_t who_am_i_value = read_I2C1(IMU_ADDRESS_SHIN, WHO_AM_I, 1);

  while (who_am_i_value != 0b01101100) {
      who_am_i_value = read_I2C1(IMU_ADDRESS_SHIN, WHO_AM_I, 1);
  }
}

void write_I2C1(int address, uint8_t reg, uint8_t data, int num_bytes, int stop) {
    I2C1->CR2 = 0; // Clear CR2 to prevent stale settings

    // Set address and write mode (write = RD_WRN = 0)
    I2C1->CR2 |= (address << 1);

    // Configure stop condition
    if (stop) {
        I2C1->CR2 |= I2C_CR2_AUTOEND;
    } else {
        I2C1->CR2 &= ~I2C_CR2_AUTOEND;
    }

    // Configure number of bytes (register address + 1 data byte)
    I2C1->CR2 |= _VAL2FLD(I2C_CR2_NBYTES, num_bytes);

    // Clear RD_WRN for write
    I2C1->CR2 &= ~I2C_CR2_RD_WRN;

    // Signal start
    I2C1->CR2 |= I2C_CR2_START;

    // Wait for TX buffer to be empty and send the register address
    while (!(I2C1->ISR & I2C_ISR_TXE));
    I2C1->TXDR = reg;

    // Wait for TX buffer to be empty and send the data byte
    while (!(I2C1->ISR & I2C_ISR_TXE));
    I2C1->TXDR = data;

    // Wait for transfer to complete
    while (!(I2C1->ISR & I2C_ISR_TC));

    // If autoend is not enabled, manually generate the stop condition
    if (!stop) {
        I2C1->CR2 |= I2C_CR2_STOP;
        while (I2C1->ISR & I2C_ISR_STOPF);
        I2C1->ICR |= I2C_ICR_STOPCF; // Clear stop flag
    }
}

char read_I2C1(int address, char reg, int num_bytes) {
volatile char output;

    // Step 1: Write the register address
    I2C1->CR2 = 0; // Clear CR2
    I2C1->CR2 |= (address << 1); // Device address with write bit
    I2C1->CR2 &= ~I2C_CR2_RD_WRN; // Write operation
    I2C1->CR2 |= _VAL2FLD(I2C_CR2_NBYTES, 1); // 1 byte to send (register address)
    I2C1->CR2 |= I2C_CR2_START; // Generate start condition

    // Wait for TX buffer to be ready and send the register address
    while (!(I2C1->ISR & I2C_ISR_TXE));
    I2C1->TXDR = reg;

    // Wait for transfer to complete
    while (!(I2C1->ISR & I2C_ISR_TC));

    // Step 2: Read the register value
    I2C1->CR2 = 0; // Clear CR2
    I2C1->CR2 |= (address << 1); // Device address with read bit
    I2C1->CR2 |= I2C_CR2_RD_WRN; // Read operation
    I2C1->CR2 |= _VAL2FLD(I2C_CR2_NBYTES, 1); // 1 byte to read
    I2C1->CR2 |= I2C_CR2_START; // Generate restart condition

    // Wait for RX buffer to have data
    while (!(I2C1->ISR & I2C_ISR_RXNE));

    // Read the data
    output = I2C1->RXDR;
      I2C1->CR2 |= I2C_CR2_AUTOEND;

    // Wait for the stop condition
    while (!(I2C1->ISR & I2C_ISR_STOPF));

    // Clear the stop flag
    I2C1->ICR |= I2C_ICR_STOPCF;

    return output;
}

/********************************************************************************
 *                             I2C3 Functions                                   *
 *  init_I2C3: initializes I2C3 peripheral and required GPIO pins (PB6 / PB7)   *
 *  write_I2C3: writes data to a addressed chip, and sub reg, with given data   *
 *       read_I2C3: reads data from requested sub reg in addressed chip         *
 ********************************************************************************/

void init_I2C3(void) {
  // Enable GPIOA and GPIOB clocks (if not done already)
  gpioEnable(GPIO_PORT_B);

  // Enable the clock for I2C3 peripheral (verify if the macro exists in your version of the HAL)
  RCC->APB1ENR1 |= RCC_APB1ENR1_I2C3EN; // This is for STM32L4, but if not working, check datasheet

  // Set both of the I2C lines to the correct alternate function mode
  pinMode(SCL2, GPIO_ALT); // SCL
  pinMode(SDA2, GPIO_ALT); // SDA

  // Select high-speed GPIO for both pins
  GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEED7; // PA7 (SCL)
  GPIOB->OSPEEDR |= GPIO_OSPEEDR_OSPEED4; // PB4 (SDA)

  // Set open-drain for SCL and SDA
  GPIOA->OTYPER |= GPIO_OTYPER_OT7; // PA7 (SCL)
  GPIOB->OTYPER |= GPIO_OTYPER_OT4; // PB4 (SDA)

  // Set AF4 for I2C functionality on PA7 (SCL) and PB4 (SDA)
  GPIOA->AFR[0] |= _VAL2FLD(GPIO_AFRL_AFSEL7, 4); // PA7 -> AF4
  GPIOB->AFR[0] |= _VAL2FLD(GPIO_AFRL_AFSEL4, 4); // PB4 -> AF4

  // Set I2C clock source to SYSCLK (if relevant)
  RCC->CCIPR &= ~RCC_CCIPR_I2C3SEL;  // Clear previous selection
  RCC->CCIPR |= _VAL2FLD(RCC_CCIPR_I2C3SEL, 0b01); // SYSCLK as source for I2C3

  // Disable I2C3 to configure it
  I2C3->CR1 &= ~I2C_CR1_PE;

  // Enable TX and TC interrupts (optional depending on need)
  I2C3->CR1 |= I2C_CR1_TXIE;
  I2C3->CR1 |= I2C_CR1_TCIE;

  // Enable byte control (SBC)
  I2C3->CR1 |= I2C_CR1_SBC;

  // Set the I2C Timing register for a 100kHz I2C clock (with an 8 MHz system clock)
  I2C3->TIMINGR &= ~I2C_TIMINGR_PRESC;  // Clear PRESC
  I2C3->TIMINGR |= (1 << I2C_TIMINGR_PRESC_Pos); // PRESC = 1

  I2C3->TIMINGR &= ~I2C_TIMINGR_SCLL;   // Clear SCLL
  I2C3->TIMINGR |= (0x13 << I2C_TIMINGR_SCLL_Pos); // Set SCLL

  I2C3->TIMINGR &= ~I2C_TIMINGR_SCLH;   // Clear SCLH
  I2C3->TIMINGR |= (0xF << I2C_TIMINGR_SCLH_Pos); // Set SCLH

  I2C3->TIMINGR &= ~I2C_TIMINGR_SDADEL; // Clear SDADEL
  I2C3->TIMINGR |= (0x2 << I2C_TIMINGR_SDADEL_Pos); // Set SDADEL

  I2C3->TIMINGR &= ~I2C_TIMINGR_SCLDEL; // Clear SCLDEL
  I2C3->TIMINGR |= (0x4 << I2C_TIMINGR_SCLDEL_Pos); // Set SCLDEL

  // Enable I2C3 peripheral
  I2C3->CR1 |= I2C_CR1_PE;

  // Test communication with the WHO_AM_I register on the IMU
  volatile uint8_t who_am_i_value2 = read_I2C3(IMU_ADDRESS_FEMAR, WHO_AM_I, 1);

  while (who_am_i_value2 != 0b01101100) {
      who_am_i_value2 = read_I2C3(IMU_ADDRESS_FEMAR, WHO_AM_I, 1);
  }
}



void write_I2C3(int address, uint8_t reg, uint8_t data, int num_bytes, int stop) {
    I2C3->CR2 = 0; // Clear CR2 to prevent stale settings

    // Set address and write mode (write = RD_WRN = 0)
    I2C3->CR2 |= (address << 1);

    // Configure stop condition
    if (stop) {
        I2C3->CR2 |= I2C_CR2_AUTOEND;
    } else {
        I2C3->CR2 &= ~I2C_CR2_AUTOEND;
    }

    // Configure number of bytes (register address + 1 data byte)
    I2C3->CR2 |= _VAL2FLD(I2C_CR2_NBYTES, num_bytes);

    // Clear RD_WRN for write
    I2C3->CR2 &= ~I2C_CR2_RD_WRN;

    // Signal start
    I2C3->CR2 |= I2C_CR2_START;

    // Wait for TX buffer to be empty and send the register address
    while (!(I2C3->ISR & I2C_ISR_TXE));
    I2C3->TXDR = reg;

    // Wait for TX buffer to be empty and send the data byte
    while (!(I2C3->ISR & I2C_ISR_TXE));
    I2C3->TXDR = data;

    // Wait for transfer to complete
    while (!(I2C3->ISR & I2C_ISR_TC));

    // If autoend is not enabled, manually generate the stop condition
    if (!stop) {
        I2C3->CR2 |= I2C_CR2_STOP;
        while (I2C3->ISR & I2C_ISR_STOPF);
        I2C3->ICR |= I2C_ICR_STOPCF; // Clear stop flag
    }
}

char read_I2C3(int address, char reg, int num_bytes) {
    volatile char output;

    // Step 1: Write the register address
    I2C3->CR2 = 0; // Clear CR2
    I2C3->CR2 |= (address << 1); // Device address with write bit
    I2C3->CR2 &= ~I2C_CR2_RD_WRN; // Write operation
    I2C3->CR2 |= _VAL2FLD(I2C_CR2_NBYTES, 1); // 1 byte to send (register address)
    I2C3->CR2 |= I2C_CR2_START; // Generate start condition

    // Wait for TX buffer to be ready and send the register address
    while (!(I2C3->ISR & I2C_ISR_TXE));
    I2C3->TXDR = reg;

    // Wait for transfer to complete
    while (!(I2C3->ISR & I2C_ISR_TC));

    // Step 2: Read the register value
    I2C3->CR2 = 0; // Clear CR2
    I2C3->CR2 |= (address << 1); // Device address with read bit
    I2C3->CR2 |= I2C_CR2_RD_WRN; // Read operation
    I2C3->CR2 |= _VAL2FLD(I2C_CR2_NBYTES, 1); // 1 byte to read
    I2C3->CR2 |= I2C_CR2_START; // Generate restart condition

    // Wait for RX buffer to have data
    while (!(I2C3->ISR & I2C_ISR_RXNE));

    // Read the data
    output = I2C3->RXDR;
    I2C3->CR2 |= I2C_CR2_AUTOEND;

    // Wait for the stop condition
    while (!(I2C3->ISR & I2C_ISR_STOPF));

    // Clear the stop flag
    I2C3->ICR |= I2C_ICR_STOPCF;

    return output;
}
