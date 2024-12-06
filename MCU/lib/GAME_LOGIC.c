// LSM6DSO32 IMU.c file
// IMU functions

#include "GAME_LOGIC.h"
#include "STM32L432KC_GPIO.h"
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




  void setup(void) {
    pinMode(OUT_0, GPIO_OUTPUT);
    pinMode(OUT_1, GPIO_OUTPUT);
    pinMode(OUT_2, GPIO_OUTPUT);
    pinMode(OUT_3, GPIO_OUTPUT);
}

void delay_ms(uint32_t ms) {
    // Assuming the system clock is running at a frequency of 16 MHz
    // You may need to adjust this based on your clock speed
    uint32_t i;
    uint32_t j;
    for(i = 0; i < ms; i++) {
        for(j = 0; j < 1600; j++) {
            __asm("NOP");  // No operation, just a simple delay
        }
    }
}


int16_t decode_angle(int body_part, int16_t accel_x, int16_t accel_y, int16_t accel_z) {
    // ANKLE
    if (body_part == ANKLE) { // normal scenario
      if (accel_x < 0) {
        if (accel_y > 0) {
          theta_1 = (int16_t)(atan((float)accel_y / (float)-accel_x) * 180 / M_PI); // Convert radians to degrees
          theta_ankle = 90 + theta_1;
          return theta_ankle;
        } else if (accel_y < 0) {
          theta_1 = (int16_t)(atan((float)-accel_y / (float)-accel_x) * 180 / M_PI); // Convert radians to degrees
          theta_ankle = 90 - theta_1;
          return theta_ankle;
        } else {
          return 90;
        }
      } else if (accel_x > 0) { // unexpected scenario
        if (accel_y > 0) {
          theta_1 = (int16_t)(atan((float)accel_y / (float)-accel_x) * 180 / M_PI); // Convert radians to degrees
          theta_ankle = 270 + theta_1;
          return theta_ankle;
        } else if (accel_y < 0) {
          theta_1 = (int16_t)(atan((float)-accel_y / (float)-accel_x) * 180 / M_PI); // Convert radians to degrees
          theta_ankle = -(360 - (270 - theta_1));
          return theta_ankle;
        } else {
          return 90;
        }
      }
    // KNEE
    } else if (body_part == KNEE) {
      if (accel_x < 0) { // normal scenario
        if (accel_y > 0) {
          theta_2 = theta_ankle;
          theta_3 = (int16_t)(atan((float)accel_y / (float)-accel_x) * 180 / M_PI); // Convert radians to degrees
          theta_4 = 90 - theta_3;
          theta_knee = theta_2 + theta_4;
          return theta_knee;
        } else if (accel_y < 0) {
          theta_2 = theta_ankle;
          theta_3 = (int16_t)(atan((float)-accel_y / (float)-accel_x) * 180 / M_PI); // Convert radians to degrees
          theta_4 = 90 + theta_3;
          theta_knee = theta_2 + theta_4;
          return theta_knee;
        } else {
          theta_knee = 180 - (theta_3 + theta_1);
          return theta_knee;
        }
      } else if (accel_x > 0) { // unexpected scenario
        if (accel_y > 0) {
          theta_2 = theta_ankle;
          theta_3 = (int16_t)(atan((float)accel_y / (float)-accel_x) * 180 / M_PI); // Convert radians to degrees
          theta_4 = 90 + theta_3;
          theta_3 = theta_3 + 180;
          theta_knee = theta_2 - theta_4;
          return theta_knee;
        } else if (accel_y < 0) {
          theta_2 = theta_ankle;
          theta_3 = (int16_t)(atan((float)-accel_y / (float)-accel_x) * 180 / M_PI); // Convert radians to degrees
          theta_4 = -(360 - (270 + theta_3));
          theta_knee = theta_2 + theta_4;
          return theta_knee;
        } else {
          theta_knee = 180 - (theta_3 + theta_1);
          return theta_knee;
        }
      }
    // HIP
    } else if (body_part == HIP) {
      if (accel_x > 0) { // normal scenario
        if (accel_z > 0) {
          theta_5 = 90 - theta_3;
          theta_6 = (int16_t)(atan((float)accel_z / (float)accel_x) * 180 / M_PI); // Convert radians to degrees
          theta_7 = 90 + theta_6;
          theta_hip = theta_5 + theta_7;
          return theta_hip;
        } else if (accel_z < 0) {
          theta_5 = 90 - theta_3;
          theta_6 = (int16_t)(atan((float)-accel_z / (float)accel_x) * 180 / M_PI); // Convert radians to degrees
          theta_7 = 90 - theta_6;
          theta_hip = theta_5 + theta_7;
          return theta_hip;
        } else {
          theta_hip = 180 - theta_3;
        }
      } else if (accel_x < 0) { // unexpected scenario
        if (accel_z > 0) {
          theta_5 = 90 - theta_3;
          theta_6 = (int16_t)(atan((float)accel_z / (float)accel_x) * 180 / M_PI); // Convert radians to degrees
          theta_7 = 90 + theta_6;
          theta_hip = theta_5 - theta_7;
          return theta_hip;
        } else if (accel_z < 0) {
          theta_5 = 90 - theta_3;
          theta_6 = (int16_t)(atan((float)-accel_z / (float)accel_x) * 180 / M_PI); // Convert radians to degrees
          theta_7 = -(360 - (270 - theta_6));
          theta_hip = theta_5 + theta_7;
          return theta_hip;
        } else {
          theta_hip = 180 - theta_3;
        }
      }
    }

    return -1; // Error code for invalid input
}


