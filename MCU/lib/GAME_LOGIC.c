// LSM6DSO32 IMU.c file
// IMU functions

#include "GAME_LOGIC.h"
#include <string.h>
#include <math.h>
#include <stdint.h>

    // global variables to hold angles
    int16_t theta_ankle;
    int16_t theta_1;
    int16_t theta_2;
    int16_t theta_3;
    int16_t theta_4;
    int16_t theta_knee;
    int16_t theta_5;
    int16_t theta_6;
    int16_t theta_7;
    int16_t theta_hip;


  // instantiate all of my variables that I need
  uint16_t heel2knee_x;
  uint16_t heel2knee_y;
  uint16_t knee2hip_x;
  uint16_t knee2hip_y;
  uint16_t hip2head_x;
  uint16_t hip2head_y;
  uint16_t toe_x;
  uint16_t toe_y;
  uint16_t heel_x;
  uint16_t heel_y;
  uint16_t knee_x;
  uint16_t knee_y;
  uint16_t hip_x;
  uint16_t hip_y;
  uint16_t head_x;
  uint16_t head_y;



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

uint16_t decode_pos(char *str1, char *str2) {

    if (strcmp(str1, "toe") == 0) {
        if (strcmp(str2, "x") == 0) {
            toe_x = heel_x - FOOT_LENGTH;
            return toe_x;
        } else if (strcmp(str2, "y") == 0) {
            toe_y = 0.5 * LINE_THICKNESS;
            return toe_y;
        } else {
            return -1;
        }
    } else if (strcmp(str1, "heel") == 0) {
        if (strcmp(str2, "x") == 0) {
            heel_x = 0.5 * MAX_DIMENSION;
            return heel_x;
        } else if (strcmp(str2, "y") == 0) {
            heel_y = 0.5 * LINE_THICKNESS;
            return heel_y;
        } else {
            return -1;
        }
    } else if (strcmp(str1, "knee") == 0) {
        if (strcmp(str2, "x") == 0) {
            heel2knee_x = asin(theta_1) * SHIN_LENGTH;
            knee_x = heel_x - heel2knee_x;
            return knee_x;
        } else if (strcmp(str2, "y") == 0) {
            heel2knee_y = acos(theta_1) * SHIN_LENGTH;
            knee_y = heel_y + heel2knee_y;
            return knee_y;
        } else {
            return -1;
        }
    } else if (strcmp(str1, "hip") == 0) {
        if (strcmp(str2, "x") == 0) {
            knee2hip_x = asin(theta_3) * FEMAR_LENGTH;
            hip_x = knee_x + knee2hip_x;
            return hip_x;
        } else if (strcmp(str2, "y") == 0) {
            knee2hip_y = acos(theta_3) * FEMAR_LENGTH;
            hip_y = knee_y + knee2hip_y;
            return hip_y;
        } else {
            return -1;
        }
    } else if (strcmp(str1, "head") == 0) {
        if (strcmp(str2, "x") == 0) {
            hip2head_x = asin(theta_6) * TORSO_LENGTH;
            head_x = hip_x - hip2head_x;
            return head_x;
        } else if (strcmp(str2, "y") == 0) {
            hip2head_y = acos(theta_6) * TORSO_LENGTH;
            head_y = hip_y + hip2head_y;
            return head_y;
        } else {
            return -1;
        }
    }

    // Explicit default case for invalid `str1`
    return -1;
}
