// STM32L432KC_I2C.h
// Source code for I2C functions COPIED FROM TROY

//#include "STM32L432KC.h"
#include "STM32L432KC_I2C.h"
#include "STM32L432KC_GPIO.h"
//#include "STM32L432KC_RCC.h"
#include "stm32l432xx.h"
#include "STM32L432KC_IMU.h"

void init_I2C(void) {
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

  // initialize internal pull up resistors
  //GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPD10 | GPIO_PUPDR_PUPD9); // Clear bits
  //GPIOA->PUPDR |= (GPIO_PUPDR_PUPD10 | GPIO_PUPDR_PUPD9); // Pull-Up (01)

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

  // Program the peripheral input clock in I2C_CR2 Register in order to generate correct timings
  //I2C1->CR2 |= (8<<0);  // PCLK1 FREQUENCY in MHz

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

  // Configure the clock control registers
  //I2C1->C |= (40 << 0);  // check calculation in PDF dont think this needs to happen here for us
 // I2C1->CR1 |= (1<<0);  // Enable I2C
 // I2C1->CR1 |= (1<<10);  // Enable the ACK
  I2C1->CR1 |= I2C_CR1_PE;  // Generate START


  // check I2C communication with who_am_i register
#define TIMEOUT 1000  // Adjust this value as needed

  write_I2C(IMU_ADDRESS_SHIN, WHO_AM_I, 1, 0);
  volatile uint8_t who_am_i_value = read_I2C(IMU_ADDRESS_SHIN, WHO_AM_I, 1);
 // int timeout_counter = 0;

  while (who_am_i_value != 0b01101100) {
      who_am_i_value = read_I2C(IMU_ADDRESS_SHIN, WHO_AM_I, 1);
     // timeout_counter++;
     // if (timeout_counter >= TIMEOUT) {
          // Handle timeout: print error, set a flag, or reset
         // printf("WHO_AM_I read timed out.\n");
     // }
  }
}



void write_I2C(int address, char reg, int num_bytes, int stop){

    // configure address, assumes 7 bit address
    I2C1->CR2 |= (address << 1);

    // if we are not stopping, we reload at the end of num_bytes, otherwise we autoend
    if (stop) {
      I2C1->CR2 |= I2C_CR2_AUTOEND;
    } 
    else { 
      I2C1->CR2 &= ~I2C_CR2_AUTOEND;
    }

    // configure size of data package
    I2C1->CR2 &= ~I2C_CR2_NBYTES;
    I2C1->CR2 |= _VAL2FLD(I2C_CR2_NBYTES, (uint8_t)num_bytes);

    // set the read/not write bit to 0 for write
    I2C1->CR2 &= ~I2C_CR2_RD_WRN;

    // signal to start
   // I2C1->CR1 |= I2C_CR1_PE; 
    I2C1->CR2 |= I2C_CR2_START; // THIS WILL NOT TURN ON NO MATTER HOW I SET IT

    // send over each byte in the data package
    //for (int i = 0; i < num_bytes; i++) {
      while (!(I2C1->ISR & I2C_ISR_TXE)); // wait until transmit buffer is empty
      I2C1->TXDR = reg;

      //I2C1->TXDR &= ~(I2C_TXDR_TXDATA);

   // }
}


char read_I2C(int address, char reg, int num_bytes) {

    volatile char output;
    // configure address, assumes 7 bit address
    I2C1->CR2 |= (address << 1); // put seven bits of address in starting in bit 1 of the CR2 register

    // set autoend
    I2C1->CR2 |= I2C_CR2_AUTOEND;

    // configure size of data package
    I2C1->CR2 &= ~I2C_CR2_NBYTES;
    I2C1->CR2 |= _VAL2FLD(I2C_CR2_NBYTES, (uint8_t)num_bytes);

    // set the read/not write bit to 1 for read
    I2C1->CR2 |= I2C_CR2_RD_WRN;

    // signal to start
    I2C1->CR2 |= I2C_CR2_START;

   // read in each byte desired
   // for (int i = 0; i < num_bytes; i++) {
 int timeout = 5;
    while (!(I2C1->ISR & I2C_ISR_RXNE)) {
      if (--timeout == 0) {
        // Handle timeout error
        return -1; // Indicate failure
    }
}

    
     // wait until data is ready to be read
      output = I2C1->RXDR;
    return output;   
}
