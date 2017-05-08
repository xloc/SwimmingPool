#include "mbed.h"
#include "gyroscope.h"

Serial pc(USBTX, USBRX); // tx, rx


int main(){
	init_gyro();
	pc.printf("Start...\n");
	while(1){
		pc.getc();
		pc.printf("%.2f\n", yaw);

		// wait(1);
	}
}