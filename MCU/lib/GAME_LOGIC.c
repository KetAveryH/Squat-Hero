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




int16_t decode_angle(int body_part, int16_t accel_x, int16_t accel_y, int16_t accel_z) {

    if (body_part == ANKLE) {
        theta_1 = (int16_t)(atan((float)accel_y / (float)accel_x) * 180 / M_PI); // Convert radians to degrees
        theta_ankle = 90 - theta_1;
        return theta_ankle;
    } else if (body_part == KNEE) {
        theta_2 = theta_ankle; // Assumes theta_ankle has been calculated previously
        theta_3 = (int16_t)(atan2((float)accel_y, (float)-accel_x) * 180 / M_PI); // Convert radians to degrees
        theta_4 = 90 - theta_3;
        theta_knee = theta_2 + theta_4;
        return theta_knee;
    } else if (body_part == HIP) {
        theta_5 = theta_4; // Assumes theta_4 has been calculated previously
        theta_6 = (int16_t)(atan2((float)accel_z, (float)accel_x) * 180 / M_PI); // Convert radians to degrees
        if (theta_6 < 0) {
          theta_6 += 360; // Normalize to 0–360°
        }
        theta_7 = 90 - theta_6;
        theta_hip = 180 + theta_5 + theta_7;
        return theta_hip;
    }

    // Default case if no match
    return -1; // Error code for invalid input
}

uint16_t decode_pos(int body_part, int axis) {

    if (body_part == TOE) {
        if (axis == X_AXIS) {
            toe_x = heel_x - FOOT_LENGTH;
            return toe_x;
        } else if (axis == Y_AXIS) {
            toe_y = 0.5 * LINE_THICKNESS;
            return toe_y;
        } else {
            return -1; // default case
        }
    } else if (body_part == HEEL) {
        if (axis == X_AXIS) {
            heel_x = 0.5 * MAX_DIMENSION_HOR;
            return heel_x;
        } else if (axis == Y_AXIS) {
            heel_y = 0.5 * LINE_THICKNESS;
            return heel_y;
        } else {
            return -1; // default case
        }
    } else if (body_part == KNEE) {
        if (axis == X_AXIS) {
            heel2knee_x = asin(theta_1) * SHIN_LENGTH;
            knee_x = heel_x - heel2knee_x;
            return knee_x;
        } else if (axis == Y_AXIS) {
            heel2knee_y = acos(theta_1) * SHIN_LENGTH;
            knee_y = heel_y + heel2knee_y;
            return knee_y;
        } else {
            return -1; // default case
        }
    } else if (body_part == HIP) {
        if (axis == X_AXIS) {
            knee2hip_x = asin(theta_3) * FEMAR_LENGTH;
            hip_x = knee_x + knee2hip_x;
            return hip_x;
        } else if (axis == Y_AXIS) {
            knee2hip_y = acos(theta_3) * FEMAR_LENGTH;
            hip_y = knee_y + knee2hip_y;
            return hip_y;
        } else {
            return -1; // default case
        }
    } else if (body_part == HEAD) {
        if (axis == X_AXIS) {
            hip2head_x = asin(theta_6) * TORSO_LENGTH;
            head_x = hip_x - hip2head_x;
            return head_x;
        } else if (axis == Y_AXIS) {
            hip2head_y = acos(theta_6) * TORSO_LENGTH;
            head_y = hip_y + hip2head_y;
            return head_y;
        } else {
            return -1; // default case
        }
    }

    // Explicit default case for invalid `str1`
    return -1;
}
