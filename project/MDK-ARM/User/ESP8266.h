/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2025-03-25 20:32:25
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2025-04-18 16:38:20
 * @FilePath: \MDK-ARM\User\ESP8266.h
 * @Description: 
 * 
 * Copyright (c) 2025 by ${git_name_email}, All Rights Reserved. 
 */
#ifndef __ESP8266__H
#define __ESP8266__H

#include "main.h"


extern u8 Esp8266_Buff[256];

#define ESP8266_Usart_Handle huart2

#define WIFI_SSID        "szh"
#define WIFI_PASSWD      "19990405"

#define MQTT_CLIENT_ID   "szh"   
#define MQTT_USER_NAME   "stm32&k1vy86hCdDq"
#define MQTT_PASSWD      "473D40822BA1A2B507108EC30C6B10017CB60F4A"
#define BROKER_ADDRESS  "114.96.79.191"
#define SUB_TOPIC        "Android"
#define PUB_TOPIC        "STM32"



bool Esp8266_Receive_Msg(void);
void Esp8266_Init(void);
bool Esp8266_Pub_Msg(const char* str);
extern char String_Buff[64];
extern u8 step;
#endif



