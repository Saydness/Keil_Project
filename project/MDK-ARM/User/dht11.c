/**
****************************(C) COPYRIGHT 2023 图解编程****************************
* @file       dht11.c
* @brief      dht11温湿度传感器驱动
* @note       
* @history
*  Version    Date            Author
*  V1.0.0     06-17-2023      图解编程    
*
****************************(C) COPYRIGHT 2023 图解编程****************************
*/

#include "dht11.h"

/**
  * @brief          微妙延时函数
  * @param[in]      delay:延时的时间,单位微妙
  * @retval         none
  */
static void delay_us(uint32_t delay)
{
	uint32_t i=0;  
	while(delay--)
	{
		i=10;
		while(i--) ;    
	}
}

/**
  * @brief          端口变为输出
  * @param[in]      none
  * @retval         none
  */
void dht11_io_out (void)
{ 
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = DHT11_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(DHT11_GPIO_Port, &GPIO_InitStruct);
}

/**
  * @brief          端口变为输入
  * @param[in]      none
  * @retval         none
  */
void dht11_io_in (void)
{ 
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = DHT11_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(DHT11_GPIO_Port, &GPIO_InitStruct);
}

/**
  * @brief          DHT11端口复位，发出起始信号（IO发送）
  * @param[in]      none
  * @retval         none
  */



void dht11_rst (void)
{
	dht11_io_out();
	HAL_GPIO_WritePin(DHT11_GPIO_Port,DHT11_Pin, GPIO_PIN_RESET);
	HAL_Delay(20); //拉低至少18ms
	HAL_GPIO_WritePin(DHT11_GPIO_Port,DHT11_Pin, GPIO_PIN_SET);
	delay_us(30); //主机拉高20~40us
}

/**
  * @brief          等待DHT11回应
  * @param[in]      none
  * @retval         返回1:未检测到DHT11，返回0:成功（IO接收）
  */
uint8_t dht11_check(void)
{ 
  uint8_t retry=0;
  dht11_io_in();//IO到输入状态
  while (HAL_GPIO_ReadPin(DHT11_GPIO_Port,DHT11_Pin)&&retry<100)
  {//DHT11会拉低40~80us
    retry++;
    delay_us(1);
  }
  if(retry>=100)return 1; else retry=0;
  while (!HAL_GPIO_ReadPin(DHT11_GPIO_Port,DHT11_Pin)&&retry<100)
  {//DHT11拉低后会再次拉高40~80us
    retry++;
    delay_us(1);
  }
  if(retry>=100)return 1;
  return 0;
}

/**
  * @brief          从DHT11读取一个位
  * @param[in]      delay:延时的时间,单位微妙
  * @retval         返回值：1/0
  */
uint8_t dht11_read_bit(void)
{ 
	uint8_t retry=0;
    while(HAL_GPIO_ReadPin(DHT11_GPIO_Port,DHT11_Pin)&&retry<100){//等待变为低电平
        retry++;
        delay_us(1);
    }
    retry=0;
    while(!HAL_GPIO_ReadPin(DHT11_GPIO_Port,DHT11_Pin)&&retry<100){//等待变高电平
        retry++;
        delay_us(1);
    }
    delay_us(40);//等待40us	//用于判断高低电平，即数据1或0
    if(HAL_GPIO_ReadPin(DHT11_GPIO_Port,DHT11_Pin))return 1; else return 0;
}

/**
  * @brief          从DHT11读取一个字节  
  * @param[in]      none
  * @retval         返回值：读到的数据
  */
uint8_t dht11_read_byte(void)
{
	uint8_t i,dat;
    dat=0;
    for (i=0;i<8;i++){
        dat<<=1;
        dat|=dht11_read_bit();
    }
    return dat;
}

/**
  * @brief          DHT11初始化
  * @param[in]      none
  * @retval         返回1:初始化失败 返回0:初始化成功
  */
uint8_t dht11_init (void)
{	
	dht11_rst();
  HAL_Delay(1000);
  dht11_rst();
	return dht11_check();
}

/**
  * @brief          读取一次数据//湿度值(十进制，范围:20%~90%) ，温度值(十进制，范围:0~50°)，返回值：0,正常;1,失败
  * @param[in]      delay:延时的时间,单位微妙
  * @retval         none
  */
uint8_t dht11_read_data(float *temp,uint8_t *humi)
{
  uint8_t buf[5];
  uint8_t i;
  dht11_rst();//DHT11端口复位，发出起始信号
  if(dht11_check()==0)
  { //等待DHT11回应
    for(i=0;i<5;i++)
    {//读取5位数据
      buf[i]=dht11_read_byte(); //读出数据
    }
    if((buf[0]+buf[1]+buf[2]+buf[3])==buf[4])
    {	//数据校验
      *humi =buf[0]; //将湿度值放入指针1
      *temp = buf[2]+ buf[3]*0.1; //将温度值放入指针2
    }
  }
  else 
  {
    return 1;
  }
  return 0;
}
