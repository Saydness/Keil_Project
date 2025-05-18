#include "work.h"
#include "vl53l0x.h"
#include "DHT11.h"
#include "OLED.h"
#include "PhotoSensor.h"
#include "Buzzer.h"
#include "ESP8266.h"
#include "Peripheral.h"
#include "StepMotor.h"
#include "tim.h"
#include "Scheduler.h"
#include "semphr.h"
#include "usart.h"
#include "driver_passive_buzzer.h"

/*  --------------------	?????????? 	------------------------*/
char *Debug_Info_Type[] = {"Warning:", "Error:", "Normal:"};
char *Debug_Info_Obj[]  = {"STM32-->", "��ESP8266"};
u8 Humi;
float Temp;
char OLED_Buff[20];
u8 temp, humi;
u16 Light_Intensity;
u16 Light_Intensity_Old;
u32 Light_Time_Tick;
u8 MotorSpeed = 0;
Ctr_Mode_Type Ctr_Mode;
bool Led_Status;
u8 SNR_Buff[5];
u8 cmd ;
bool Init_Success;
u8 Key_Val, Key_Down, Key_Up, Key_Old;
BaseType_t snr_pxHigherPriorityTaskWoken;
BaseType_t esp_pxHigherPriorityTaskWoken;

/*  --------------------	?????????? 	------------------------*/

void SNR_Work(void);
void TOF_Work(void);
void Scan_Key(void);
void System_init()
{
    HAL_UARTEx_ReceiveToIdle_IT(&huart2, Esp8266_Buff, sizeof(Esp8266_Buff));
    HAL_UARTEx_ReceiveToIdle_IT(&huart3, SNR_Buff, sizeof(SNR_Buff));
    PassiveBuzzer_Init();
    OLED_Init();
    OLED_Clear();
    Esp8266_Init();

    while (vl53l0x_init(&vl53l0x_dev)) // vl53l0x?????
    {
        printf("tof init failed\r\n");
        HAL_Delay(500);
    }

    vl53l0x_general_start(&vl53l0x_dev, 2);

    while( dht11_init() )
    {
    	HAL_Delay(10);
    	printf("Warning:DHT11 Device offline??\r\n");
    }

    printf("DHT11 be ready!\r\n");

    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1); //?????PWM
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4); //?????PWM
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
    Init_Success = 1;
}

