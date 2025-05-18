/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2025-04-15 00:13:36
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2025-04-19 17:08:02
 * @FilePath: \MDK-ARM\User\Scheduler.c
 * @Description: 
 * 
 * Copyright (c) 2025 by ${git_name_email}, All Rights Reserved. 
 */
#include "Scheduler.h"
#include "event_groups.h"
#include "semphr.h"
#include "work.h"
#include "Buzzer.h"
#include "driver_passive_buzzer.h"

char task_info[512];
void FreeRTOS_Start(void);
void createTask(void);
void printTask(void);
void initTask(void);

extern bool suspend;
TaskHandle_t createTask_handle;
TaskHandle_t initTask_handle;
#define initTaskPriority 4
#define initTaskStackDepth 512
TaskHandle_t initTask_handle;

#define dht11TaskPriority 4
#define dht11TaskStackDepth 128
TaskHandle_t dht11Task_handle;

#define OLEDTaskPriority 4
#define OLEDTaskStackDepth 128
TaskHandle_t OLEDTask_handle;

#define printTaskPriority 3
#define printTaskStackDepth 128
TaskHandle_t printTask_handle;

#define tofTaskPriority 4
#define tofTaskStackDepth 128
TaskHandle_t tofTask_handle;

#define snrTaskPriority 5
#define snrTaskStackDepth 128
TaskHandle_t snrTask_handle;

#define photosensorTaskPriority 4
#define photosensorTaskStackDepth 128
TaskHandle_t photosensorTask_handle;

#define esp8266RecTaskPriority 5
#define esp8266RecTaskStackDepth 384
TaskHandle_t esp8266RecTask_handle;

#define esp8266SendTaskPriority 4
#define esp8266SendTaskStackDepth 256
TaskHandle_t esp8266SendTask_handle;

#define esp8266RecoverTaskPriority 3
#define esp8266RecoverTaskStackDepth 256
TaskHandle_t esp8266RecoverTask_handle;

// #define playTaskPriority 
// #define playTaskStackDepth 64
// TaskHandle_t playTask_handle;

#define scanTaskPriority 4
#define scanTaskStackDepth 64
TaskHandle_t scanTask_handle;

QueueHandle_t snrBinSemaphoreHandle;
QueueHandle_t espBinSemaphoreHandle;
QueueHandle_t espinitBinSemaphoreHandle;
QueueHandle_t sendMsgSemaphoreHandle;
EventGroupHandle_t eventGroup;
QueueHandle_t xESP8266MsgQueue;
QueueHandle_t recToSednQueue;

#define ESP8266_RX_BUF_SIZE 256

void FreeRTOS_Start(void)
{

   taskENTER_CRITICAL();
   sendMsgSemaphoreHandle=xSemaphoreCreateBinary();
   if (sendMsgSemaphoreHandle == NULL)
   {
      printf("������Ϣ��ֵ�ź�������ʧ��\r\n");
   }
   
   xESP8266MsgQueue = xQueueCreate(5,ESP8266_RX_BUF_SIZE);
   
   if(xESP8266MsgQueue == NULL)
   {
         printf("���ڶ��д���ʧ��\r\n");
   }
   recToSednQueue=xQueueCreate(1,ESP8266_RX_BUF_SIZE);
    if(xESP8266MsgQueue == NULL)
   {
         printf("rec to send���д���ʧ��\r\n");
   }
    snrBinSemaphoreHandle=xSemaphoreCreateBinary();
    
    if(snrBinSemaphoreHandle == NULL)
    {
      printf("snr��ֵ�ź�������ʧ��\r\n");
    }

    espBinSemaphoreHandle=xSemaphoreCreateBinary();
    if(espBinSemaphoreHandle == NULL)
    {
    printf("esp��ֵ�ź�������ʧ��\r\n");
    }

// espinitBinSemaphoreHandle=xSemaphoreCreateBinary();
 
//     if(espinitBinSemaphoreHandle ==NULL)
//     {
//        printf("��ʼ���ź�������ʧ��\r\n");
//     }

   // eventGroup=xEventGroupCreate();
// if(eventGroup == NULL)
//   {
//     printf("�¼��鴴��ʧ��\r\n");
//   }


   
    xTaskCreate((TaskFunction_t) initTask,
                            (const char * const) "initTask", /*lint !e971 Unqualified char types are allowed for strings and single characters only. */
                            (const configSTACK_DEPTH_TYPE) initTaskStackDepth,
                            (void * const) NULL,
                            (UBaseType_t) initTaskPriority,
                            (TaskHandle_t * const) &initTask_handle);

    
    
   
  
     vTaskStartScheduler();
  taskEXIT_CRITICAL();
  
}

