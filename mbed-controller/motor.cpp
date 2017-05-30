#include "motor.h"

#define MOTOR_Q_PWMPIN p24
#define MOTOR_A_PWMPIN p23
#define MOTOR_W_PWMPIN p22
#define MOTOR_S_PWMPIN p21

// Add '-' to define, if reverse is needed
#define MOTOR_Q_REVERSE
#define MOTOR_A_REVERSE
#define MOTOR_W_REVERSE
#define MOTOR_S_REVERSE

// With the reference of latcher pin
#define MOTOR_Q_PIN1 6
#define MOTOR_Q_PIN2 7
#define MOTOR_A_PIN1 4
#define MOTOR_A_PIN2 5
#define MOTOR_W_PIN1 3
#define MOTOR_W_PIN2 2
#define MOTOR_S_PIN1 1
#define MOTOR_S_PIN2 0

#define WATCHDOG_COUNT_RELOAD 20
#define WATCHDOG_PERIOD 0.1


int MOTOR_DIR_PINMAP[4][2] = {
    {MOTOR_Q_PIN1, MOTOR_Q_PIN2}, // Motor Q
    {MOTOR_A_PIN1, MOTOR_A_PIN2}, // Motor A
    {MOTOR_W_PIN1, MOTOR_W_PIN2}, // Motor W
    {MOTOR_S_PIN1, MOTOR_S_PIN1}  // Motor S
};

PwmOut motor_pwm[] = {
    PwmOut(MOTOR_Q_PWMPIN),PwmOut(MOTOR_A_PWMPIN),
    PwmOut(MOTOR_W_PWMPIN),PwmOut(MOTOR_S_PWMPIN)
};

Ticker t;

int watchdog_counter = WATCHDOG_COUNT_RELOAD;

void watchdog(){
    if(watchdog_counter > 0){
        watchdog_counter--;
    }else{
        set_speeds(0, 0, 0, 0);
    }
}

void init_motor(){
    t.attach(&watchdog, WATCHDOG_PERIOD);
}

void set_speed(uint8_t motor_id, float speed, int8_t *p_speed){
    motor_pwm[motor_id] = speed>0 ? speed : -speed;
    if(speed > 0){
        if(*p_speed > 0){
        // Case: + speed required, + previous
            // already satisfied -> Do nothing
            return;
        }else{
        // Case: + speed required, - previous
            // Reverse direction flag & update dir output
        // Case: + speed required, no previous
            // Init direction flag & update dir output
            *p_speed = 1;
        }
        ENABLE_OUTPUT_LATCH(latch_enable_o1);
        latched_bus[ MOTOR_DIR_PINMAP[motor_id][0] ] = 1;
        latched_bus[ MOTOR_DIR_PINMAP[motor_id][1] ] = 0;
        DISABLE_OUTPUT_LATCH(latch_enable_o1);

    }else{
        if(*p_speed < 0){
            return;
        }else{
            *p_speed = -1;
        }
        ENABLE_OUTPUT_LATCH(latch_enable_o1);
        latched_bus[ MOTOR_DIR_PINMAP[motor_id][0] ] = 0;
        latched_bus[ MOTOR_DIR_PINMAP[motor_id][1] ] = 1;
        DISABLE_OUTPUT_LATCH(latch_enable_o1);
    }
}

void set_speeds(float q, float a, float w, float s){
    static int8_t pq, pa, pw, ps = 0;
    set_speed(0, MOTOR_Q_REVERSE q, &pq);
    set_speed(1, MOTOR_A_REVERSE a, &pa);
    set_speed(2, MOTOR_W_REVERSE w, &pw);
    set_speed(3, MOTOR_S_REVERSE s, &ps);
    watchdog_counter = WATCHDOG_COUNT_RELOAD;
}
