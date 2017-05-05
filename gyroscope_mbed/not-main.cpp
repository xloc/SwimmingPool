/* First gyroscope program that correctly run */

#include "mbed.h"
 
Serial pc(USBTX, USBRX); // tx, rx
Serial device(p9, p10);  // tx, rx

#define SEARCH_FIND 0
#define SEARCH_CHECK 1


char gyroBuffer[2000] = {0};
char gidx = 0;
uint8_t search = SEARCH_FIND;

void p(char *d, int len=11){
	for(int i=0; i<len; i++)
		pc.printf("%2X ", d[i]);
	pc.putc('\n');
}

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

void parse_data(char* d){
    if(d[1] == 0x53){
        double yaw;
        yaw = d[7] << 8 | d[6];
        yaw = yaw / 32768.0 * 180;
        pc.printf("\r%8.2f", yaw);
        

    }
}
 
int main()
{
    pc.baud(9600);
    device.baud(9600);
    while(1) {
        for(int i=0; i<11; i++){
        	gyroBuffer[i] = device.getc();
        }
        // p(gyroBuffer, 20);

    	// Find pack head which is 0x55
    	int idx = findPackHead();

    	// p(gyroBuffer);
    	// pc.printf("idx %d\n", idx);

    	// If found a head
    	if(idx > 0){
    	// 	p(gyroBuffer);

    		// Move head and the rest to index 0
    		for(int i=0,j=idx; i<=10; i++, j++){
    			gyroBuffer[i] = gyroBuffer[j];
    		}
    		// p(gyroBuffer, 20);
    		
    		// Complete buffer
    		for(int i=0; i<idx; i++){
        		gyroBuffer[11-idx+i] = device.getc();
        	}

        	// p(gyroBuffer, 20);
        	// while(1);

        	// Check sum
    		while(checkSum(gyroBuffer)){
                // p(gyroBuffer);

                parse_data(gyroBuffer);


                for(int i=0; i<11; i++){
                    gyroBuffer[i] = device.getc();
                }
            }




    	}
    	// pc.putc('-');
    	
        
    }
}