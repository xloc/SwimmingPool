#include "gyroscope.h"

Serial device(p28, p27);  // tx, rx

char gyroBuffer[20] = {0};
uint8_t gwi = 0;

float yaw = 0;

extern Serial pc;

int findPackHead(){
    for(int i=0; i<=10; i++){
        if(gyroBuffer[i] == 0x55){
            return i;
        }
    }
    return -1;
}

char checkSum(char *d){
    char sum = 0;
    for(int i=0; i<=10-1; i++){
        sum += d[i];
    }
    return sum == d[10];
}

void parseData(char* d){
	static float lastYaw = 0;
	static int turnNum = 0;
    if(d[1] == 0x53){
        double yaw_;
        yaw_ = d[7] << 8 | d[6];
        yaw_ = yaw_ / 32768.0 * 180;

        // Add multi-turn recognition
        if(lastYaw < 90 && yaw_ > 270){turnNum -= 1;}
        else if(lastYaw > 270 && yaw_ < 90){turnNum += 1;}
        lastYaw = yaw_;
        yaw = yaw_ + 360*turnNum;
        //////////////////

        // pc.printf("      %8.2f\n", yaw_+360*turnNum);
    }
}

void d(int i){
    pc.printf("debug %d\n", i);
}

void gyroHandler(){
    // pc.putc(device.getc());
    #define LAST_IS_RIGHT 1
    #define LAST_IS_WRONG 2
    static int flag = LAST_IS_WRONG;

    // if(!device.readable()) return;

    if(flag == LAST_IS_WRONG){
        if(gwi < 11){
            gyroBuffer[gwi] = device.getc();
            gwi++;
        }else{
            int idx = findPackHead();
            if(idx == -1){
                gwi = 0;
                return;
            }else if(idx > 0){
                for(int i=0,j=idx; i<=10; i++, j++){
                    gyroBuffer[i] = gyroBuffer[j];
                }
                gwi = 11 - idx;
                return;
            }
            if(checkSum(gyroBuffer)){
                parseData(gyroBuffer);
                flag = LAST_IS_RIGHT;
            }
            gwi = 0;
        }
    }else if(flag == LAST_IS_RIGHT){
        if(gwi < 11){
            gyroBuffer[gwi] = device.getc();
            gwi++;
        }else{
            if(checkSum(gyroBuffer)){
                parseData(gyroBuffer);
                gwi = 0;
            }else{
                flag = LAST_IS_WRONG;
            }
        }
    }
}

void init_gyro(){
    device.attach(&gyroHandler, device.RxIrq);
}
