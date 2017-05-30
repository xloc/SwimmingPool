#include "wireless.h"

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
