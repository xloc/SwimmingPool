#include "distance.h"

Timer echo_duration;

int get_distance(int id){
    char info[30];

    ENABLE_OUTPUT_LATCH(latch_enable_o2);
    // Send measure pulse to ultrasonic sensor
    latched_bus[id] = 0;
    wait_us(5);
    latched_bus[id] = 1;
    wait_us(10);
    latched_bus[id] = 0;
    DISABLE_OUTPUT_LATCH(latch_enable_o2);


    ENABLE_INPUT_LATCH(latch_enable_i1);
    // Measure echo pulse width
    echo_duration.stop();
    echo_duration.reset();
    echo_duration.start();
    while(!latched_bus[id]){
        if(echo_duration.read_ms() > 5){
            DISABLE_INPUT_LATCH(latch_enable_i1);
            return -1;
        }
    }
    echo_duration.reset();
    while(latched_bus[id]){
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