// (X,Y) bottom left
uint16_t decode_pos(int body_part, int axis) {
    const float DEG_TO_RAD = M_PI / 180.0; // Conversion factor from degrees to radians

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
            heel2knee_x = sin((90 - theta_ankle) * DEG_TO_RAD) * SHIN_LENGTH;
            knee_x = heel_x - heel2knee_x;
            return knee_x;
        } else if (axis == Y_AXIS) {
            heel2knee_y = cos((90 - theta_ankle) * DEG_TO_RAD) * SHIN_LENGTH;
            knee_y = heel_y + heel2knee_y;
            return knee_y;
        } else {
            return -1; // default case
        }
    } else if (body_part == HIP) {
        if (axis == X_AXIS) {
            knee2hip_x = sin((180 - (90 - theta_ankle) - theta_knee) * DEG_TO_RAD) * FEMAR_LENGTH;
            hip_x = knee_x + knee2hip_x;
            return hip_x;
        } else if (axis == Y_AXIS) {
            knee2hip_y = cos((180 - (90 - theta_ankle) - theta_knee) * DEG_TO_RAD) * FEMAR_LENGTH;
            hip_y = knee_y + knee2hip_y;
            return hip_y;
        } else {
            return -1; // default case
        }
    } else if (body_part == HEAD) {
        if (axis == X_AXIS) {
            hip2head_x = sin((180 - ((180 - (90 - theta_ankle) - theta_knee)) - theta_hip) * DEG_TO_RAD) * TORSO_LENGTH;
            head_x = hip_x - hip2head_x;
            return head_x;
        } else if (axis == Y_AXIS) {
            hip2head_y = cos((180 - ((180 - (90 - theta_ankle) - theta_knee)) - theta_hip) * DEG_TO_RAD) * TORSO_LENGTH;
            head_y = hip_y + hip2head_y;
            return head_y;
        } else {
            return -1; // default case
        }
    }

    // Explicit default case for invalid `body_part`
    return -1;
}


void updatePins(int16_t angle_femar) {
    // OUT_0, OUT_1, OUT_2
    if (angle_femar <= 180 && angle_femar > 168) {
        // 180 to 168 range
        digitalWrite(OUT_0, 0);
        digitalWrite(OUT_1, 0);
        digitalWrite(OUT_2, 0);  // OUT = 000
    }
    else if (angle_femar <= 168 && angle_femar > 156) {
        // 168 to 156 range
        digitalWrite(OUT_0, 1);
        digitalWrite(OUT_1, 0);
        digitalWrite(OUT_2, 0);  // OUT = 010
    }
    else if (angle_femar <= 156 && angle_femar > 144) {
        // 156 to 144 range
        digitalWrite(OUT_0, 0);
        digitalWrite(OUT_1, 1);
        digitalWrite(OUT_2, 0);  // OUT = 011
    }
    else if (angle_femar <= 144 && angle_femar > 132) {
        // 144 to 132 range
        digitalWrite(OUT_0, 1);
        digitalWrite(OUT_1, 1);
        digitalWrite(OUT_2, 0);  // OUT = 100
    }
    else if (angle_femar <= 132 && angle_femar > 120) {
        // 132 to 120 range
        digitalWrite(OUT_0, 0);
        digitalWrite(OUT_1, 0);
        digitalWrite(OUT_2, 1);  // OUT = 101
    }
    else if (angle_femar <= 120 && angle_femar > 108) {
        // 120 to 108 range
        digitalWrite(OUT_0, 1);
        digitalWrite(OUT_1, 0);
        digitalWrite(OUT_2, 1);  // OUT = 110
    }
    else if (angle_femar <= 108 && angle_femar > 96) {
        // 108 to 96 range
        digitalWrite(OUT_0, 0);
        digitalWrite(OUT_1, 1);
        digitalWrite(OUT_2, 1);  // OUT = 111
    }
    else if (angle_femar <= 96 && angle_femar > 90) {
        // 96 to 90 range
        digitalWrite(OUT_0, 1);
        digitalWrite(OUT_1, 1);
        digitalWrite(OUT_2, 1);  // OUT = 100
    }
}



