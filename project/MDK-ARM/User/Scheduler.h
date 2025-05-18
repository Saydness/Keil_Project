/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2025-04-15 00:13:52
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2025-04-19 09:09:50
 * @FilePath: \MDK-ARM\User\Scheduler.h
 * @Description: 
 * 
 * Copyright (c) 2025 by ${git_name_email}, All Rights Reserved. 
 */
#ifndef __SCHEDULER_H__
#define __SCHEDULER_H__


#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"
#include "queue.h"
void FreeRTOS_Start(void);
extern QueueHandle_t snrBinSemaphoreHandle;
extern QueueHandle_t espBinSemaphoreHandle;
extern QueueHandle_t xESP8266MsgQueue;

extern  TaskHandle_t esp8266RecoverTask_handle;
extern TaskHandle_t playTask_handle;
extern EventGroupHandle_t eventGroup;
extern  QueueHandle_t sendMsgSemaphoreHandle;
extern  TaskHandle_t esp8266RecTask_handle;
extern   TaskHandle_t esp8266SendTask_handle;
extern QueueHandle_t recToSednQueue;
 extern       TaskHandle_t OLEDTask_handle;
 extern TaskHandle_t dht11Task_handle;
#endif
