#include "StepMotor.h"


#define IN1(x) 	HAL_GPIO_WritePin(IN1_GPIO_Port,IN1_Pin,(GPIO_PinState)(x))
#define IN2(x) 	HAL_GPIO_WritePin(IN2_GPIO_Port,IN2_Pin,(GPIO_PinState)(x))
#define IN3(x) 	HAL_GPIO_WritePin(IN3_GPIO_Port,IN3_Pin,(GPIO_PinState)(x))
#define IN4(x) 	HAL_GPIO_WritePin(IN4_GPIO_Port,IN4_Pin,(GPIO_PinState)(x))

int Angle=0;
void StepMotor_1_2(u8 step,uint8_t delay)
{
	switch(step)
	{
			case 1:  
				IN1(1);IN2(0);IN3(0);IN4(0);
			break;
			
			case 2:	
				IN1(1);IN2(1);IN3(0);IN4(0);
			break;
			
			case 3: 	
				IN1(0);IN2(1);IN3(0);IN4(0);

			break;
			
			case 4:		
				IN1(0);IN2(1);IN3(1);IN4(0);

			break;
			
			case 5:	
								IN1(0);IN2(0);IN3(1);IN4(0);

			break;
			
			case 6:		
									IN1(0);IN2(0);IN3(1);IN4(1);
			break;
			
			case 7:		
							IN1(0);IN2(0);IN3(0);IN4(1);
			break;
			
			case 8:		
					IN1(1);IN2(0);IN3(0);IN4(1);
			break;
	}
	HAL_Delay(delay);
}

void StepMotor_One_Step(bool dir,u8 delay)
{
	if(dir)
	for(int i =1 ;i<9;i++)
		StepMotor_1_2(i,delay);
	else
		for(int i =8 ;i>0;i--)
		StepMotor_1_2(i,delay);
}


void StepMotor_Run_Angle(int angle,u8 delay)
{
	if(angle>Angle)
		for(int i=0;i<(u16)(64.0*(angle-Angle)/45);i++)
		StepMotor_One_Step(1,delay);
	else
			for(int i=0;i<(u16)(64.0*(Angle-angle)/45);i++)
		StepMotor_One_Step(0,delay);
	 Angle=angle;
	
	
}



