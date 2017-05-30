#ifndef CONFIG_H

#define BUFFER_SIZE 100
#define RESPONSE_BUFFER_SIZE 100

// Distance pinmaps
#define DISTANCE_TRIGGER0_PINMAP 3
#define DISTANCE_TRIGGER1_PINMAP 4
#define DISTANCE_TRIGGER2_PINMAP 5
#define DISTANCE_TRIGGER3_PINMAP 6
#define DISTANCE_TRIGGER4_PINMAP 7

#define DISTANCE_ECHO0_PINMAP 7
#define DISTANCE_ECHO1_PINMAP 6
#define DISTANCE_ECHO2_PINMAP 5
#define DISTANCE_ECHO3_PINMAP 4
#define DISTANCE_ECHO4_PINMAP 3

// IMU pinmaps
#define IMU_TX_PINMAP p28
#define IMU_RX_PINMAP p27

// Latch pinmaps
#define LATCH_PIN0 p5
#define LATCH_PIN1 p6
#define LATCH_PIN2 p7
#define LATCH_PIN3 p8
#define LATCH_PIN4 p18
#define LATCH_PIN5 p17
#define LATCH_PIN6 p16
#define LATCH_PIN7 p15

#define LATCH_ENABLE_OUTPUT_PIN1 p30
#define LATCH_ENABLE_OUTPUT_PIN2 p29
#define LATCH_ENABLE_INPUT_PIN1  p12

// Motor pinmaps
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

// Wireless pinmaps
#define WIRELESS_TX_PINMAP p13
#define WIRELESS_RX_PINMAP p14



#define CONFIG_H
#endif
