#ifndef WIRELESS_H
#define WIRELESS_H

#include "mbed.h"
#include "config.h"


extern Serial wireless_uart;

#define WIRELESS_RECEIVED 2
#define WIRELESS_NOT_RECEIVED 3

void wireless_send(char *message);
int wireless_get(char *p_message);

#endif
