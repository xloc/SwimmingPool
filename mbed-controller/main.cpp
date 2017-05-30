#include "mbed.h"

#include "imu.h"
#include "latch.h"
#include "distance.h"
#include "motor.h"

Serial pc(USBTX, USBRX); // tx, rx


#define BUFFER_SIZE 100
#define RESPONSE_BUFFER_SIZE 100

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

#define WIRELESS_RECEIVED 2
#define WIRELESS_NOT_RECEIVED 3

Serial wireless_uart(p13, p14); //tx rx

/**
 *
 */
void wireless_send(char *message){
    wireless_uart.printf(message);
}

int wireless_get(char *p_message){
    if(wireless_uart.readable()){
        int i = 0;
        // While data avaliable and rx buffer not full
        // "i" should in range [0, SIZE-1], so i < SIZE is the condition
        // and it should be a space for '\0' so i < SIZE - 1
        while(wireless_uart.readable() && i < RESPONSE_BUFFER_SIZE - 1){
            p_message[i++] = wireless_uart.getc();
        }
        // Append '\0'
        p_message[i] = '\0';

        return WIRELESS_RECEIVED;
    }else{
        return WIRELESS_NOT_RECEIVED;
    }
}


/**
 All supported commands:
 * g: gyroscope yaw.
    "g"
    return angle in float
 * u: ultrasonic ranging.
    "u?" (? is integer id)
    return distance in milimeter
    return "!DISTANCE" if distance invalid
    return "!ID" if id invalid

 * m: control speed.
    "m? ? ? ?" (? is float in range [-1,1])
    return loop backed speeds in form "%.2f,%.2f,%.2f,%.2f"

 * c: test acknowledge.
    "c"
    return "RECEIVED"

 * t: loop test. return what sent back
    "t?" (? is string that should loop back)
    return ?

 * w: wireless send.
    "w?" (? is message)
    return "SEND_OK"

 * r: wireless read.
    "r"
    return received message

 * a: 3-axes accelerometer
    "a"
    return "?,?,?"
        where ?s are head, right and down accelerometer readings in unit g
 */
void response(){
    char rdata[RESPONSE_BUFFER_SIZE] = {0};
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
        // sprintf(rdata, "%.2f,%.2f,%.2f,%.2f", q,a,w,s);
        reply("OK");
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
        for(char *origin=(buffer+2); *origin!='\0'; origin++, dupli++){
            *dupli = *origin;
        }
        *dupli = '\0';
        reply(rdata);
    }else if(buffer[1] == 'w'){
    // Wireless Send
        // Prepare message:
        // Copy data segment to rdata
        char *dupli = rdata;
        for(char *origin=(buffer+2); *origin!='\0'; origin++, dupli++){
            *dupli = *origin;
        }
        // Append '\0' after all
        *dupli = '\0';

        // Send message througe wireless_uart
        wireless_send(rdata);
        reply("SEND_OK");
    }else if(buffer[1] == 'r'){
        int status = wireless_get(rdata);
        if(status == WIRELESS_RECEIVED){
            reply(rdata);
        }else{
            reply("!NO_DATA");
        }
    }else if(buffer[1] == 'a'){
    // Request acceleration of 3 axis
        sprintf(rdata, "%.3f,%.3f,%.3f", a_x, a_y, a_z);
        reply(rdata);
    }
}




int main(){
    init_latch();
    init_gyro();
    init_motor();

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
