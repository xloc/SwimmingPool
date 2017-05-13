#include "latch.h"

BusInOut latched_bus(p5, p6, p7, p8, p11, p12, p17, p18);

DigitalOut latch_enable_o2(p30);
DigitalOut latch_enable_i1(p29);

void init_latch(){
    // latched_bus.mode(OpenDrain);
    // latched_bus.mode(PullUp);

    latch_enable_i1 = LATCH_INPUT_DISABLE;
    latch_enable_o2 = LATCH_OUTPUT_DISABLE;

    ENABLE_OUTPUT_LATCH(latch_enable_o2);
    latched_bus = 0x00;
    DISABLE_OUTPUT_LATCH(latch_enable_o2);
}