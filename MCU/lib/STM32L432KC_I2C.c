// STM32L432KC_I2C.h
// Source code for I2C functions COPIED FROM TROY

//#include "STM32L432KC.h"
#include "STM32L432KC_I2C.h"
#include "STM32L432KC_GPIO.h"
//#include "STM32L432KC_RCC.h"
#include "stm32l432xx.h"

void init_I2C(void) {


  // initialize for I2C to work on 'B' I/Os
  gpioEnable(GPIO_PORT_A);

  RCC->CCIPR &= ~RCC_CCIPR_I2C1SEL;
  RCC->CCIPR |= _VAL2FLD(RCC_CCIPR_I2C1SEL, 0b01);  //set I2C clk as the SYSCLK
  RCC->APB1ENR1 |= RCC_APB1ENR1_I2C1EN;//enable the clock to run on I2C1 peripheral
  
  //disable both analog and digital filters and clock stretching
  //I2C1->CR1 &= ~I2C_CR1_ANFOFF;
  //I2C1->CR1 &= ~I2C_CR1_DNF;
  //I2C1->CR1 &= ~I2C_CR1_NOSTRETCH; // this must be disables in master mode

  // set both of the I2C lines to be the correct alternate function mode
  pinMode(I2C_SDA, GPIO_ALT);
  pinMode(I2C_SCL, GPIO_ALT);

  // select for open drain I/O and
  GPIOA->OTYPER |= (GPIO_OTYPER_OT9 | GPIO_OTYPER_OT10);

  // select high speed GPIO
  GPIOA->OSPEEDR |= (GPIO_OSPEEDR_OSPEED10 | GPIO_OSPEEDR_OSPEED9);

  // initialize internal pull up resistors
  GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPD10 | GPIO_PUPDR_PUPD9); // Clear bits
  GPIOA->PUPDR |= (GPIO_PUPDR_PUPD10_0 | GPIO_PUPDR_PUPD9_0); // Pull-Up (01)

  // set alternat funcntion "high" registers
  //GPIOB->AFRH |= (GPIO_AFRH_AFR ---- WHY IS THIS ONE NOT WORKING
  GPIOA->AFR[1] &= ~((0xF << (4 * 1)) | (0xF << (4 * 2))); // Clear AFR[1] bits for pins 9 and 10
  GPIOA->AFR[1] |= (4 << (4 * 1)) | (4 << (4 * 2));        // AF4 for I2C1

  // reset the I2C peripheral so that it can reboot corectly
  //I2C1->CR1 &= ~I2C_CR1_PE;    //disable I2C
  I2C1->CR1 |= I2C_CR1_SWRST;  //reset Is2
  I2C1->CR1 &= ~I2C_CR1_SWRST; //clear reset

  // auto end
  I2C1->CR2 |= I2C_CR2_AUTOEND;

  // Program the peripheral input clock in I2C_CR2 Register in order to generate correct timings
  I2C1->CR2 |= (8<<0);  // PCLK1 FREQUENCY in MHz

  // Set the I2C Timing register for a 100kHz I2C clock with a 8 MHz system clock (LOOK AT PAGE 1159) in reference manual
  I2C1->TIMINGR &= ~I2C_TIMINGR_PRESC;     // Clear the PRESC field
  I2C1->TIMINGR |= (1 << I2C_TIMINGR_PRESC_Pos); // Set PRESC

  I2C1->TIMINGR &= ~I2C_TIMINGR_SCLL;      // Clear SCL low period
  I2C1->TIMINGR |= (0x13 << I2C_TIMINGR_SCLL_Pos); // Set SCLL 

  I2C1->TIMINGR &= ~I2C_TIMINGR_SCLH;      // Clear SCL high period
  I2C1->TIMINGR |= (0xF << I2C_TIMINGR_SCLH_Pos); // Set SCLH

  // Configure the clock control registers
  //I2C1->C |= (40 << 0);  // check calculation in PDF dont think this needs to happen here for us
  I2C1->CR1 |= (1<<0);  // Enable I2C
  I2C1->CR1 |= (1<<10);  // Enable the ACK
  I2C1->CR1 |= (1<<8);  // Generate START

  // enable I2C
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
        *(volatile char *) (&I2C1->TXE) = reg;

    // Write a data value to the resister
    while (!(I2C1->ISR & I2C_ISR_TXIS));
        *(volatile char *) (&I2C1->TXE) = data;

    //autoend will stop the process

}

char read_I2C(char addr, char reg) {
    char received_data;


    // Step 1: Write the register address to read from
    // Clear CR2 settings from previous transactions
    I2C1->CR2 &= ~(I2C_CR2_SADD | I2C_CR2_NBYTES | I2C_CR2_RD_WRN);

    // Master operates in 7-bit addressing mode
    I2C1->CR2 &= ~I2C_CR2_ADD10;

    // Slave address for writing (to send the register address)
    I2C1->CR2 |= _VAL2FLD(I2C_CR2_SADD, (addr << 1));

    // Master requests a write transfer
    I2C1->CR2 |= I2C_CR2_RD_WRN;

    // Number of bytes to write (1 byte for the register address)
    I2C1->CR2 |= _VAL2FLD(I2C_CR2_NBYTES, 1);

    // Start I2C Master Write
    I2C1->CR2 |= I2C_CR2_START;

    // Write the register address to TXDR
    while (!(I2C1->ISR & I2C_ISR_RXNE)); //MAYBE USE RXNE
        *(volatile char *)(&I2C1->TXDR) = reg;

    // Wait for transfer to complete
    while (!(I2C1->ISR & I2C_ISR_TC));

    // Step 2: Read data from the register
    // Clear CR2 settings from previous transactions
    I2C1->CR2 &= ~(I2C_CR2_SADD | I2C_CR2_NBYTES | I2C_CR2_RD_WRN);

    // Slave address for reading
    I2C1->CR2 |= _VAL2FLD(I2C_CR2_SADD, (addr << 1));

    // Master requests a read transfer
    I2C1->CR2 |= I2C_CR2_RD_WRN;

    // Number of bytes to read (1 byte for the data)
    I2C1->CR2 |= _VAL2FLD(I2C_CR2_NBYTES, 1);

    // Start I2C Master Read
    I2C1->CR2 |= I2C_CR2_START;

    // Wait for the received data to be available
    while (!(I2C1->ISR & I2C_ISR_RXNE));
        received_data = *(volatile char *)(&I2C1->RXDR);

    // Wait for transfer to complete
    while (!(I2C1->ISR & I2C_ISR_TC));

    // Return the received data
    return received_data;
}
