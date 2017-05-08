#include "mbed.h"
#include "gyroscope.h"

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
pc.printf("[!] warn repeated finding head\n");
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
	uint8_t sum = 0;
	for(int i = 0; i<(iBufferW - 3); i++){
		sum += buffer[i];
pc.printf("%x ", buffer[i]);
	}
pc.printf("\n    actual sum %2x\n", sum);
	uint8_t recv_sum = 
		(uint8_t)hex2nibble(buffer[iBufferW - 2]) << 4 | 
		(uint8_t)hex2nibble(buffer[iBufferW - 1]);
pc.printf("  expected sum %2x\n", recv_sum);

	if(sum == recv_sum){
		return true;
	}else{
		return false;
	}


}


int main(){
	init_gyro();
	pc.printf("Start...\n");
	while(1){
		char c = pc.getc();

		if(c == '$'){
		// if(flag == FLAG_HEAD_FINDING)
pc.printf("find head received %c, %2x\n", c, c);
			iBufferW = 0;
			buffer[iBufferW++] = c;
			if(flag != FLAG_HEAD_FINDING){
				warning(WARNING_REPEATED_START_HEAD_FINDING);
			}
			flag = FLAG_RECEIVING_BODY;
			
		}else if(flag == FLAG_RECEIVING_BODY){
pc.printf("in recving body %c, %2x\n", c, c);
			if(c == '#'){
			// Char '#'' means start of checksum
				buffer[iBufferW++] = c;
				flag = FLAG_RECEIVING_CHECKSUM1;
			}else{
			// Or it is just part of body
				buffer[iBufferW++] = c;
			}
		}else if(flag == FLAG_RECEIVING_CHECKSUM1){
pc.printf("in checksum 1 %c, %2x\n", c, c);
			buffer[iBufferW++] = c;
			flag = FLAG_RECEIVING_CHECKSUM2;
		}else if(flag == FLAG_RECEIVING_CHECKSUM2){
pc.printf("in checksum 2 %c, %2x\n", c, c);
			buffer[iBufferW++] = c;

			if(validateChecksum()){
				pc.printf("+$OK#e9");
			}else{
				pc.printf("+$!SUM#65 ");
			}

			iBufferW = 0;
			flag = FLAG_HEAD_FINDING;
		}

		// pc.printf("%.2f\n", yaw);

		// wait(1);
	}
}