//uint16_t decode_pos(int body_part, int axis) {
//    const float DEG_TO_RAD = M_PI / 180.0; // Conversion factor from degrees to radians

//    if (body_part == TOE) {
//        if (axis == X_AXIS) {
//            toe_x = heel_x - FOOT_LENGTH;
//            return toe_x;
//        } else if (axis == Y_AXIS) {
//            toe_y = MAX_DIMENSION - (0.5 * LINE_THICKNESS); // Flip Y-axis
//            return toe_y;
//        } else {
//            return -1; // default case
//        }
//    } else if (body_part == HEEL) {
//        if (axis == X_AXIS) {
//            heel_x = 0.5 * MAX_DIMENSION_HOR;
//            return heel_x;
//        } else if (axis == Y_AXIS) {
//            heel_y = MAX_DIMENSION - (0.5 * LINE_THICKNESS); // Flip Y-axis
//            return heel_y;
//        } else {
//            return -1; // default case
//        }
//    } else if (body_part == KNEE) {
//        if (axis == X_AXIS) {
//            heel2knee_x = sin((90 - theta_ankle) * DEG_TO_RAD) * SHIN_LENGTH;
//            knee_x = heel_x - heel2knee_x;
//            return knee_x;
//        } else if (axis == Y_AXIS) {
//            heel2knee_y = cos((90 - theta_ankle) * DEG_TO_RAD) * SHIN_LENGTH;
//            knee_y = heel_y + heel2knee_y;
//            if (knee_y > MAX_DIMENSION) knee_y = MAX_DIMENSION; // Clamp to bounds
//            knee_y = MAX_DIMENSION - knee_y; // Flip Y-axis
//            return knee_y;
//        } else {
//            return -1; // default case
//        }
//    } else if (body_part == HIP) {
//        if (axis == X_AXIS) {
//            knee2hip_x = sin((180 - (90 - theta_ankle) - theta_knee) * DEG_TO_RAD) * FEMAR_LENGTH;
//            hip_x = knee_x + knee2hip_x;
//            return hip_x;
//        } else if (axis == Y_AXIS) {
//            knee2hip_y = cos((180 - (90 - theta_ankle) - theta_knee) * DEG_TO_RAD) * FEMAR_LENGTH;
//            hip_y = knee_y + knee2hip_y;
//            if (hip_y > MAX_DIMENSION) hip_y = MAX_DIMENSION; // Clamp to bounds
//            hip_y = MAX_DIMENSION - hip_y; // Flip Y-axis
//            return hip_y;
//        } else {
//            return -1; // default case
//        }
//    } else if (body_part == HEAD) {
//        if (axis == X_AXIS) {
//            hip2head_x = sin((180 - ((180 - (90 - theta_ankle) - theta_knee)) - theta_hip) * DEG_TO_RAD) * TORSO_LENGTH;
//            head_x = hip_x - hip2head_x;
//            return head_x;
//        } else if (axis == Y_AXIS) {
//            hip2head_y = cos((180 - ((180 - (90 - theta_ankle) - theta_knee)) - theta_hip) * DEG_TO_RAD) * TORSO_LENGTH;
//            head_y = hip_y + hip2head_y;
//            if (head_y > MAX_DIMENSION) head_y = MAX_DIMENSION; // Clamp to bounds
//            head_y = MAX_DIMENSION - head_y; // Flip Y-axis
//            return head_y;
//        } else {
//            return -1; // default case
//        }
//    }

//    // Explicit default case for invalid `body_part`
//    return -1;
//}