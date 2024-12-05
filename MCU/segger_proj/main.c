// Jason Bowman
// 11/10/24 (created)
// ??????? (finished)

// main.c

#include "../lib/STM32L432KC_I2C.h"
#include "../lib/STM32L432KC_RCC.h"
#include "../lib/STM32L432KC_FLASH.h"
#include "../lib/STM32L432KC_IMU.h"
#include "../lib/GAME_LOGIC.h"
#include "../lib/STM32L432KC_SPI.h"
#include <stdio.h>

/*************************************************
*                 print functions                *
*************************************************/
#include "stm32l432xx.h"

// Function used by printf to send characters to the laptop
int _write(int file, char *ptr, int len) {
  int i = 0;
  for (i = 0; i < len; i++) {
    ITM_SendChar((*ptr++));
  }
  return len;
}

/******************************************************************
*                          MAIN FUNCTION                          *
*     the main functions inializes all chips, reads data,         *
*      and decodes the accelerometer data into x/y posisitons     *   
*       to be sent over SPI to the FPGA for VGA control           *
******************************************************************/
int main(void) {

    /*******************************************
    *   INITIALIZE AND CONFIUGRE MCU / IMUs    *
    *******************************************/
    // Step 1: System Initialization 
    configureFlash();   // Configure flash memory access
    configureClock();   // Configure the system clock
    init_I2C1();        // Initialize the I2C1 peripheral & corresponding GPIOs
    init_I2C3();        // Initialize the I2C3 peripheral & corresponding GPIOs
    initSPI(6, 0, 0);

    // Step 2: Initialize all of the LSM6DSO32 IMU
    IMU_config_I2C3(IMU_ADDRESS_FEMAR, CTRL1_XL);
    IMU_config_I2C1(IMU_ADDRESS_SHIN, CTRL1_XL);
    IMU_config_I2C1(IMU_ADDRESS_TORSO, CTRL1_XL);
    digitalWrite(LOAD, 0); // make sure SPI LOAD is low

    // Step 3: Run while(1) loop until the end of tiiiimmmmeee (there is no time limit)
    while (1) {

        /************************************************
        *          READ RAW DATA FROM ALL IMUs          *
        ************************************************/
        int16_t accel_x_shin = read_accel_I2C1(IMU_ADDRESS_SHIN, OUTX_L_A);
        int16_t accel_y_shin = read_accel_I2C1(IMU_ADDRESS_SHIN, OUTY_L_A);
        int16_t accel_z_shin = read_accel_I2C1(IMU_ADDRESS_SHIN, OUTZ_L_A);

        int16_t accel_x_femar = read_accel_I2C3(IMU_ADDRESS_FEMAR, OUTX_L_A);
        int16_t accel_y_femar = read_accel_I2C3(IMU_ADDRESS_FEMAR, OUTY_L_A);
        int16_t accel_z_femar = read_accel_I2C3(IMU_ADDRESS_FEMAR, OUTZ_L_A);

        int16_t accel_x_torso = read_accel_I2C1(IMU_ADDRESS_TORSO, OUTX_L_A);
        int16_t accel_y_torso = read_accel_I2C1(IMU_ADDRESS_TORSO, OUTY_L_A);
        int16_t accel_z_torso = read_accel_I2C1(IMU_ADDRESS_TORSO, OUTZ_L_A);

        /************************************************************************************************
        *           CONVERT ACCEL TO ANGLES                                                             *
        *        KEY: (X/Y/Z)(+/- as you squat): explination                                            *
        * - chest (screws to the left): X(+)=down, Z(+)=out of chest, Y(N/A)=left                       *
        * - thigh (screws forward, chip on side of leg): X(+)=down, Z(N/A)=right, Y(-)=forward          *
        * - shin (screws forward, chip on side of leg): X(+)=down, Z(N/A)=right, Y(+)=forward           *
        ************************************************************************************************/

        int16_t angle_ankle = decode_angle(ANKLE, accel_x_shin, accel_y_shin, accel_z_shin);
        int16_t angle_knee = decode_angle(KNEE, accel_x_femar, accel_y_femar, accel_z_femar);
        int16_t angle_hip = decode_angle(HIP, accel_x_torso, accel_y_torso, accel_z_torso);

        /**********************************************************************************
        *                           LINE LENGTH CALCULATIONS                              *
        *     GOAL: convert the angles, and desired line lengths to give our              *
        *     the positions of all of the individual joints and nodes                     *
        *                                                                                 *
        * - look at the picture on website to better understand                           *
        * - cycle through and update all of the positions sequenitally                    *
        * - MUST CONFIGURE HEEL FIRST, because everything is based off of this location   *
        **********************************************************************************/
        
        // calculate the origin position of the heel one time
        uint16_t x_heel = 0, y_heel = 0;
        int delay1 = 0;

        while (delay1 == 0) {
            x_heel = decode_pos(HEEL, X_AXIS);
            y_heel = decode_pos(HEEL, Y_AXIS);
            delay1 = 1; // Ensure this runs only once
        }

        // calculate the rest of the positions from the reference point of the heel
        uint16_t x_toe = decode_pos(TOE, X_AXIS);
        uint16_t y_toe = decode_pos(TOE, Y_AXIS);
        uint16_t x_knee = decode_pos(KNEE, X_AXIS);
        uint16_t y_knee = decode_pos(KNEE, Y_AXIS);
        uint16_t x_hip = decode_pos(HIP, X_AXIS);
        uint16_t y_hip = decode_pos(HIP, Y_AXIS);
        uint16_t x_head = decode_pos(HEAD, X_AXIS);
        uint16_t y_head = decode_pos(HEAD, Y_AXIS);

        // FOLLOW THE pinMode function for the outline to choose different things in the game logic functions I have written

        // Print the X-axis accelerometer value (this assumes you have some serial or debug output setup)
        //***************************************
        //printf("SHIN X: %d, Y: %d, Z: %d | FEMUR X: %d, Y: %d, Z: %d | TORSO X: %d, Y: %d, Z: %d\n", 
        //       accel_x_shin, accel_y_shin, accel_z_shin, 
        //       accel_x_femar, accel_y_femar, accel_z_femar, 
        //       accel_x_torso, accel_y_torso, accel_z_torso);
        //delay_ms(5);
        //***************************************

        // Print the angles
        //***************************************
        printf("ANKLE: %d | KNEE: %d | HIP: %d\n", 
              angle_ankle, 
              angle_knee, 
              angle_hip);
        delay_ms(5);
        //***************************************

        // Print (X,Y) positions
        //***************************************
        //printf("HEEL: X: %u, Y: %u | TOE: X: %u, Y: %u | KNEE: X: %u, Y: %u | HIP: X: %u, Y: %u | HEAD: X: %u, Y: %u\n", 
        //     x_heel, y_heel, 
        //     x_toe, y_toe, 
        //     x_knee, y_knee, 
        //     x_hip, y_hip, 
        //     x_head, y_head);
        //delay_ms(5);
        //***************************************

        // Print ALL angles and positions
        //***************************************
        //printf("ANKLE: %d KNEE: %d HIP: %d HEEL: X: %u Y: %u TOE: X: %u Y: %u KNEE: X: %u Y: %u HIP: X: %u Y: %u HEAD: X: %u Y: %u\n", 
        //       angle_ankle, 
        //       angle_knee, 
        //       angle_hip, 
        //       x_heel, y_heel, 
        //       x_toe, y_toe, 
        //       x_knee, y_knee, 
        //       x_hip, y_hip, 
        //       x_head, y_head);
        //delay_ms(5);
        //***************************************

        //***************************************
        //printf("TOE: X: %u, Y: %u\n", 
        //       x_toe, y_toe);
        //delay_ms(5);
        //***************************************

        //***************************************
        //printf("HEEL: X: %u, Y: %u\n", 
        //       x_heel, y_heel);
        //delay_ms(5);
        //***************************************

        //***************************************
        //printf("KNEE: X: %u, Y: %u\n", 
        //       x_knee, y_knee);
        //delay_ms(5);
        //***************************************

        //***************************************
        //printf("HIP: X: %u, Y: %u\n", 
        //       x_hip, y_hip);
        //delay_ms(5);
        //***************************************

        //***************************************
        //printf("HEAD: X: %u, Y: %u\n", 
        //       x_head, y_head);
        //delay_ms(5);
        //***************************************
        

      pinMode(LOAD, GPIO_OUTPUT); // LOAD
      
      char data[16] = {0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00,
                0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00};

      digitalWrite(LOAD, 1);
      
      int i;

         for(i = 0; i < 16; i++) {
            digitalWrite(SPI_CE, 1); // Arificial CE high
            spiSendReceive(data[i]);
            digitalWrite(SPI_CE, 0); // Arificial CE low
          }
          digitalWrite(LOAD, 0);
    }

    return 0;
}