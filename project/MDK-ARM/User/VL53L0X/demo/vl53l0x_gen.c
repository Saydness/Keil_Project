#include "vl53l0x_gen.h"
#include "oled.h"


 char buf[VL53L0X_MAX_STRING_LENGTH];
void delay_ms(uint32_t Time)
{
	HAL_Delay(Time);
	
}
VL53L0X_RangingMeasurementData_t vl53l0x_data;//�������ṹ��
volatile uint16_t Distance_data=0;//����������

//VL53L0X ����ģʽ����
//dev:�豸I2C�����ṹ��
//mode: 0:Ĭ��;1:�߾���;2:������
VL53L0X_Error vl53l0x_set_mode(VL53L0X_Dev_t *dev,u8 mode)
{
	
	 VL53L0X_Error status = VL53L0X_ERROR_NONE;
	 uint8_t VhvSettings;
	 uint8_t PhaseCal;
	 uint32_t refSpadCount;
	 uint8_t isApertureSpads;
	
	 vl53l0x_reset(dev);//��λvl53l0x(Ƶ���л�����ģʽ���׵��²ɼ��������ݲ�׼���������һ����)
	 status = VL53L0X_StaticInit(dev);

     if(AjustOK!=0)//��У׼����,д��У׼ֵ
     {
	    status= VL53L0X_SetReferenceSpads(dev,Vl53l0x_data.refSpadCount,Vl53l0x_data.isApertureSpads);//�趨SpadsУ׼ֵ
        if(status!=VL53L0X_ERROR_NONE) goto error;	
        delay_ms(2);		 
	    status= VL53L0X_SetRefCalibration(dev,Vl53l0x_data.VhvSettings,Vl53l0x_data.PhaseCal);//�趨RefУ׼ֵ
		if(status!=VL53L0X_ERROR_NONE) goto error;
		 delay_ms(2);
	    status=VL53L0X_SetOffsetCalibrationDataMicroMeter(dev,Vl53l0x_data.OffsetMicroMeter);//�趨ƫ��У׼ֵ
		if(status!=VL53L0X_ERROR_NONE) goto error; 
		 delay_ms(2);
		status=VL53L0X_SetXTalkCompensationRateMegaCps(dev,Vl53l0x_data.XTalkCompensationRateMegaCps);//�趨����У׼ֵ
		if(status!=VL53L0X_ERROR_NONE) goto error;
         delay_ms(2);		 
		 
     }
	 else
	 {
		status = VL53L0X_PerformRefCalibration(dev, &VhvSettings, &PhaseCal);//Ref�ο�У׼
		if(status!=VL53L0X_ERROR_NONE) goto error;
		delay_ms(2);
		status = VL53L0X_PerformRefSpadManagement(dev, &refSpadCount, &isApertureSpads);//ִ�вο�SPAD����
		if(status!=VL53L0X_ERROR_NONE) goto error;
        delay_ms(2);		 	 
	 }
	 status = VL53L0X_SetDeviceMode(dev,VL53L0X_DEVICEMODE_SINGLE_RANGING);//ʹ�ܵ��β���ģʽ
	 if(status!=VL53L0X_ERROR_NONE) goto error;
	 delay_ms(2);
	 status = VL53L0X_SetLimitCheckEnable(dev,VL53L0X_CHECKENABLE_SIGMA_FINAL_RANGE,1);//ʹ��SIGMA��Χ���
	 if(status!=VL53L0X_ERROR_NONE) goto error;
	 delay_ms(2);
	 status = VL53L0X_SetLimitCheckEnable(dev,VL53L0X_CHECKENABLE_SIGNAL_RATE_FINAL_RANGE,1);//ʹ���ź����ʷ�Χ���
	 if(status!=VL53L0X_ERROR_NONE) goto error;
	 delay_ms(2);
	 status = VL53L0X_SetLimitCheckValue(dev,VL53L0X_CHECKENABLE_SIGMA_FINAL_RANGE,Mode_data[mode].sigmaLimit);//�趨SIGMA��Χ
	 if(status!=VL53L0X_ERROR_NONE) goto error;
	 delay_ms(2);
	 status = VL53L0X_SetLimitCheckValue(dev,VL53L0X_CHECKENABLE_SIGNAL_RATE_FINAL_RANGE,Mode_data[mode].signalLimit);//�趨�ź����ʷ�Χ��Χ
	 if(status!=VL53L0X_ERROR_NONE) goto error;
	 delay_ms(2);
	 status = VL53L0X_SetMeasurementTimingBudgetMicroSeconds(dev,Mode_data[mode].timingBudget);//�趨��������ʱ��
	 if(status!=VL53L0X_ERROR_NONE) goto error;
	 delay_ms(2);
	 status = VL53L0X_SetVcselPulsePeriod(dev, VL53L0X_VCSEL_PERIOD_PRE_RANGE, Mode_data[mode].preRangeVcselPeriod);//�趨VCSEL��������
	 if(status!=VL53L0X_ERROR_NONE) goto error;
	 delay_ms(2);
	 status = VL53L0X_SetVcselPulsePeriod(dev, VL53L0X_VCSEL_PERIOD_FINAL_RANGE, Mode_data[mode].finalRangeVcselPeriod);//�趨VCSEL�������ڷ�Χ
	 
	 error://������Ϣ
	 if(status!=VL53L0X_ERROR_NONE)
	 {
		print_pal_error(status);
		return status;
	 }
	 return status;
	
}	

