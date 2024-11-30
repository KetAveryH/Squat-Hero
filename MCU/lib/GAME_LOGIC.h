// LSM6DSO32 IMU

#ifndef GAME_H
#define GAME_H

#include <stdint.h> // Include stdint header

///////////////////////////////////////////////////////////////////////////////
// Definitions
///////////////////////////////////////////////////////////////////////////////
// shin = 0.8*femar length / total leg length = torso length
// basing this on a 10'b X 10'b array per request of Ket for the VGA display logic
#define MAX_DIMENSION 1023
#define FOOT_LENGTH ((uint16_t)(0.075 * MAX_DIMENSION))
#define SHIN_LENGTH ((uint16_t)(0.22 * MAX_DIMENSION))
#define FEMAR_LENGTH ((uint16_t)(0.28 * MAX_DIMENSION))
#define TORSO_LENGTH ((uint16_t)(0.5 * MAX_DIMENSION))
#define LINE_THICKNESS 12

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

///////////////////////////////////////////////////////////////////////////////
// Function prototypes
///////////////////////////////////////////////////////////////////////////////

int16_t twoComplement2Signed(uint16_t raw_value);
int16_t decode_angle(int body_part, int16_t accel_x, int16_t accel_y, int16_t accel_z);
uint16_t decode_pos(int body_part, int axis);

#endif