void DHT11_Work(void)
{

    while (1) {
        /* code */
        if (dht11_read_data(&Temp, &Humi)) {
            printf("%sDHT11 Data Error\r\n", Debug_Info_Type[0]);

        } else {

            printf("Temp:%.1f Humi:%d\r\n", Temp, Humi);
        }

        if (Ctr_Mode == Auto) {
            if (Temp > 40) {
                SetMotorSpeed(100);
            } else if (Temp > 35) {
                SetMotorSpeed(75);
            } else if (Temp > 33) {
                SetMotorSpeed(50);
            }
        }

        // MUSIC_Analysis();
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    //	 else
}

void PhotoSensor_Work()
{

    while (1) {
        /* code */
        Light_Intensity = PhotoSensor_Get_LightIntensity();
        if (Ctr_Mode == Auto) {
            if (abs(Light_Intensity - Light_Intensity_Old) > 100) {
                Light_Time_Tick = xTaskGetTickCount();

            } else {
                if (xTaskGetTickCount() - Light_Time_Tick > 2000) {
                    if (Light_Intensity > 1000) {
                        StepMotor_Run_Angle(-1080, 0);
                    } else if (Light_Intensity > 500) {
                        StepMotor_Run_Angle(-540, 0);
                    } else {
                        StepMotor_Run_Angle(0, 0);
                    }
                }
            }
        }

        Light_Intensity_Old = Light_Intensity;
        vTaskDelay(50);
    }

    //		printf("Light_Intensity:%d\r\n",Light_Intensity);

    //		Buzzer_Turn(Light_Intensity < 500);
}

u8 Key_Val, Key_Down, Key_Up, Key_Old;
u8 Key_Read()
{
    u8 temp = 0;
    if (HAL_GPIO_ReadPin(Key1_GPIO_Port, Key1_Pin) == GPIO_PIN_RESET)
        temp = 1;
    else if (HAL_GPIO_ReadPin(Key2_GPIO_Port, Key2_Pin) == GPIO_PIN_RESET)
        temp = 2;
    return temp;
}

void Scan_Key(void)
{

    while (1) {
        /* code */

        Key_Val  = Key_Read();
        Key_Down = Key_Val & (Key_Val ^ Key_Old);
        Key_Up   = ~Key_Val & (Key_Val ^ Key_Old);
        Key_Old  = Key_Val;

        if (Key_Down == 1) {
            if (++Ctr_Mode == 3) Ctr_Mode = 0;

        } else if (Key_Down == 2) {
        }
        vTaskDelay(10);
    }
}

void OLED_Work()
{

    while (1) {
        /* code */
        // printf("OLED Work now\r\n");

        OLED_ShowString(1, 1, "   Smart Home   ");
        sprintf(OLED_Buff, "  T:%.1f H:%d", Temp, Humi);
        OLED_ShowString(2, 1, OLED_Buff);
        sprintf(OLED_Buff, " Distance:%d ", vl53l0x_data.RangeMilliMeter < 2000 ? vl53l0x_data.RangeMilliMeter : 0);

        OLED_ShowString(3, 1, OLED_Buff);

        sprintf(OLED_Buff, "Lt:%4d  %s", Light_Intensity, Ctr_Mode == 0 ? "Auto" : (Ctr_Mode == 1 ? "Wifi" : "Spch"));
        OLED_ShowString(4, 1, OLED_Buff);
        vTaskDelay(80);
    }
}

void ESP8266_Recover_Work(void)
{

    while (1) {
        /* code */
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        if (Ctr_Mode == WiFi) {
            OLED_Clear();
            OLED_ShowString(2, 1, "Net breakdown");
            vTaskSuspend(esp8266SendTask_handle);
            vTaskSuspend(esp8266RecTask_handle);
            MUSIC_Alarming();
            printf("recover start\r\n");
            MUSIC_Alarming();
            MUSIC_Alarming();
            MUSIC_Alarming();
            PassiveBuzzer_Control(0);
            Esp8266_Init();
            vTaskResume(esp8266SendTask_handle);
            vTaskResume(esp8266RecTask_handle);
            step = 0;
        }

        // step = 3;
        // sprintf(String_Buff, "AT+CWJAP=\"%s\",\"%s\"\r\n", WIFI_SSID, WIFI_PASSWD);
        // if (!Esp8266_Send_Cmd("AT+CWJAP?\r\n", "+CWJAP=\"" WIFI_SSID "\"", 1000))
        //     while (!Esp8266_Send_Cmd(String_Buff, "WIFI GOT IP\r\n", 5000)) {

        //         vTaskDelay(pdMS_TO_TICKS(3000));
        //     }
        // OLED_ShowString(2, 1, "  MQTT DISC");
        // step = 5;
        // sprintf(String_Buff, "AT+MQTTCONN=0,\"%s\",1883,0\r\n", BROKER_ADDRESS);
        // while (!Esp8266_Send_Cmd(String_Buff, "OK", 5000)) {

        //     vTaskDelay(5000);
        // }
    }
}
void Esp8266_Rec_Work(void)
{

    while (1) {
        
        Esp8266_Receive_Msg();
    }
}

void Esp8266_Send_Work(void)
{
    char temp[30];
    while (1) {

        sprintf(temp, "Temp:%.1f Humi:%d", Temp, Humi);
        printf("send work\r\n");
        Esp8266_Pub_Msg(temp);

        vTaskDelay(pdMS_TO_TICKS(3000));
    }
}

void SNR_Work(void)
{
    BaseType_t status;

    while (1) {
        status = xSemaphoreTake(snrBinSemaphoreHandle, portMAX_DELAY);

        if (status != pdTRUE) {
            printf("��ȡ�ź���ʧ��\r\n");
        } else {

            if (Ctr_Mode == Speech) {
                // taskENTER_CRITICAL();
              
                // taskEXIT_CRITICAL();
                switch (cmd) {
                    case 1: //  ????
                        printf("turn on light\r\n");
                        SetLedStatus(1);
                        break;

                    case 2: // ?????
                        printf("turn off light\r\n");
                        SetLedStatus(0);
                        break;

                    case 3: // ?????
                        SetMotorSpeed(25);
                        break;

                    case 4: // ??????
                        SetMotorSpeed(0);
                        break;

                    case 5: // ?????
                        SetMotorSpeed(50);
                        break;

                    case 6: // ??????
                        SetMotorSpeed(75);
                        break;

                    case 7: // ??????
                        SetMotorSpeed(100);
                        break;

                    case 8: // ?????
                        StepMotor_Run_Angle(1080, 0);
                        break;

                    case 9: // ??????
                        StepMotor_Run_Angle(540, 0);
                        break;
              
                    case 10: // ??????
                        StepMotor_Run_Angle(1800, 0);
                        break;

                    case 11:
                        SetServoAngle(0);
                        break;

                    case 12:
                        SetServoAngle(90);
                        break;
                }
                cmd=0;
            }
        }
    }
}

void TOF_Work()
{

    //	  if(!getDistance(&VL53))
    //		{
    //			printf("distance:%d\r\n",distance);
    //		}
    //		else{
    //						printf("error\r\n");

    //		}

    while (1) {
        vl53l0x_general_test(&vl53l0x_dev, Default_Mode); //

        if (Ctr_Mode == Auto) {
            if (vl53l0x_data.RangeMilliMeter < 500) {
                SetServoAngle(180);
            } else {
                SetServoAngle(0);
            }
        }

        vTaskDelay(500);
    }
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    // BaseType_t snrwork_status;
    // BaseType_t espwork_status;
    if (huart->Instance == USART2) {

        printf("%s <-from uart2\r\n", Esp8266_Buff);
        BaseType_t status = 0;
        if (Init_Success) {
            if (strstr(Esp8266_Buff, "+MQTTDISCONNECTED") != 0 || strstr(Esp8266_Buff, "WIFI DISCONNECT") != 0) {

                vTaskNotifyGiveFromISR(esp8266RecoverTask_handle, &status);
                portYIELD_FROM_ISR(status);
                // HAL_GPIO_WritePin(LED3_GPIO_Port,LED3_Pin,GPIO_PIN_RESET);
                memset(Esp8266_Buff, 0, Size);
                HAL_UARTEx_ReceiveToIdle_IT(&huart2, Esp8266_Buff, sizeof(Esp8266_Buff));
                return;
            }
            if (strstr(Esp8266_Buff, "+MQTTSUBRECV:0") != 0) {

                xQueueSendToBackFromISR(xESP8266MsgQueue, Esp8266_Buff, &status);
                portYIELD_FROM_ISR(status);

            } else {

                xQueueSendToBackFromISR(recToSednQueue, Esp8266_Buff, &status);
                portYIELD_FROM_ISR(status);
            }

        } else {
            xQueueSendToBackFromISR(xESP8266MsgQueue, Esp8266_Buff, &status);
            portYIELD_FROM_ISR(status);
        }

        memset(Esp8266_Buff, 0, Size);
        //   Esp8266_Receive_Msg();
        HAL_UARTEx_ReceiveToIdle_IT(&huart2, Esp8266_Buff, sizeof(Esp8266_Buff));
    }

    if (huart->Instance == USART3) {
        printf("%s <-from uart3\r\n", SNR_Buff);
        xSemaphoreGiveFromISR(snrBinSemaphoreHandle, &snr_pxHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(snr_pxHigherPriorityTaskWoken);

          cmd= SNR_Buff[0];
        HAL_UARTEx_ReceiveToIdle_IT(&huart3, SNR_Buff, sizeof(SNR_Buff));
    }
}
