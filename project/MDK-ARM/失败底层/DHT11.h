#ifndef __DHT11__H
#define __DHT11__H


#include "main.h"


bool DHT11_Init(void);
bool DHT11_ReadData(uint8_t* Humi,float* Temp);

#endif



