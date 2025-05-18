#include "vl53l0x_i2c.h"





void VL_SDA_IN(void) 	
	{  
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = TOF_SDA_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(TOF_SDA_GPIO_Port, &GPIO_InitStruct);
} 
void VL_SDA_OUT(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = TOF_SDA_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(TOF_SDA_GPIO_Port, &GPIO_InitStruct);
	
}
void VL_IIC_SCL(uint8_t Data)
{
	  if(Data==1)


		{
		HAL_GPIO_WritePin(TOF_SCL_GPIO_Port,TOF_SCL_Pin,GPIO_PIN_SET);//PA11�����	
		}else 
		{
		HAL_GPIO_WritePin(TOF_SCL_GPIO_Port,TOF_SCL_Pin,GPIO_PIN_RESET);//PA11�����	
		}
}

void VL_IIC_SDA(uint8_t Data)
{
	  if(Data==1)
		{
		HAL_GPIO_WritePin(TOF_SDA_GPIO_Port,TOF_SDA_Pin,GPIO_PIN_SET);//PA12�����	
		}else 
		{
		HAL_GPIO_WritePin(TOF_SDA_GPIO_Port,TOF_SDA_Pin,GPIO_PIN_RESET);//PA12�����	
		}
}
uint8_t VL_READ_SDA(void)
{
	
		if( HAL_GPIO_ReadPin(TOF_SDA_GPIO_Port,TOF_SDA_Pin)==GPIO_PIN_SET)
		{
			return 1;
		}else
		{
			return 0;
		}

}
//us���ȴ�
void delay_us(u32 nus)
{		
int j;
  for(j=0;j<	nus;j++)
	{
	      for(int i = 0; i <20; i++)    
        {
            __asm("NOP");
        }
	}
}
//VL53L0X I2C��ʼ��
void VL53L0X_i2c_init(void)
{
	GPIO_InitTypeDef GPIO_Initure;
	
	__HAL_RCC_GPIOA_CLK_ENABLE();           //����GPIOAʱ��
	__HAL_RCC_GPIOB_CLK_ENABLE();           //����GPIOBʱ��
	__HAL_RCC_GPIOC_CLK_ENABLE();           //����GPIOCʱ��
	GPIO_Initure.Pin=TOF_SCL_Pin|TOF_SDA_Pin; //PA11��PA12
	GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //�������
	GPIO_Initure.Pull=GPIO_PULLUP;          //����
	GPIO_Initure.Speed=GPIO_SPEED_FREQ_HIGH;    	//����
	 HAL_GPIO_WritePin(TOF_SCL_GPIO_Port,TOF_SCL_Pin|TOF_SDA_Pin,GPIO_PIN_SET);//PA11,PA12 �����	
	HAL_GPIO_Init(TOF_SCL_GPIO_Port,&GPIO_Initure);	


	GPIO_Initure.Pin=TOF_XSHUT_Pin; 
	GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //�������
	GPIO_Initure.Pull=GPIO_PULLUP;          
	GPIO_Initure.Speed=GPIO_SPEED_FREQ_HIGH;    	//����
  HAL_GPIO_WritePin(TOF_XSHUT_GPIO_Port,TOF_XSHUT_Pin,GPIO_PIN_SET);//�����	
	HAL_GPIO_Init(TOF_XSHUT_GPIO_Port,&GPIO_Initure);	

}

//����IIC��ʼ�ź�
void VL_IIC_Start(void)
{
	VL_SDA_OUT();//sda�����
	VL_IIC_SDA(1);	  	  
	VL_IIC_SCL(1);
	delay_us(4);
 	VL_IIC_SDA(0);//START:when CLK is high,DATA change form high to low 
	delay_us(4);
	VL_IIC_SCL(0);//ǯסI2C���ߣ�׼�����ͻ�������� 
}

//����IICֹͣ�ź�
void VL_IIC_Stop(void)
{
	VL_SDA_OUT();//sda�����
	VL_IIC_SCL(0);
	VL_IIC_SDA(0);//STOP:when CLK is high DATA change form low to high
 	delay_us(4);
	VL_IIC_SCL(1); 
	VL_IIC_SDA(1);//����I2C���߽����ź�
	delay_us(4);							   	
}

//�ȴ�Ӧ���źŵ���
//����ֵ��1������Ӧ��ʧ��
//        0������Ӧ��ɹ�
u8 VL_IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	VL_SDA_IN();  //SDA����Ϊ����  
	VL_IIC_SDA(1);delay_us(1);	   
	VL_IIC_SCL(1);delay_us(1);	 
	while(VL_READ_SDA())
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			VL_IIC_Stop();
			return 1;
		}
	}
	VL_IIC_SCL(0);//ʱ�����0 	   
	return 0;  
}

