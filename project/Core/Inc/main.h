/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "string.h"
#include "stdbool.h"

#include "math.h"
#include "stdlib.h"

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
typedef int32_t  s32;
typedef int16_t s16;
typedef int8_t  s8;
typedef __IO uint32_t  vu32;
typedef __IO uint16_t vu16;
typedef __IO uint8_t  vu8;
typedef uint32_t  u32;
typedef uint16_t u16;
typedef uint8_t  u8;
typedef const uint32_t uc32;  /*!< Read Only */
typedef const uint16_t uc16;  /*!< Read Only */
typedef const uint8_t uc8;   /*!< Read Only */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LED3_Pin GPIO_PIN_13
#define LED3_GPIO_Port GPIOC
#define ESP8266_RESET_Pin GPIO_PIN_14
#define ESP8266_RESET_GPIO_Port GPIOC
#define PhotoSensor_Pin GPIO_PIN_0
#define PhotoSensor_GPIO_Port GPIOA
#define Buzzer_Pin GPIO_PIN_1
#define Buzzer_GPIO_Port GPIOA
#define IN1_Pin GPIO_PIN_4
#define IN1_GPIO_Port GPIOA
#define IN2_Pin GPIO_PIN_5
#define IN2_GPIO_Port GPIOA
#define IN3_Pin GPIO_PIN_6
#define IN3_GPIO_Port GPIOA
#define IN4_Pin GPIO_PIN_7
#define IN4_GPIO_Port GPIOA
#define Motor2_TIM3_CH3_Pin GPIO_PIN_0
#define Motor2_TIM3_CH3_GPIO_Port GPIOB
#define TB6612_BIN1_Pin GPIO_PIN_1
#define TB6612_BIN1_GPIO_Port GPIOB
#define TB6612_BIN2_Pin GPIO_PIN_2
#define TB6612_BIN2_GPIO_Port GPIOB
#define TB6612_AIN1_Pin GPIO_PIN_12
#define TB6612_AIN1_GPIO_Port GPIOB
#define TB6612_AIN2_Pin GPIO_PIN_13
#define TB6612_AIN2_GPIO_Port GPIOB
#define LED1_Pin GPIO_PIN_14
#define LED1_GPIO_Port GPIOB
#define LED2_Pin GPIO_PIN_15
#define LED2_GPIO_Port GPIOB
#define Angle_TIM1_CH1_Pin GPIO_PIN_8
#define Angle_TIM1_CH1_GPIO_Port GPIOA
#define Motor_TIM1_CH4_Pin GPIO_PIN_11
#define Motor_TIM1_CH4_GPIO_Port GPIOA
#define DHT11_Pin GPIO_PIN_12
#define DHT11_GPIO_Port GPIOA
#define TOF_XSHUT_Pin GPIO_PIN_15
#define TOF_XSHUT_GPIO_Port GPIOA
#define TOF_SDA_Pin GPIO_PIN_4
#define TOF_SDA_GPIO_Port GPIOB
#define TOF_SCL_Pin GPIO_PIN_5
#define TOF_SCL_GPIO_Port GPIOB
#define Key1_Pin GPIO_PIN_6
#define Key1_GPIO_Port GPIOB
#define Key2_Pin GPIO_PIN_7
#define Key2_GPIO_Port GPIOB
#define OLED_SCL_Pin GPIO_PIN_8
#define OLED_SCL_GPIO_Port GPIOB
#define OLED_SDA_Pin GPIO_PIN_9
#define OLED_SDA_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
