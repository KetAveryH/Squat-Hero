// WRITTEN WITH CHAT

// main.c

//#include "../lib/STM32L432KC_GPIO.h"
#include "../lib/STM32L432KC_I2C.h"
#include "../lib/STM32L432KC_RCC.h"
#include "../lib/STM32L432KC_FLASH.h"
#include "../lib/STM32L432KC_IMU.h"
#include "../lib/GAME_LOGIC.h"
#include <stdio.h>

// Necessary includes for printf to work///////////////////
#include "stm32l432xx.h"

// Function used by printf to send characters to the laptop
int _write(int file, char *ptr, int len) {
  int i = 0;
  for (i = 0; i < len; i++) {
    ITM_SendChar((*ptr++));
  }
  return len;
}
////////////////////////////////////////////////////////////


int main(void) {
    // Step 1: System Initialization
    configureFlash();   // Configure flash memory access
    configureClock();  // Configure the system clock
    init_I2C();        // Initialize the I2C peripheral

    // Step 2: Initialize the LSM6DSO32 IMU
    IMU_config(IMU_ADDRESS_SHIN, CTRL1_XL);
    //IMU_config(IMU_ADDRESS_FEMAR, CTRL1_XL);
    //IMU_config(IMU_ADDRESS_TORSO, CTRL1_XL);

    // Step 3: 
    while (1) {


        //*******************
        // accelerometer data from all 3 axis, getting a uint16_t form outputs. Raw data format not in g's
        uint16_t raw_accel_x_shin = read_accel(IMU_ADDRESS_SHIN, OUTX_L_A);
        uint16_t raw_accel_y_shin = read_accel(IMU_ADDRESS_SHIN, OUTY_L_A);
        uint16_t raw_accel_z_shin = read_accel(IMU_ADDRESS_SHIN, OUTZ_L_A);

        //uint16_t raw_accel_x_femar = read_accel(IMU_ADDRESS_FEMAR, OUTX_L_A);
        //uint16_t raw_accel_y_femar = read_accel(IMU_ADDRESS_FEMAR, OUTY_L_A);
        //uint16_t raw_accel_z_femar = read_accel(IMU_ADDRESS_FEMAR, OUTZ_L_A);

        //uint16_t raw_accel_x_torso = read_accel(IMU_ADDRESS_TORSO, OUTX_L_A);
        //uint16_t raw_accel_y_torso = read_accel(IMU_ADDRESS_TORSO, OUTY_L_A);
        //uint16_t raw_accel_z_torso = read_accel(IMU_ADDRESS_TORSO, OUTZ_L_A);
        //*******************


        //*******************
        // converting the twos compliment values into signed values
        int16_t accel_x_shin = twoComplement2Signed(raw_accel_x_shin);
        int16_t accel_y_shin = twoComplement2Signed(raw_accel_y_shin);
        int16_t accel_z_shin = twoComplement2Signed(raw_accel_z_shin);

        //int16_t accel_x_femar = twoComplement2Signed(raw_accel_x_femar);
        //int16_t accel_y_femar = twoComplement2Signed(raw_accel_y_femar);
        //int16_t accel_z_femar = twoComplement2Signed(raw_accel_z_femar);

        //int16_t accel_x_torso = twoComplement2Signed(raw_accel_x_torso);
        //int16_t accel_y_torso = twoComplement2Signed(raw_accel_y_torso);
        //int16_t accel_z_torso = twoComplement2Signed(raw_accel_z_torso);
        //*******************


        //*******************
        // converstion into angles, this is different for all of the indivisual IMUs due to orientaition they are mounted
        // KEY: (X/Y/Z)(+/- as you squat): explination
        // - chest (screws to the left): X(+)=down, Z(+)=out of chest, Y(N/A)=left
        // - thigh (screws forward, chip on side of leg): X(+)=down, Z(N/A)=right, Y(-)=forward
        // - shin (screws forward, chip on side of leg): X(+)=down, Z(N/A)=right, Y(+)=forward
        char *hip = "hip";
        char *knee = "knee";
        char *ankle = "ankle";

        int16_t angle_ankle = decode_angle(ankle, accel_x_shin, accel_y_shin, accel_z_shin);
        //int16_t angle_knee = decode_angle(knee, accel_x_femar, accel_y_femar, accel_z_femar);
        //int16_t angle_hip = decode_angle(hip, accel_x_torso, accel_y_torso, accel_z_torso);
        //*******************

        //*******************
        // convert the angles, and desired line lengths to give our the positions of all of the individual joints and nodes
        // - look at the picture to better understand
        // - cycle through and update all of the positions sequenitally
        // - MUST CONFIGURE HEEL FIRST, because everything is based off of this location
        char *toe = "toe";
        char *heel = "heel";
        //char *knee = "knee"; already declared
        //char *toe = "hip";   already declared
        char *head = "head";
        char *x = "x";
        char *y = "y";

        // calculate the origin position of the heel one time
        int delay1 = 0;

        while(delay1 == 0) {
          uint16_t x_heel = decode_pos(heel, x);
          uint16_t y_heel = decode_pos(heel, y);
          delay1 += 1;
        }
        /////////

        // calculate the rest of the positions from the reference point of the heel
        uint16_t x_toe = decode_pos(toe, x);
        uint16_t y_toe = decode_pos(toe, y);
        uint16_t x_knee = decode_pos(knee, x);
        uint16_t y_knee = decode_pos(knee, y);
        uint16_t x_hip = decode_pos(hip, x);
        uint16_t y_hip = decode_pos(hip, y);
        uint16_t x_head = decode_pos(head, x);
        uint16_t y_head = decode_pos(head, y);
        //*******************

        
        // Print the X-axis accelerometer value (this assumes you have some serial or debug output setup)
        printf("Accelerometer X-axis: %d\n", angle_ankle);

        // Delay (for demonstration purposes, adjust as needed)
        for (volatile int i = 0; i < 100; i++); // Simple delay
    }

    return 0;
}

