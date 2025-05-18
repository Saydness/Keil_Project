#include "Peripheral.h"
#include "tim.h"


#define Angle_TIM TIM1
#define Angle_Max_Freq  Angle_TIM->ARR*0.125
#define Angle_Min_Freq  Angle_TIM->ARR*0.025


void SetMotorSpeed(int speed)
{
	u16 temp =0;
	if(abs(speed)<=100)
	{
		if(speed>0)
		{
		HAL_GPIO_WritePin(TB6612_AIN1_GPIO_Port,TB6612_AIN1_Pin,GPIO_PIN_RESET);
		HAL_GPIO_WritePin(TB6612_AIN2_GPIO_Port,TB6612_AIN2_Pin,GPIO_PIN_SET);
			temp =speed/100.0*Angle_TIM->ARR;
		__HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_4,temp);

		
		}
	
		else{
		HAL_GPIO_WritePin(TB6612_AIN1_GPIO_Port,TB6612_AIN1_Pin,GPIO_PIN_SET);
		HAL_GPIO_WritePin(TB6612_AIN2_GPIO_Port,TB6612_AIN2_Pin,GPIO_PIN_RESET);
			temp =-speed/100.0*Angle_TIM->ARR;
		__HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_4,temp);
		
		}
	}
	
	
}


void SetLedStatus(bool status)
{
	HAL_GPIO_WritePin(LED3_GPIO_Port,LED3_Pin,(GPIO_PinState)!status);
	
}



void SetServoAngle(u8 angle)
{
	u16 compare=0;
	compare = angle*1.0/180*(Angle_Max_Freq-Angle_Min_Freq)+Angle_Min_Freq;
	Angle_TIM->CCR1=compare;
}