//����ACKӦ��
void VL_IIC_Ack(void)
{
	VL_IIC_SCL(0);
	VL_SDA_OUT();
	VL_IIC_SDA(0);
	delay_us(2);
	VL_IIC_SCL(1);
	delay_us(2);
	VL_IIC_SCL(0);
}

//������ACKӦ��		    
void VL_IIC_NAck(void)
{
	VL_IIC_SCL(0);
	VL_SDA_OUT();
	VL_IIC_SDA(1);
	delay_us(2);
	VL_IIC_SCL(1);
	delay_us(2);
	VL_IIC_SCL(0);
}

//IIC����һ���ֽ�
//���شӻ�����Ӧ��
//1����Ӧ��
//0����Ӧ��			  
void VL_IIC_Send_Byte(u8 txd)
{                        
    u8 t;   
	VL_SDA_OUT(); 	    
    VL_IIC_SCL(0);//����ʱ�ӿ�ʼ���ݴ���
    for(t=0;t<8;t++)
    {              
		if((txd&0x80)>>7)
			VL_IIC_SDA(1);
		else
			VL_IIC_SDA(0);
		txd<<=1; 	  
		delay_us(2);  
		VL_IIC_SCL(1);
		delay_us(2); 
		VL_IIC_SCL(0);	
		delay_us(2);
    }	 
} 

//��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK   
u8 VL_IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	VL_SDA_IN();//SDA����Ϊ����
	for(i=0;i<8;i++ )
	{
		VL_IIC_SCL(0); 
		delay_us(4);
	  VL_IIC_SCL(1);
		receive<<=1;
		if(VL_READ_SDA())receive++;   
	  delay_us(4); //1
	}					 
	if (!ack)
		VL_IIC_NAck();//����nACK
	else
		VL_IIC_Ack(); //����ACK   
	return receive;
}

//IICдһ���ֽ�����
u8 VL_IIC_Write_1Byte(u8 SlaveAddress, u8 REG_Address,u8 REG_data)
{
	VL_IIC_Start();
	VL_IIC_Send_Byte(SlaveAddress);
	if(VL_IIC_Wait_Ack())
	{
		VL_IIC_Stop();//�ͷ�����
		return 1;//ûӦ�����˳�

	}
	VL_IIC_Send_Byte(REG_Address);
	VL_IIC_Wait_Ack();	
	VL_IIC_Send_Byte(REG_data);
	VL_IIC_Wait_Ack();	
	VL_IIC_Stop();

	return 0;
}

//IIC��һ���ֽ�����
u8 VL_IIC_Read_1Byte(u8 SlaveAddress, u8 REG_Address,u8 *REG_data)
{
	VL_IIC_Start();
	VL_IIC_Send_Byte(SlaveAddress);//��д����
	if(VL_IIC_Wait_Ack())
	{
		 VL_IIC_Stop();//�ͷ�����
		 return 1;//ûӦ�����˳�
	}		
	VL_IIC_Send_Byte(REG_Address);
	VL_IIC_Wait_Ack();
	VL_IIC_Start(); 
	VL_IIC_Send_Byte(SlaveAddress|0x01);//��������
	VL_IIC_Wait_Ack();
	*REG_data = VL_IIC_Read_Byte(0);
	VL_IIC_Stop();

	return 0;
}

//IICдn�ֽ�����
u8 VL_IIC_Write_nByte(u8 SlaveAddress, u8 REG_Address,u16 len, u8 *buf)
{
	VL_IIC_Start();
	VL_IIC_Send_Byte(SlaveAddress);//��д����
	if(VL_IIC_Wait_Ack()) 
	{
		VL_IIC_Stop();//�ͷ�����
		return 1;//ûӦ�����˳�
	}
	VL_IIC_Send_Byte(REG_Address);
	VL_IIC_Wait_Ack();
	while(len--)
	{
		VL_IIC_Send_Byte(*buf++);//����buff������
		VL_IIC_Wait_Ack();	
	}
	VL_IIC_Stop();//�ͷ�����

	return 0;
	
}

//IIC��n�ֽ�����
u8 VL_IIC_Read_nByte(u8 SlaveAddress, u8 REG_Address,u16 len,u8 *buf)
{
	VL_IIC_Start();
	VL_IIC_Send_Byte(SlaveAddress);//��д����
	if(VL_IIC_Wait_Ack()) 
	{
		VL_IIC_Stop();//�ͷ�����
		return 1;//ûӦ�����˳�
	}
	VL_IIC_Send_Byte(REG_Address);
	VL_IIC_Wait_Ack();

	VL_IIC_Start();
	VL_IIC_Send_Byte(SlaveAddress|0x01);//��������
	VL_IIC_Wait_Ack();
	while(len)
	{
		if(len==1)
		{
			*buf = VL_IIC_Read_Byte(0);
		}
		else
		{
			*buf = VL_IIC_Read_Byte(1);
		}
		buf++;
		len--;
	}
	VL_IIC_Stop();//�ͷ�����

	return 0;
	
}

