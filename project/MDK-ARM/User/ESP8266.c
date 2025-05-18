#include "ESP8266.h"
#include "core_json.h"
#include <errno.h>
#include "Scheduler.h"
#include "semphr.h"
#include "usart.h"
#include "work.h"
#include "Peripheral.h"
#include "OLED.h"
#include "stdarg.h"

#define RESPONSE_RIGHT "OK"
#define RETRY_WAIT 1000
 // 0��ʾû���յ�  1��ʾ���յ���???
extern bool Rec_CMD_Flag;

u8 Esp8266_Buff[256];
char String_Buff[64];
 char buff[50];
u8 step=0;
extern bool Led_Status;
extern u8 Humi;
extern float Temp;
extern u8 MotorSpeed;
u8 info[512];
bool Esp8266_Send_Cmd(char* cmd, char* cmd_response,uint16_t delay);
void Esp8266_Init(void);

bool Esp8266_Pub_Msg(const char* str);
bool Parse_Json_Msg(uint8_t *json_msg,uint8_t json_len);
void Wait_ESP8266_Ready(void); 
/*----------  ?????????-----------  */
//void SetMotorSpeed(u8 speed);
//void SetLedStatus(bool status);
//void SetServoAngle(int angle);
/*----------  ?????????-----------  */




/* ------  3-27   --------- 

	?????????????????????????????????????Android???

*/

	


bool Esp8266_Pub_Msg(const char* str)
{
	
    bool retval = 0;
    u8 retry = 0;
    sprintf(String_Buff,"AT+MQTTPUB=0,\"%s\",\"%s\",0,0\r\n", PUB_TOPIC, str);
	// printf("pub:%s\r\n",String_Buff);
    HAL_UART_Transmit(&huart2,String_Buff,strlen(String_Buff),0xffff);
	memset(String_Buff,0,sizeof(String_Buff));
	// vTaskDelay(3000);
    if(Init_Success)
    {
       if(xQueueReceive(recToSednQueue, String_Buff, portMAX_DELAY) == pdPASS) 
       {
            // printf("rec to send:%s\r\n",String_Buff);
            if(strcmp(String_Buff, "OK") == 0)
            {
				// printf("pub success\r\n");
                retval = 1;
            }
			memset(String_Buff,0,sizeof(String_Buff));
       }
     
      
        
    }
    // ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
	// xSemaphoreTake(snrBinSemaphoreHandle, portMAX_DELAY);
	

    // ???? Esp8266_Send_Cmd ????????
    // while(!Esp8266_Send_Cmd(String_Buff, "OK", 1000) && retry < 3)
    // {
    //     printf("%s: ?? %d ?��?????????, ????????...\r\n", Debug_Info_Type[0], 2 - retry);
    //     HAL_Delay(1000);  // ??? 1 ??
    //     retry++;
    // }
    
    // if(retry < 3)
    // {
    //     // ??????
    //     retval = 1;
    // }
    // else
    // {
    //     retval = 0;
    //     printf("%s: ??????????\r\n", Debug_Info_Type[1]);
    // }

    return retval;
}



void Esp8266_Check(void)
{
	
}