void initTask(void)
{
  printf("start init\r\n");
  System_init();
  xTaskCreate((TaskFunction_t) createTask,
                            (const char * const) "createTask", /*lint !e971 Unqualified char types are allowed for strings and single characters only. */
                            (const configSTACK_DEPTH_TYPE) 256,
                            (void * const) NULL,
                            (UBaseType_t) 1,
                            (TaskHandle_t * const) &createTask_handle);
  vTaskDelete(NULL);
}

void createTask(void)
{

while (1)
{
   /* code */
   
  taskENTER_CRITICAL();

   //  ����SNR�ź���
//  xTaskCreate((TaskFunction_t) DHT11_Work,
//                             (const char * const) "DHT11_Work", /*lint !e971 Unqualified char types are allowed for strings and single characters only. */
//                             (const configSTACK_DEPTH_TYPE) dht11TaskStackDepth,
//                             (void * const) NULL,
//                             (UBaseType_t) dht11TaskPriority,
//                             (TaskHandle_t * const) &dht11Task_handle);




  
   if (xTaskCreate(DHT11_Work, "DHT11_Work", dht11TaskStackDepth, NULL, dht11TaskPriority, &dht11Task_handle) != pdPASS) {
      printf("DHT11_Work ���񴴽�ʧ��\r\n");
   }

   if (xTaskCreate(OLED_Work, "OLED_Work", OLEDTaskStackDepth, NULL, OLEDTaskPriority, &OLEDTask_handle) != pdPASS) {
      printf("OLED_Work ���񴴽�ʧ��\r\n");
   }

   if (xTaskCreate(TOF_Work, "TOF_Work", tofTaskStackDepth, NULL, tofTaskPriority, &tofTask_handle) != pdPASS) {
      printf("TOF_Work ���񴴽�ʧ��\r\n");
   }

   if (xTaskCreate(SNR_Work, "SNR_Work", snrTaskStackDepth, NULL, snrTaskPriority, &snrTask_handle) != pdPASS) {
      printf("SNR_Work ���񴴽�ʧ��\r\n");
   }

   if (xTaskCreate(PhotoSensor_Work, "PhotoSensor_Work", photosensorTaskStackDepth, NULL, photosensorTaskPriority, &photosensorTask_handle) != pdPASS) {
      printf("PhotoSensor_Work ���񴴽�ʧ��\r\n");
   }

   if (xTaskCreate(Esp8266_Rec_Work, "Esp8266_Rec_Work", esp8266RecTaskStackDepth, NULL, esp8266RecTaskPriority, &esp8266RecTask_handle) != pdPASS) {
      printf("Esp8266_Rec_Work ���񴴽�ʧ��\r\n");
   }

   if (xTaskCreate(Esp8266_Send_Work, "Esp8266_Send_Work", esp8266SendTaskStackDepth, NULL, esp8266SendTaskPriority, &esp8266SendTask_handle) != pdPASS) {
      printf("Esp8266_Send_Work ���񴴽�ʧ��\r\n");
   }

   if (xTaskCreate(ESP8266_Recover_Work, "ESP8266_Recover_Work", esp8266RecoverTaskStackDepth, NULL, esp8266RecTaskPriority, &esp8266RecoverTask_handle) != pdPASS) {
      printf("ESP8266_Recover_Work ���񴴽�ʧ��\r\n");
   }

  
   // if (xTaskCreate(printTask, "printTask", printTaskStackDepth, NULL, printTaskPriority, &printTask_handle) != pdPASS) {
   //    printf("printTask ���񴴽�ʧ��\r\n");
   // }

 // if (xTaskCreate(PlayMusic, "PlayMusic", playTaskStackDepth, NULL, playTaskPriority, &playTask_handle) != pdPASS) {
   //    printf("PlayMusic ���񴴽�ʧ��\r\n");
   // } else {
   //    vTaskSuspend(playTask_handle);
   //    suspend = 1;
   // }

      if(xTaskCreate((TaskFunction_t)Scan_Key,
                            (const char * const) "Scan_Key", /*lint !e971 Unqualified char types are allowed for strings and single characters only. */
                            (const configSTACK_DEPTH_TYPE)scanTaskStackDepth,
                            (void * const) NULL,
                            (UBaseType_t) scanTaskPriority,
                            (TaskHandle_t * const) &scanTask_handle) != pdPASS)
                            {
                                printf("failed to create Scan_Key task\r\n");
                            }

                            printf("Free heap size: %u\n", xPortGetFreeHeapSize());
     vTaskDelete(NULL);
  taskEXIT_CRITICAL();
}
 
}




void printTask(void)
{
  
   
    while (1)
    {
    vTaskList(task_info);
    printf("%s \r\n",task_info);
     HAL_GPIO_TogglePin(LED1_GPIO_Port,LED1_Pin);
   printf("Free heap size: %u\n", xPortGetFreeHeapSize());
    vTaskDelay(pdMS_TO_TICKS(2000));
    }
    

}

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
    printf("task stack overflow%s\r\n", pcTaskName);
    // ���������������߹���
    while(1); 
}