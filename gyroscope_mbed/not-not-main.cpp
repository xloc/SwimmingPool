/* Solve the problem of intermediate 0x55 in pack
 * with neat program structure */

#include "mbed.h"
 
Serial pc(USBTX, USBRX); // tx, rx
Serial device(p9, p10);  // tx, rx

char gyroBuffer[20] = {0};
uint8_t gwi = 0;

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
    if(d[1] == 0x53){
        double yaw;
        yaw = d[7] << 8 | d[6];
        yaw = yaw / 32768.0 * 180;
        pc.printf("\r%8.2f", yaw);
    }
}

int main(){
    while(1){

        // Fill the buffer
        for(; gwi<11; gwi++){
            gyroBuffer[gwi] = device.getc();
        }

        // Find pack head
        int idx = findPackHead();
        
        if(idx == -1){
        // If no head detected
            // Need: refill buffer
            // Reset write index the very beginning
            gwi = 0;
            continue;
        }else if(idx == 0){
        // If head is already at index 0
        // Have fun
        }else if(idx > 0){
        // If head is not at index 0
            // Move head to 0
            for(int i=0,j=idx; i<=10; i++, j++){
                gyroBuffer[i] = gyroBuffer[j];
            }
            // Need: refill buffer
            // Reset write index to the tail
            gwi = 11 - idx;
            continue;
        }

        // If the transfered data is correct
        // Keep sample
        while(checkSum(gyroBuffer)){
            // Parse the data to yaw
            parseData(gyroBuffer);

            // Collect another group
            for(int i=0; i<11; i++){
                gyroBuffer[i] = device.getc();
            }
        }
        // else: find pack head again

        // Need: find head
        // Let the route move on.
        // The gwi is at the last, so the buffer will not be refilled
        // As a result, the process will of course find the head

    }



}