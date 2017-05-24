#include "mbed.h"

#include "gyroscope.h"
#include "latch.h"
#include "distance.h"

Serial pc(USBTX, USBRX); // tx, rx


#define BUFFER_SIZE 100

#define FLAG_RECEIVING_BODY 1
#define FLAG_HEAD_FINDING 0
#define FLAG_RECEIVING_CHECKSUM1 2
#define FLAG_RECEIVING_CHECKSUM2 3

char buffer[BUFFER_SIZE] = {0};
int iBufferW = 0;
char flag = FLAG_HEAD_FINDING;

#define WARNING_REPEATED_START_HEAD_FINDING 0


void warning(int warning_num){
    if(warning_num == WARNING_REPEATED_START_HEAD_FINDING){

    }

}


char hex2nibble(char c){
    if(c >= 'a' && c <= 'f'){
        return c - 'a' + 10;
    }else if(c >= '0' && c <= '9'){
        return c - '0';
    }else{
        return -1;
    }
}

bool validateChecksum(){

    // Calculate sum
    uint8_t sum = 0;
    for(int i = 0; i<(iBufferW - 3); i++)
        // $ 1 2 3 # a b
        //               ^
        // So use index - 3
        sum += buffer[i];

    // Extract sum from last 2 hexadecimal
    uint8_t recv_sum = (
        (uint8_t)hex2nibble(buffer[iBufferW - 2]) << 4 |
        (uint8_t)hex2nibble(buffer[iBufferW - 1])
    );

    if(sum == recv_sum){
        return true;
    }else{
        return false;
    }
}

const char HEX_LOOKUP_TABLE[] = "0123456789abcdef";
void reply(char *message){
    // Calculate char sum of message
    char *input = message;
    uint8_t sum = '+' + '$';
    for(; (*input)!='\0' ; input++){
        sum += *input;
    }

    static uint8_t sum_s[3] = {0};
    sum_s[0] = HEX_LOOKUP_TABLE[ sum>>4 & 0x0F ];
    sum_s[1] = HEX_LOOKUP_TABLE[ sum & 0x0F ];

    pc.printf("+$%s#%s", message, sum_s);
}

#define MOTOR_Q_PWMPIN p24
#define MOTOR_A_PWMPIN p23
#define MOTOR_W_PWMPIN p22
#define MOTOR_S_PWMPIN p21

#define MOTOR_Q_REVERSE
#define MOTOR_A_REVERSE
#define MOTOR_W_REVERSE
#define MOTOR_S_REVERSE


int MOTOR_DIR_PINMAP[4][2] = {
// Motor Q
{6, 7},
// Motor A
{4, 5},
// Motor W
{3, 2},
// Motor S
{1, 0}
};

PwmOut motor_pwm[] = {
    PwmOut(MOTOR_Q_PWMPIN),PwmOut(MOTOR_A_PWMPIN),
    PwmOut(MOTOR_W_PWMPIN),PwmOut(MOTOR_S_PWMPIN)
};

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
}

void response(){
    char rdata[50] = {0};
    if(buffer[1]=='g'){
    // Request gyroscope angle
        sprintf(rdata, "%.2f", yaw);
        reply(rdata);
    }else if(buffer[1] == 'u'){
    // Request ultrasonic sensor data
        int sensor_id = buffer[2] - '0';
        if(sensor_id >= 0 && sensor_id <= 9){
            int distance = get_distance(sensor_id);
            if(distance != -1){
                sprintf(rdata, "%d", distance);
                reply(rdata);
            }else{
                reply("!DISTANCE");
            }

        }else{
            reply("!ID");
        }

    }else if(buffer[1] == 'm'){
    // Test 74HC573
        float q,a,w,s;
        sscanf(buffer+2, "%f %f %f %f", &q, &a, &w, &s);
        set_speeds(q,a,w,s);
        // if(buffer[2] == '0'){
        //     latch_enable_o2 = LATCH_OUTPUT_ENABLE;
        //     latched_bus.output();
        //     latched_bus[0] = 0;
        //     latch_enable_o2 = LATCH_OUTPUT_DISABLE;
        // }else if(buffer[2] == '1'){
        //     latch_enable_o2 = LATCH_OUTPUT_ENABLE;
        //     latched_bus.output();
        //     latched_bus[0] = 1;
        //     latch_enable_o2 = LATCH_OUTPUT_DISABLE;
        // }
        sprintf(rdata, "%.2f,%.2f,%.2f,%.2f", q,a,w,s);
        reply(rdata);
    // }else if(buffer[1] == '0'){
    //     latched_bus[6] = !latched_bus[6];
    //     reply("OK");
    // }else if(buffer[1] == '9'){
    //     latched_bus[7] = !latched_bus[7];
    //     reply("OK");
    }
    else if(buffer[1]=='c'){
    // Test Acknowledge
        reply("RECEIVED");
    }else if(buffer[1] == 't'){
    // Test Loop
        char *dupli = rdata;
        for(char *origin=(buffer+2);
            *origin!='#'; origin++, dupli++){
            *dupli = *origin;
        }
        *dupli = '\0';
        reply(rdata);
    }
}




int main(){
    init_latch();
    init_gyro();

    while(1){
        char c = pc.getc();

        if(c == '$'){
        // Case: FLAG_HEAD_FINDING
            iBufferW = 0;
            buffer[iBufferW++] = c;
            if(flag != FLAG_HEAD_FINDING){
                warning(WARNING_REPEATED_START_HEAD_FINDING);
            }
            flag = FLAG_RECEIVING_BODY;

        }else if(flag == FLAG_RECEIVING_BODY){
        // Case: FLAG_RECEIVING_BODY
            if(c == '#'){
            // Char '#'' means start of checksum
                buffer[iBufferW++] = c;
                flag = FLAG_RECEIVING_CHECKSUM1;
            }else{
            // Or it is just part of body
                buffer[iBufferW++] = c;
            }
        }else if(flag == FLAG_RECEIVING_CHECKSUM1){
        // Case: FLAG_RECEIVING_CHECKSUM1
            buffer[iBufferW++] = c;
            flag = FLAG_RECEIVING_CHECKSUM2;
        }else if(flag == FLAG_RECEIVING_CHECKSUM2){
        // Case: FLAG_RECEIVING_CHECKSUM2
            buffer[iBufferW++] = c;

            if(validateChecksum()){
                buffer[iBufferW - 3] = '\0';
                response();
            }else{
                pc.printf("+$!SUM#65");
            }

            iBufferW = 0;
            flag = FLAG_HEAD_FINDING;
        }
    }
}
