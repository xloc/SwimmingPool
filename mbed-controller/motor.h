#ifndef MOTOR_H
#define MOTOR_H

#include "mbed.h"
#include "latch.h"

void set_speeds(float q, float a, float w, float s);
void init_motor();

#endif
