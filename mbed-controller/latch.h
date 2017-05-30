#ifndef LATCH_H
#define LATCH_H

#include "mbed.h"
#include "config.h"

extern BusInOut latched_bus;

extern DigitalOut latch_enable_o1;
extern DigitalOut latch_enable_o2;
extern DigitalOut latch_enable_i1;

#define LATCH_OUTPUT_ENABLE 1
#define LATCH_OUTPUT_DISABLE 0
#define LATCH_INPUT_ENABLE 0
#define LATCH_INPUT_DISABLE 1


#define ENABLE_OUTPUT_LATCH(PIN) do{latched_bus.output();PIN = LATCH_OUTPUT_ENABLE;}while(0)
#define DISABLE_OUTPUT_LATCH(PIN) do{PIN = LATCH_OUTPUT_DISABLE;}while(0)

#define ENABLE_INPUT_LATCH(PIN) do{latched_bus.input(); PIN = LATCH_INPUT_ENABLE;}while(0)
#define DISABLE_INPUT_LATCH(PIN) do{PIN = LATCH_INPUT_DISABLE;}while(0)

void init_latch();


#endif
