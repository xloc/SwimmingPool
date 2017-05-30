#include "latch.h"

// BusInOut latched_bus(p5, p6, p7, p8, p11, p12, p17, p18);
BusInOut latched_bus(
    LATCH_PIN0, LATCH_PIN1, LATCH_PIN2, LATCH_PIN3,
    LATCH_PIN4, LATCH_PIN5, LATCH_PIN6, LATCH_PIN7
);

// DigitalOut latch_enable_o1(p26);
// DigitalOut latch_enable_o2(p30);
// DigitalOut latch_enable_i1(p29);
DigitalOut latch_enable_o1(LATCH_ENABLE_OUTPUT_PIN1);
DigitalOut latch_enable_o2(LATCH_ENABLE_OUTPUT_PIN2);
DigitalOut latch_enable_i1(LATCH_ENABLE_INPUT_PIN1);

void init_latch(){
    // latched_bus.mode(OpenDrain);
    // latched_bus.mode(PullUp);

	// Disable all latched IOs
    latch_enable_i1 = LATCH_INPUT_DISABLE;

    latch_enable_o1 = LATCH_OUTPUT_DISABLE;
    latch_enable_o2 = LATCH_OUTPUT_DISABLE;

    // Initialize output of output latches
    ENABLE_OUTPUT_LATCH(latch_enable_o2);
    latched_bus = 0x00;
    DISABLE_OUTPUT_LATCH(latch_enable_o2);

    ENABLE_OUTPUT_LATCH(latch_enable_o1);
    latched_bus = 0x00;
    DISABLE_OUTPUT_LATCH(latch_enable_o1);
}
