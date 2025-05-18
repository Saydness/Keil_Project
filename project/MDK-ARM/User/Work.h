/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2025-03-24 15:02:26
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2025-04-19 15:45:13
 * @FilePath: \MDK-ARM\User\Work.h
 * @Description: 
 * 
 * Copyright (c) 2025 by ${git_name_email}, All Rights Reserved. 
 */
#ifndef __Work__H
#define __Work__H


#include "main.h"

//#include "vl53l0x.h"

typedef enum{
    Auto,
    WiFi,
    Speech
}Ctr_Mode_Type;
extern Ctr_Mode_Type Ctr_Mode;
extern char* Debug_Info_Type[];
extern char* Debug_Info_Obj[];
extern    bool Init_Success;


void System_init(void);
void DHT11_Work(void);
void OLED_Work(void);
void System_init(void);
void PhotoSensor_Work(void);
void TOF_Work(void);
void SNR_Work(void);
void Esp8266_Rec_Work(void);
void Esp8266_Send_Work(void);
void ESP8266_Recover_Work(void);
void Scan_Key(void);
#endif



