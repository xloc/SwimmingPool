#include "distance.h"

Timer echo_duration;

int TRIGGER_ID_PINMAP[] = {
//  0  1  2  3  4
    3, 4, 5, 6, 7
};

int ECHO_ID_PINMAP[] = {
//  0  1  2  3  4
    7, 6, 5, 4, 3
};


int get_distance(int id){
    char info[30];

    int trigger_pin = TRIGGER_ID_PINMAP[id];
    ENABLE_OUTPUT_LATCH(latch_enable_o2);
    // Send measure pulse to ultrasonic sensor
    latched_bus[trigger_pin] = 0;
    wait_us(5);
    latched_bus[trigger_pin] = 1;
    wait_us(10);
    latched_bus[trigger_pin] = 0;
    DISABLE_OUTPUT_LATCH(latch_enable_o2);


    int echo_pin = ECHO_ID_PINMAP[id];
    ENABLE_INPUT_LATCH(latch_enable_i1);
    // Measure echo pulse width
    echo_duration.stop();
    echo_duration.reset();
    echo_duration.start();
    while(!latched_bus[echo_pin]){
        if(echo_duration.read_ms() > 5){
            DISABLE_INPUT_LATCH(latch_enable_i1);
            return -1;
        }
    }
    echo_duration.reset();
    while(latched_bus[echo_pin]){
        if(echo_duration.read_ms() > 40){
            DISABLE_INPUT_LATCH(latch_enable_i1);
            return -1;
        }
    }
    echo_duration.stop();
    float distance = (echo_duration.read_us()/2.0) * 0.34;

    DISABLE_INPUT_LATCH(latch_enable_i1);

    return distance;
}