void Esp8266_Init(void)
{
	Init_Success=0;
	OLED_ShowString(1,1,"     RST    ");
	
			while( 	 !Esp8266_Send_Cmd("AT+RST\r\n","OK",3000)	)
		{
			printf("reset failed\r\n");
					HAL_Delay(1000);
		}

		HAL_Delay(2000);
	OLED_ShowString(1,1,"  Close ATEO ");
	// 	 HAL_UART_Transmit(&huart2, (uint8_t*)"AT+RST\r\n", strlen("AT+RST\r\n"), 100);
    // 		vTaskDelay(pdMS_TO_TICKS(2000)); // ��΢��һ�£�AT+RST ��ʱҪ��ʱ��ſ�ʼ�����

    // // �ȴ� ��ready?? ����
    // Wait_ESP8266_Ready();

	
		// while( 	 !Esp8266_Send_Cmd("AT+RST\r\n","ready",10000)	)
		// {
		// 	printf("%sreset failed\r\n",Debug_Info_Type[0]);
		// 			HAL_Delay(1000);
		// }
			step =1;
	

		while( 	!Esp8266_Send_Cmd("ATE0\r\n","OK",2000)	 )  //??????
	{
		printf("ATEO close failed\r\n");
		HAL_Delay(1000);
	}
	
	
	OLED_ShowString(1,1,"  Set Mode   ");
		step =2;
	while( 	!Esp8266_Send_Cmd("AT+CWMODE=1\r\n","OK",1000)	 )
	{
		printf("set AP failed\r\n");
		HAL_Delay(1000);
	}
	

		
	step = 3;
	
		OLED_ShowString(1,1,"  Conn Wifi  ");
	sprintf(String_Buff,"AT+CWJAP=\"%s\",\"%s\"\r\n",WIFI_SSID,WIFI_PASSWD);
	if( !Esp8266_Send_Cmd("AT+CWJAP?\r\n","+CWJAP:\"szh\"",3000))
		while( 	!Esp8266_Send_Cmd(String_Buff,"WIFI GOT IP\r\n",1000)	 )
	{
		printf("WIFI connect failed\r\n");
		HAL_Delay(3000); 
	}
	else{
				printf("WIFI connect ssful\r\n");

	}
	
		step = 4;
			
	OLED_ShowString(1,1,"  Cfg User   ");
		sprintf(String_Buff,"AT+MQTTUSERCFG=0,1,\"%s\",\"%s\",\"%s\",0,0,\"\"\r\n",MQTT_CLIENT_ID,MQTT_USER_NAME,MQTT_PASSWD);

		// while( 	!Esp8266_Send_Cmd(String_Buff,"OK",1000)	 )
	while( 	!Esp8266_Send_Cmd(String_Buff,"OK",3000)	 )
	{
	printf("%susercfg failed\r\n",Debug_Info_Type[1]);
				HAL_Delay(3000);
	}
	printf("usercfg ssful\r\n");
	
	step = 5;
	OLED_ShowString(1,1,"  Conn Server");
		sprintf(String_Buff, "AT+MQTTCONN=0,\"%s\",1883,0\r\n", BROKER_ADDRESS);

	
		while( 	!Esp8266_Send_Cmd(String_Buff,"OK",3000)	 )
	{
		printf("connect server failed\r\n");
		HAL_Delay(3000);
	}
		printf("connect server ssful\r\n");

		step = 6;
	
	OLED_ShowString(1,1,"  Sub Topic  ");
	step = 7;
	sprintf(String_Buff, "AT+MQTTSUB=0,\""SUB_TOPIC"\",0\r\n" );
	while( 	!Esp8266_Send_Cmd(String_Buff,"OK",1000) 		)
	{
	printf("sub topic failed\r\n");
			HAL_Delay(1000);
	}
			printf("sub topic ssful\r\n");
	
	step=8;
		memset(String_Buff,0,sizeof(String_Buff));
		Esp8266_Pub_Msg("stm32 online");
		


}