//VL53L0X ���ξ����������
//dev:�豸I2C�����ṹ��
//pdata:����������ݽṹ��
VL53L0X_Error vl53l0x_start_single_test(VL53L0X_Dev_t *dev,VL53L0X_RangingMeasurementData_t *pdata,char *buf)
{

	VL53L0X_Error status = VL53L0X_ERROR_NONE;
	uint8_t RangeStatus;
	
	status = VL53L0X_PerformSingleRangingMeasurement(dev, pdata);//ִ�е��β�ಢ��ȡ����������
	if(status !=VL53L0X_ERROR_NONE) return status;
   
	RangeStatus = pdata->RangeStatus;//��ȡ��ǰ����״̬
    memset(buf,0x00,VL53L0X_MAX_STRING_LENGTH);
	VL53L0X_GetRangeStatusString(RangeStatus,buf);//���ݲ���״̬��ȡ״̬�ַ���
	
	Distance_data = pdata->RangeMilliMeter;//�������һ�β���������

    return status;
}



//������ͨ����
//dev���豸I2C�����ṹ��
//modeģʽ���� 0:Ĭ��;1:�߾���;2:������;3:����
VL53L0X_Error vl53l0x_general_start(VL53L0X_Dev_t *dev,u8 mode)
{
	//����ģʽ�ַ����ַ�������
		VL53L0X_Error Status=VL53L0X_ERROR_NONE;//����״̬
	
	Status=vl53l0x_set_mode(dev,mode);//���ò���ģʽ
	
	
     
		
	return Status;
	
}

//vl53l0x��ͨ����ģʽ����
//dev:�豸I2C�����ṹ��
//mode:����ģʽ
void vl53l0x_general_test(VL53L0X_Dev_t *dev, u8 mode)
{ 
			VL53L0X_Error Status=VL53L0X_ERROR_NONE;
	
	 if(Status==VL53L0X_ERROR_NONE)
		 {
			
				Status = vl53l0x_start_single_test(dev,&vl53l0x_data,buf);//ִ��һ�β���
		
	
			    if(vl53l0x_data.RangeMilliMeter<=2000)
						{
					
					
							printf("RangeMilliMeter:%d\r\n",vl53l0x_data.RangeMilliMeter);
						}						
		 }
//     delay_ms(200);//�ȴ�200ms
	
	
}
