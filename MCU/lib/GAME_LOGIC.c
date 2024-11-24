// LSM6DSO32 IMU.c file
// IMU functions

#include "GAME_LOGIC.h"
#include <string.h>
#include <math.h>
#include <stdint.h>


volatile int x_accel; // 'volatile' because its value changes asynchronously


// Function to convert 16-bit two's complement to signed integer
int16_t twoComplement2Signed(uint16_t raw_value) {
    // Check if the MSB (bit 15) is set (indicating a negative number)
    if (raw_value & 0x8000) {
        // Convert to negative value by applying two's complement
        return (int16_t)(raw_value - 0x10000);
    } else {
        // Otherwise, it's already positive
        return (int16_t)raw_value;
    }
}

int16_t decode_angle(char *str, int16_t accel_x, int16_t accel_y, int16_t accel_z) {
    // Local variables to hold angles
    int16_t theta_ankle = 0;
    int16_t theta_1 = 0;
    int16_t theta_2 = 0;
    int16_t theta_3 = 0;
    int16_t theta_4 = 0;
    int16_t theta_knee = 0;
    int16_t theta_5 = 0;
    int16_t theta_6 = 0;
    int16_t theta_7 = 0;
    int16_t theta_hip = 0;

    // Ensure no divide-by-zero occurs
    if (accel_x == 0) {
        return -1; // Error code for divide-by-zero
    }

    if (strcmp(str, "ankle") == 0) {
        theta_1 = (int16_t)(atan((float)accel_y / (float)accel_x) * 180 / M_PI); // Convert radians to degrees
        theta_ankle = 90 - theta_1;
        return theta_ankle;
    } else if (strcmp(str, "knee") == 0) {
        theta_2 = 90; // Assumes theta_ankle has been calculated previously
        theta_3 = (int16_t)(atan((float)-accel_y / (float)accel_x) * 180 / M_PI); // Convert radians to degrees
        theta_4 = 90 - theta_3;
        theta_knee = theta_2 + theta_4;
        return theta_knee;
    } else if (strcmp(str, "hip") == 0) {
        theta_5 = theta_4; // Assumes theta_4 has been calculated previously
        theta_6 = (int16_t)(atan((float)accel_z / (float)accel_x) * 180 / M_PI); // Convert radians to degrees
        theta_7 = 90 - theta_6;
        theta_hip = theta_5 + theta_7;
        return theta_hip;
    }

    // Default case if no match
    return -1; // Error code for invalid input
}

uint16_t decode_pos(char *str1, char *str2, uint16_t length, int16_t angle) {

  // instantiate all of my variables that I need
  uint16_t heel2knee_x = 0;
  uint16_t heel2knee_y = 0;
  uint16_t knee2hip_x = 0;
  uint16_t knee2hip_y = 0;
  uint16_t hip2head_x = 0;
  uint16_t hip2head_y = 0;
  volatile uint16_t output;

  // TOE
    if (strcmp(str1, "toe") == 0) {

      if (strcmp(str2, "x") == 0) {
        
      }

      else if (strcmp(str2, "y") == 0) {
      
      }

    }

  // HEEL
    else if (strcmp(str1, "heel") == 0) {
    
      if (strcmp(str2, "x") == 0) {
      
      }

      else if (strcmp(str2, "y") == 0) {
      
      }

    }

  // KNEE
    else if (strcmp(str1, "knee") == 0) {
    
      if (strcmp(str2, "x") == 0) {
      
      }

      else if (strcmp(str2, "y") == 0) {
      
      }

    }

    // HIP
    else if (strcmp(str1, "hip") == 0) {
    
      if (strcmp(str2, "x") == 0) {
      
      }

      else if (strcmp(str2, "y") == 0) {
      
      }

    }

    // HEAD
    else if (strcmp(str1, "head") == 0) {
    
      if (strcmp(str2, "x") == 0) {
      
      }

      else if (strcmp(str2, "y") == 0) {
      
      }

    }
}