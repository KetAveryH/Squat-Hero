// STM32F401RE_RCC.c
// Source code for RCC functions

#include "STM32L432KC_RCC.h"

void configureMSI(void) {
   RCC->CR |= _VAL2FLD(RCC_CR_MSION, 0b1);       // Turn on MSI
   RCC->CR |= _VAL2FLD(RCC_CR_MSIRANGE, 0b0111); // configure MSI to be 8MHz
   while(!RCC_CR_MSIRDY);                        // wait for the HSI to be ready

   // Switch SYSCLK to MSI
   RCC->CFGR &= ~RCC_CFGR_SW;                     // Clear the SW bits
   RCC->CFGR |= RCC_CFGR_SW_MSI;                  // Set MSI as SYSCLK source

   while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_MSI); // Wait until SWS bits are set to MSI
}

void configureClock(void){
  // Configure and set MSI to be SYSCLK at 100kHz
  configureMSI();
}