bool Esp8266_Send_Cmd(char* cmd, char* cmd_response,uint16_t delay)
{
//	u16 wait=0;
	
	bool retval=0;

	xQueueReset(xESP8266MsgQueue);
    // printf("?????????��\r\n");
    // xEventGroupSetBits(eventGroup,1);
	HAL_UART_Transmit(&huart2,(u8*)cmd,strlen((char*)cmd),0xffff);
	// xSemaphoreTake(snrBinSemaphoreHandle, portMAX_DELAY);
	vTaskDelay(delay);
	if(xQueueReceive(xESP8266MsgQueue,info,pdMS_TO_TICKS(delay)) == pdTRUE)
	{
       
			if(step == 3)
				{
					if(strstr((char*)info,"WIFI CONNECTED") || strstr((char*)info,"WIFI GOT IP") || strstr((char*)info,cmd_response)|| strstr((char*)info,"AT+CWJAP=\"szh\",\"19990405\""))
					{
						
					
						retval = 1 ;
					}
						else
					{
					
					retval = 0;
					}
					
				}
                else if(step == 4)
				{
					if(strstr((char*)info,"AT+MQTTUSERCFG=0,1,\"szh\"") ||  strstr((char*)info,cmd_response))
					{
					    retval = 1 ;
					}
				}
				else if(step == 5)
				{
					if(strstr((char*)info,"AT+MQTTCONN=0,\"114.96.79.191\",1883,0") ||  strstr((char*)info,cmd_response))
					{
					    retval = 1 ;
					}
				}
				else if(step == 6)
				{
					if(strstr((char*)info,"AT+MQTTSUB=0,\"Android\",0") ||  strstr((char*)info,cmd_response))
					{
					    retval = 1 ;
					}
				}

				else
				{
						if(strstr((char*)info,cmd_response)) 
					{
							
						retval = 1 ;
					}
					else
					{
					
					retval = 0;
					}
					
				}
					
			}

		else
				retval = 0;
	
	
	
	return retval;
	
}

bool Esp8266_Receive_Msg()
{
	bool retval =0;
	int msg_len=0;
	uint8_t msg_body[256] ={0};

		printf("Esp8266_Receive_Msg\r\n");
		if(xQueueReceive(xESP8266MsgQueue,info,portMAX_DELAY)==pdPASS) 
		{
			if(Ctr_Mode == WiFi)
			{
					printf("Esp8266_Receive_GetMsg\r\n");
			if(sscanf((const char *)info,"+MQTTSUBRECV:0,\""SUB_TOPIC"\",%d,%s",&msg_len,msg_body)  ==2)
			{
				printf("match\r\n");
				if(strlen((const char*)msg_body)== (msg_len))
				{
					
					retval = Parse_Json_Msg(msg_body,msg_len);
					printf("�ɹ�����\r\n");
				}
				else
				{
					printf("length error\r\n body_len:%d  ",strlen((const char*)msg_body));
					retval = 1;
				}
				
			}
			}
		
			// else
			// {
            //     xQueueSend(recToSednQueue,info,portMAX_DELAY);
            //   	taskYIELD();
			// 	retval = 1;
			// }
		}
		
		return retval;
	
}

// bool Esp8266_Send_Cmd(char* cmd, char* cmd_response, uint16_t timeout_ms)
// {
//     bool retval = false;
//     uint8_t info[512] = {0};
//     uint8_t temp_buf[128];
//     TickType_t start_tick = xTaskGetTickCount();
//     TickType_t timeout_tick = pdMS_TO_TICKS(timeout_ms);

//     // ��ս��ն���?
//     xQueueReset(xESP8266MsgQueue);

//     // ����ָ??
//     HAL_UART_Transmit(&huart2, (uint8_t*)cmd, strlen(cmd), 0xffff);

//     while ((xTaskGetTickCount() - start_tick) < timeout_tick)
//     {
//         if (xQueueReceive(xESP8266MsgQueue, temp_buf, pdMS_TO_TICKS(100)) == pdTRUE)
//         {
//             strcat((char*)info, (char*)temp_buf); // ????��������

//             // ƥ���ж�
//             if (step == 3)
//             {
//                 if (strstr((char*)info, "WIFI CONNECTED") || strstr((char*)info, "WIFI GOT IP") || strstr((char*)info, cmd_response))
//                 {
//                     printf("%s��???:%s\n", Debug_Info_Obj[0], info);
//                     retval = true;
//                     break;
//                 }
//             }
//             else
//             {
//                 if (strstr((char*)info, cmd_response))
//                 {
//                     printf("%s��???:%s\n", Debug_Info_Obj[0], info);
//                     retval = true;
//                     break;
//                 }
//             }
//         }
//     }

