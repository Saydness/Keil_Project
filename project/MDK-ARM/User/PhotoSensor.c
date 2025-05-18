#include "PhotoSensor.h"
#include "adc.h"


uint16_t Get_ADC_Value(ADC_HandleTypeDef* hadc);
uint16_t PhotoSensor_Get_LightIntensity(void);


#define VIN 3.3




uint16_t Get_ADC_Value(ADC_HandleTypeDef* hadc)
{
	u16 temp = 0;
	HAL_ADC_Start(hadc);
	temp = HAL_ADC_GetValue(hadc);
	return temp ;
}



uint16_t PhotoSensor_Get_LightIntensity()
{
	float VOUT = Get_ADC_Value(&hadc1)*3.3f/4096;
	float PhotoSensor_R = 1e4 * VOUT / VIN - VOUT ;
	u16 Lux = 4*1e4 * pow(PhotoSensor_R,-0.6021) ;
	
	return Lux;
}















