// LSM6DSO32 IMU

#ifndef GAME_H
#define GAME_H

#include <stdint.h> // Include stdint header

///////////////////////////////////////////////////////////////////////////////
// Definitions
///////////////////////////////////////////////////////////////////////////////
// shin = 0.8*femar length / total leg length = torso length
// basing this on a 10'b X 10'b array per request of Ket for the VGA display logic
#define MAX_DIMENSION 480
#define BUFFER_DIMENSION 440
#define MAX_DIMENSION_HOR 640
#define FOOT_LENGTH ((uint16_t)(0.075 * BUFFER_DIMENSION))
#define SHIN_LENGTH ((uint16_t)(0.22 * BUFFER_DIMENSION))
#define FEMAR_LENGTH ((uint16_t)(0.28 * BUFFER_DIMENSION))
#define TORSO_LENGTH ((uint16_t)(0.5 * BUFFER_DIMENSION))
#define LINE_THICKNESS 12

#define OUT_0 PA0
#define OUT_1 PB3
#define OUT_2 PB5
#define OUT_3 PA5

#define TOE    0
#define HEEL   1
#define ANKLE  2
#define SHIN   3
#define KNEE   4
#define FEMAR  5
#define HIP    6
#define TORSO  7
#define HEAD   8
#define X_AXIS 9
#define Y_AXIS 10
#define Z_AXIS 11
#define FEMAR2FLOOR 13


///////////////////////////////////////////////////////////////////////////////
// Function prototypes
///////////////////////////////////////////////////////////////////////////////

int16_t decode_angle(int body_part, int16_t accel_x, int16_t accel_y, int16_t accel_z);
uint16_t decode_pos(int body_part, int axis);
void delay_ms(uint32_t ms);
uint16_t decode_pos_top_left(int body_part, int axis);

#endif