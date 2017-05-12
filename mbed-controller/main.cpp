#include "mbed.h"
#include "gyroscope.h"
#include "latch.h"

Serial pc(USBTX, USBRX); // tx, rx

Timer echo_duration;


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





void response(){
    char rdata[20] = {0};
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
        if(buffer[2] == '0'){
            latch_enable_o2 = LATCH_OUTPUT_ENABLE;
            latched_bus.output();
            latched_bus[0] = 0;
            latch_enable_o2 = LATCH_OUTPUT_DISABLE;
        }else if(buffer[2] == '1'){
            latch_enable_o2 = LATCH_OUTPUT_ENABLE;
            latched_bus.output();
            latched_bus[0] = 1;
            latch_enable_o2 = LATCH_OUTPUT_DISABLE;
        }
        reply("OK");
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
                response();
            }else{
                pc.printf("+$!SUM#65");
            }

            iBufferW = 0;
            flag = FLAG_HEAD_FINDING;
        }
    }
}