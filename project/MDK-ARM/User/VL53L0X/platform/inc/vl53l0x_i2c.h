#ifndef __VL53L0_I2C_H
#define __VL53L0_I2C_H

#include "main.h"





#define   VL53L0X_XshutPort   GPIOA
#define   VL53L0X_XshutPin    GPIO_PIN_15//PB7


//״̬
#define STATUS_OK       0x00
#define STATUS_FAIL     0x01

//IIC��������
void VL53L0X_i2c_init(void);//��ʼ��IIC��IO��

uint8_t VL53L0X_write_byte(uint8_t address,uint8_t index,uint8_t data);              //IICдһ��8λ����
uint8_t VL53L0X_write_word(uint8_t address,uint8_t index,uint16_t data);             //IICдһ��16λ����
uint8_t VL53L0X_write_dword(uint8_t address,uint8_t index,uint32_t data);            //IICдһ��32λ����
uint8_t VL53L0X_write_multi(uint8_t address, uint8_t index,uint8_t *pdata,uint16_t count);//IIC����д

uint8_t VL53L0X_read_byte(uint8_t address,uint8_t index,uint8_t *pdata);             //IIC��һ��8λ����
uint8_t VL53L0X_read_word(uint8_t address,uint8_t index,uint16_t *pdata);            //IIC��һ��16λ����
uint8_t VL53L0X_read_dword(uint8_t address,uint8_t index,uint32_t *pdata);           //IIC��һ��32λ����
uint8_t VL53L0X_read_multi(uint8_t address,uint8_t index,uint8_t *pdata,uint16_t count);  //IIC������


#endif 