//     if (!retval)
//     {
//         printf("%s response timeout or unmatched: %s\n", Debug_Info_Type[1], info);
//     }

//     return retval;
// }


// typedef enum {
//     JSONSuccess = 0,
//     JSONNotFound,
//     JSONBadParameter,
//     JSONInvalid
// } JSONStatus_t;

// ????????��?????????
typedef struct {
    const char* key;
    char* value;
    size_t length;
} JsonParam;

// ?????????????????
static int fast_atoi(const char* str, size_t len) {
    int sign = 1, value = 0;
    size_t i = 0;
    
    if (len == 0) return -1;
    if (str[0] == '-') {
        sign = -1;
        i = 1;
    }

    for (; i < len; i++) {
        if (str[i] < '0' || str[i] > '9') return -1;
        value = value * 10 + (str[i] - '0');
    }
    return value * sign;
}

bool Parse_Json_Msg(uint8_t *json_msg, uint8_t json_len) {
    bool retval = 0;
    JsonParam params[4] = {0}; // ???cmd??????????
    const char* keys[] = {"cmd", "MotorSpeed", "Led", "DjAngle"};
    
    // ???1??????????��?????????????????�h
    for (int i=0; i<4; i++) {
        JSONStatus_t result = JSON_Search((char*)json_msg, json_len, 
                                        keys[i], strlen(keys[i]),
                                        &params[i].value, &params[i].length);
        if (result == JSONSuccess) {
            params[i].key = keys[i];
        }
    }

    // ???2???????JSON???
    if (JSON_Validate((const char*)json_msg, json_len) != JSONSuccess) {
        printf("??��??JSON???\r\n");
        return 1;
    }

    // ???3????????
    if (params[0].value) { // cmd????
        // ??��????????????????????????
        if (strncmp(params[0].value, "set", params[0].length) == 0) {
            for (int i=1; i<4; i++) {
                if (params[i].value) {
                    int val = fast_atoi(params[i].value, params[i].length);
                    char buf[32];
                    
                    switch(i) {
                        case 1: // MotorSpeed
                            if (val < -100 || val > 100) {
                                printf("MotorSpeed???????��\r\n");
                                retval = 1;
                            } else {
                                SetMotorSpeed(val);
                                snprintf(buf, sizeof(buf), "MotorSpeed:%d", val);
                                Esp8266_Pub_Msg(buf);
                            }
                            break;
                            
                        case 2: // Led
                            if (val < 0 || val > 1) {
                                printf("LED???��\r\n");
                                retval = 1;
                            } else {
                                SetLedStatus(val);
                                snprintf(buf, sizeof(buf), "Led:%d", val);
                                Esp8266_Pub_Msg(buf);
                            }
                            break;
                            
                        case 3: // DjAngle
                            if (val < 0 || val > 180) {
                                printf("??????��\r\n");
                                retval = 1;
                            } else {
                                SetServoAngle(val);
                                snprintf(buf, sizeof(buf), "Angle:%d", val);
                                Esp8266_Pub_Msg(buf);
                            }
                            break;
                    }
                    return retval; // ?????????
                }
            }
            printf("��?????��????\r\n");
            retval = 1;
        }
        else if (strncmp(params[0].value, "get", params[0].length) == 0) {
            // ???????????????????????��???
            if (params[1].value) { // Param????
                if (strncmp(params[1].value, "Led", params[1].length) == 0) {
                    Esp8266_Pub_Msg(Led_Status ? "LED:ON" : "LED:OFF");
                } 
                else if (strncmp(params[1].value, "MotorSpeed", params[1].length) == 0) {
                    char buf[32];
                    snprintf(buf, sizeof(buf), "MotorSpeed:%d", MotorSpeed);
                    Esp8266_Pub_Msg(buf);
                }
                else {
                    Esp8266_Pub_Msg("??��????");
                    retval = 1;
                }
            }
        }
        else {
            printf("??��cmd???\r\n");
            retval = 1;
        }
    }
    return retval;
}