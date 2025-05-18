#include "DHT11.h"
#include "gpio.h"

bool DHT11_Check(void);
void DHT11_Start(void);

void Onewire_Switch_In()
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = DHT11_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(DHT11_GPIO_Port, &GPIO_InitStruct);
}


void Onewire_Switch_Out()
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = DHT11_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(DHT11_GPIO_Port, &GPIO_InitStruct);
}


//void Delay_us(uint16_t time)
//{
//	while(time--)
//	for(int i =0 ;i<72;i++)
//	__NOP();
//}


static void Delay_us(uint32_t delay)
{
    while (delay--)
    {
        for (int i = 0; i < 72; i++)
            __NOP();
    }
}

bool DHT11_Init()
{
	DHT11_Start();
	HAL_Delay(1000);
	DHT11_Start();
	return DHT11_Check();
}


void DHT11_Start()
{
	Onewire_Switch_Out();
	HAL_GPIO_WritePin(DHT11_GPIO_Port,DHT11_Pin,GPIO_PIN_RESET);
	HAL_Delay(20);   //20ms��ʱ
	HAL_GPIO_WritePin(DHT11_GPIO_Port,DHT11_Pin,GPIO_PIN_SET);
	Delay_us(40);  //30us��ʱ
	
}

/**
  * @brief          ��DHT11��ȡһ��λ
  * @param[in]      delay:��ʱ��ʱ��,��λus
  * @retval         ����ֵ��1/0  ,1Ϊ�ɹ���ȡ,  0Ϊʧ��
  */

bool DHT11_Check()
{
	u16 count=0;
	Onewire_Switch_In();
	while(HAL_GPIO_ReadPin(DHT11_GPIO_Port,DHT11_Pin) && count <= 100)
	{
		count ++ ;
		Delay_us(1);
	}
	printf("count1:%d\r\n",count);
	if(count <= 100) count = 0; //��⵽DHT11��������
	else 					return 0;
	
	while(!HAL_GPIO_ReadPin(DHT11_GPIO_Port,DHT11_Pin)&& count <= 100)
	{
		count ++ ;
		Delay_us(1);
	}
		printf("count2:%d\r\n",count);

	if(count <= 100) count = 0;  //��⵽DHT11��������
	else  return 0;
	
	return 1;
}

bool DHT11_ReadBit()
{
	
		u16 count=0;
	while(HAL_GPIO_ReadPin(DHT11_GPIO_Port,DHT11_Pin) &&count <100 ){
		count ++ ;
		Delay_us(1);
	}
	
	count=0;
	while(!HAL_GPIO_ReadPin(DHT11_GPIO_Port,DHT11_Pin) &&count <100 )
	{
		
		count ++ ;
		Delay_us(1);
	}

	Delay_us(40);
	
    if(HAL_GPIO_ReadPin(DHT11_GPIO_Port,DHT11_Pin))return 1; else return 0;

}


u8 DHT11_ReadByte()
{
	u8 temp = 0;
	for(int i=0;i<8;i++)
	{
			temp <<= 1;
		
			temp |= DHT11_ReadBit();
	}
	return temp;
}


bool DHT11_ReadData(u8* Humi,float* Temp)
{
//	u8 temp=0;
//	u8 sum =0;
//	u8 Parity=0;
//	DHT11_Start();
//	if(DHT11_Check())
//		for(int i=0;i<5;i++)
//		{
//			temp=DHT11_ReadByte();
//			sum += temp;
//			if(i == 0) *Humi=temp;
//			else if(i ==2) *Temp += temp;
//			else if(i ==3) 
//			{
//				if( temp & 0x80)  //�¶�Ϊ��
//				{
//					temp &= 0x7F;
//					*Temp += temp;
//					*Temp *= -1;
//				}
//				
//				else
//				{
//					*Temp += temp;
//				}
//			}
//			else if(i ==5) 	Parity =temp;
//		}
//	else
//	{
//		printf("check failed\r\n");
//		return 0;
//	}
//	
//	
//	//����У��
//	if(sum  == Parity) return 1;
//	else return 0;


 uint8_t buf[5];
  uint8_t i;
  DHT11_Start();//DHT11�˿ڸ�λ��������ʼ�ź�
  if(  DHT11_Check())
  { //�ȴ�DHT11��Ӧ
    for(i=0;i<5;i++)
    {//��ȡ5λ����
      buf[i]=DHT11_ReadByte(); //��������
    }
    if((buf[0]+buf[1]+buf[2]+buf[3])==buf[4])
    {	//����У��
      *Humi = buf[0]; //��ʪ��ֵ����ָ��1
      *Temp = buf[2] + buf[3] * 0.1; //���¶�ֵ����ָ��2
    }
		
		else
		{
			printf("DHT11 Raw Data: %d %d %d %d %d\r\n", buf[0], buf[1], buf[2], buf[3], buf[4]);
			printf("����У�鲻��\r\n");
		}
  }
  else 
  {
    return 0;
  }
  return 1;



}


















































