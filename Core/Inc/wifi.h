#ifndef SMARTLOCK_STM32_WIFI_H
#define SMARTLOCK_STM32_WIFI_H

#include "main.h"

void WIFI_Enable(void);

void WIFI_openServer(void);

void WIFI_Receive(void);

void WIFI_startReceiveIt(void);

void WIFI_receiveStatus();

void WIFI_receiveData();

void WIFI_sendStatus();

#endif //SMARTLOCK_STM32_WIFI_H