//VL53L0X д�������
//address:��ַ
//index:ƫ�Ƶ�ַ
//pdata:����ָ��
//count:���� ���65535
u8 VL53L0X_write_multi(u8 address, u8 index,u8 *pdata,u16 count)
{
	u8 status = STATUS_OK;

	if(VL_IIC_Write_nByte(address,index,count,pdata))
	{
	   status  = STATUS_FAIL;

	}

	return status;
}


//VL53L0X ���������
//address:��ַ
//index:ƫ�Ƶ�ַ
//pdata:����ָ��
//count:���� ���65535
u8 VL53L0X_read_multi(u8 address,u8 index,u8 *pdata,u16 count)
{
	u8 status = STATUS_OK;

	if(VL_IIC_Read_nByte(address,index,count,pdata))
	{
	  status  = STATUS_FAIL;
	}

	return status;
}

//VL53L0X д1������(���ֽ�)
//address:��ַ
//index:ƫ�Ƶ�ַ
//data:����(8λ)
u8 VL53L0X_write_byte(u8 address,u8 index,u8 data)
{
	u8 status = STATUS_OK;

	status = VL53L0X_write_multi(address,index,&data,1);

	return status;
}

//VL53L0X д1������(˫�ֽ�)
//address:��ַ
//index:ƫ�Ƶ�ַ
//data:����(16λ)
u8 VL53L0X_write_word(u8 address,u8 index,u16 data)
{
	u8 status = STATUS_OK;
	
	u8 buffer[2];
	
	//��16λ���ݲ�ֳ�8λ
	buffer[0] = (u8)(data>>8);//�߰�λ
	buffer[1] = (u8)(data&0xff);//�Ͱ�λ
	
	if(index%2==1)
	{  
		//����ͨ�Ų��ܴ����Է�2�ֽڶ���Ĵ������ֽ�
		status = VL53L0X_write_multi(address,index,&buffer[0],1);
		status = VL53L0X_write_multi(address,index,&buffer[0],1);
	}else
	{
		status = VL53L0X_write_multi(address,index,buffer,2);
	}
	
	return status;
}

//VL53L0X д1������(���ֽ�)
//address:��ַ
//index:ƫ�Ƶ�ַ
//data:����(32λ)
u8 VL53L0X_write_dword(u8 address,u8 index,u32 data)
{
	
    u8 status = STATUS_OK;

    u8 buffer[4];	
	
	//��32λ���ݲ�ֳ�8λ
	buffer[0] = (u8)(data>>24);
	buffer[1] = (u8)((data&0xff0000)>>16);
	buffer[2] = (u8)((data&0xff00)>>8);
	buffer[3] = (u8)(data&0xff);
	
	status = VL53L0X_write_multi(address,index,buffer,4);
	
	return status;
	
}


//VL53L0X ��1������(���ֽ�)
//address:��ַ
//index:ƫ�Ƶ�ַ
//data:����(8λ)
u8 VL53L0X_read_byte(u8 address,u8 index,u8 *pdata)
{
	u8 status = STATUS_OK;
	 
	status = VL53L0X_read_multi(address,index,pdata,1);
	
	return status;
	 
}

//VL53L0X ��������(˫�ֽ�)
//address:��ַ
//index:ƫ�Ƶ�ַ
//data:����(16λ)
u8 VL53L0X_read_word(u8 address,u8 index,u16 *pdata)
{
	u8 status = STATUS_OK;
	
	u8 buffer[2];
	
	status = VL53L0X_read_multi(address,index,buffer,2);
	
	*pdata = ((u16)buffer[0]<<8)+(u16)buffer[1];
	
	return status;
	
}

//VL53L0X ��1������(���ֽ�)
//address:��ַ
//index:ƫ�Ƶ�ַ
//data:����(32λ)
u8 VL53L0X_read_dword(u8 address,u8 index,u32 *pdata)
{
	u8 status = STATUS_OK;
	
	u8 buffer[4];
	
	status = VL53L0X_read_multi(address,index,buffer,4);
	
	*pdata = ((u32)buffer[0]<<24)+((u32)buffer[1]<<16)+((u32)buffer[2]<<8)+((u32)buffer[3]);
	
	return status;
	
}
