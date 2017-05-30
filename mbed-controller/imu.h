#ifndef GYROSCOPE_H
#define GYROSCOPE_H

#include "mbed.h"
#include "config.h"


extern float yaw;
extern float a_x, a_y, a_z;

void init_gyro();

#endif
