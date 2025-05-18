// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright (c) 2008-2023 100askTeam : Dongshan WEI <weidongshan@qq.com>
 * Discourse:  https://forums.100ask.net
 */

/*  Copyright (C) 2008-2023 深圳百问网科技有限公司
 *  All rights reserved
 *
 * 免责声明: 百问网编写的文档, 仅供学员学习使用, 可以转发或引用(请保留作者信息),禁止用于商业用途！
 * 免责声明: 百问网编写的程序, 可以用于商业用途, 但百问网不承担任何后果！
 *
 * 本程序遵循GPL V3协议, 请遵循协议
 * 百问网学习平台   : https://www.100ask.net
 * 百问网交流社区   : https://forums.100ask.net
 * 百问网官方B站    : https://space.bilibili.com/275908810
 * 本程序所用开发板 : DShanMCU-F103
 * 百问网官方淘宝   : https://100ask.taobao.com
 * 联系我们(E-mail): weidongshan@qq.com
 *
 *          版权所有，盗版必究。
 *
 * 修改历史     版本号           作者        修改内容
 *-----------------------------------------------------
 * 2023.08.04      v01         百问科技      创建文件
 *-----------------------------------------------------
 */

#include "driver_passive_buzzer.h"

#include "driver_timer.h"
#include "stm32f1xx_hal.h"
#include "tim.h"
#include "Scheduler.h"
#include "event_groups.h"
#include "queue.h"
extern TIM_HandleTypeDef htim2;

static TIM_HandleTypeDef *g_HPWM_PassiveBuzzer = &htim2;

/**********************************************************************
 * 函数名称： PassiveBuzzer_Init
 * 功能描述： 无源蜂鸣器的初始化函数
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期：      版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/04	     V1.0	  韦东山	      创建
 ***********************************************************************/
void PassiveBuzzer_Init(void)
{
    TIM_OC_InitTypeDef sConfig       = {0};
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /**TIM1 GPIO Configuration
    PA8     ------> TIM1_CH1
    */
    GPIO_InitStruct.Pin   = GPIO_PIN_1;
    GPIO_InitStruct.Mode  = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // HAL_TIM_Base_DeInit(g_HPWM_PassiveBuzzer);
    g_HPWM_PassiveBuzzer->Instance               = TIM2;
    g_HPWM_PassiveBuzzer->Init.Prescaler         = 71;
    g_HPWM_PassiveBuzzer->Init.CounterMode       = TIM_COUNTERMODE_UP;
    g_HPWM_PassiveBuzzer->Init.Period            = 999; /* 1KHz */
    g_HPWM_PassiveBuzzer->Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;
    g_HPWM_PassiveBuzzer->Init.RepetitionCounter = 0;
    g_HPWM_PassiveBuzzer->Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
    // HAL_TIM_Base_Init(g_HPWM_PassiveBuzzer);
    TIM_Base_SetConfig(g_HPWM_PassiveBuzzer->Instance, &g_HPWM_PassiveBuzzer->Init);
    // return;

    sConfig.OCMode     = TIM_OCMODE_PWM1;    // PWM 输出的两种模式:PWM1 当极性为低,CCR<CNT,输出低电平,反之高电平
    sConfig.OCPolarity = TIM_OCPOLARITY_LOW; // 设置极性为低(硬件上低电平亮灯)
    sConfig.OCFastMode = TIM_OCFAST_DISABLE; // 输出比较快速使能禁止(仅在 PWM1 和 PWM2 可设置)
    sConfig.Pulse      = 499;                // 在 PWM1 模式下,50%占空比

    HAL_TIM_PWM_ConfigChannel(g_HPWM_PassiveBuzzer, &sConfig, TIM_CHANNEL_1);
}

/**********************************************************************
 * 函数名称： PassiveBuzzer_Control
 * 功能描述： 无源蜂鸣器控制函数
 * 输入参数： on - 1-响, 0-不响
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期：      版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/04	     V1.0	  韦东山	      创建
 ***********************************************************************/
void PassiveBuzzer_Control(int on)
{
    if (on) {
        HAL_TIM_PWM_Start(g_HPWM_PassiveBuzzer, TIM_CHANNEL_2);
    } else {
        HAL_TIM_PWM_Stop(g_HPWM_PassiveBuzzer, TIM_CHANNEL_2);
    }
}

/**********************************************************************
 * 函数名称： PassiveBuzzer_Set_Freq_Duty
 * 功能描述： 无源蜂鸣器控制函数: 设置频率和占空比
 * 输入参数： freq - 频率, duty - 占空比
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期：      版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/04	     V1.0	  韦东山	      创建
 ***********************************************************************/
void PassiveBuzzer_Set_Freq_Duty(int freq, int duty)
{
    TIM_OC_InitTypeDef sConfig = {0};

    HAL_TIM_PWM_Stop(g_HPWM_PassiveBuzzer, TIM_CHANNEL_2);

    HAL_TIM_Base_DeInit(g_HPWM_PassiveBuzzer);
    g_HPWM_PassiveBuzzer->Instance               = TIM2;
    g_HPWM_PassiveBuzzer->Init.Prescaler         = 71;
    g_HPWM_PassiveBuzzer->Init.CounterMode       = TIM_COUNTERMODE_UP;
    g_HPWM_PassiveBuzzer->Init.Period            = 1000000 / freq - 1;
    g_HPWM_PassiveBuzzer->Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;
    g_HPWM_PassiveBuzzer->Init.RepetitionCounter = 0;
    g_HPWM_PassiveBuzzer->Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
    HAL_TIM_Base_Init(g_HPWM_PassiveBuzzer);

    sConfig.OCMode     = TIM_OCMODE_PWM1;                   // PWM 输出的两种模式:PWM1 当极性为低,CCR<CNT,输出低电平,反之高电平
    sConfig.OCPolarity = TIM_OCPOLARITY_LOW;                // 设置极性为低(硬件上低电平亮灯)
    sConfig.OCFastMode = TIM_OCFAST_DISABLE;                // 输出比较快速使能禁止(仅在 PWM1 和 PWM2 可设置)
    sConfig.Pulse      = (1000000 / freq - 1) * duty / 100; // 在 PWM1 模式占空比

    HAL_TIM_PWM_ConfigChannel(g_HPWM_PassiveBuzzer, &sConfig, TIM_CHANNEL_2);

    HAL_TIM_PWM_Start(g_HPWM_PassiveBuzzer, TIM_CHANNEL_2);
}

/**********************************************************************
 * 函数名称： PassiveBuzzer_Test
 * 功能描述： 无源蜂鸣器测试程序
 * 输入参数： 无
 * 输出参数： 无
 *            无
 * 返 回 值： 无
 * 修改日期        版本号     修改人        修改内容
 * -----------------------------------------------
 * 2023/08/04        V1.0     韦东山       创建
 ***********************************************************************/
// void PassiveBuzzer_Test(void)
//{
//     PassiveBuzzer_Init();
//	LCD_Init();
//	LCD_Clear();

//    while (1)
//    {
//        LCD_PrintString(0, 0, "PassiveBuzzer");
//        LCD_PrintString(0, 2, "ON ");
//        PassiveBuzzer_Control(1);
//        mdelay(1000);

//        LCD_PrintString(0, 0, "PassiveBuzzer");
//        LCD_PrintString(0, 2, "OFF");
//        PassiveBuzzer_Control(0);
//        mdelay(1000);
//    }
//}

#define Bass          0
#define Alto          1
#define Teble         2

#define One_Beat      1
#define One_TWO_Beat  2
#define One_FOUR_Beat 4

/**
 ******************************************************************************
 * @file           : Music.c
 * @brief          : Music program body
 * @author         : Lesterbor
 *	@time			: 2022-01-15
 *
 ******************************************************************************
 * @attention
 *
 *
 *
 ******************************************************************************
 */

/* USER CODE END Includes */
/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PT */
const uint16_t Tone_Index[8][3] = {
    {0, 0, 0},
    {262, 523, 1046},
    {294, 587, 1175},
    {330, 659, 1318},
    {349, 698, 1397},
    {392, 784, 1568},
    {440, 880, 1760},
    {494, 988, 1976}};

const uint16_t Music_Jingdi[][3] = {
    // 曲信息：每分钟60拍 => 每拍1000ms

    {0, 0, 1500},

    // 第一轮报警声（低音持续2拍 → 高音持续2拍）
    {8, Bass, One_FOUR_Beat},  // “呜～”
    {7, Teble, One_FOUR_Beat}, // “哇～”

    // 第二轮
    {8, Bass, One_FOUR_Beat},
    {7, Teble, One_FOUR_Beat},

};

//
////中速代表乐曲速度术语里的Moderato，或称中板，每分钟在88到104拍。
////中速 每分钟100拍 一拍600ms
// 两只老虎简谱，没有细调
//	uint16_t Music_Two_Tigers[][3]={
//		{0,0,570},
//		{1,Alto,One_Beat},
//		{2,Alto,One_Beat},
//		{3,Alto,One_Beat},
//		{1,Alto,One_Beat},
//
//		{0,Alto,24},
//
//		{1,Alto,One_Beat},
//		{2,Alto,One_Beat},
//		{3,Alto,One_Beat},
//		{1,Alto,One_Beat},
//
//		//{0,Alto,3},
//
//		{3,Alto,One_Beat},
//		{4,Alto,One_Beat},
//		{5,Alto,One_Beat},
//		{0,Alto,One_Beat},
//
//
//		{3,Alto,One_Beat},
//		{4,Alto,One_Beat},
//		{5,Alto,One_Beat},
//		{0,Alto,One_Beat},
//
//
//		{5,Alto,One_TWO_Beat},
//		{6,Alto,One_TWO_Beat},
//		{5,Alto,One_TWO_Beat},
//		{4,Alto,One_TWO_Beat},
//		{3,Alto,One_Beat},
//		{1,Alto,One_Beat},
//
//		//{0,Alto,3},
//
//		{5,Alto,One_TWO_Beat},
//		{6,Alto,One_TWO_Beat},
//		{5,Alto,One_TWO_Beat},
//		{4,Alto,One_TWO_Beat},
//		{3,Alto,One_Beat},
//		{1,Alto,One_Beat},
//
//		{0,Alto,24},
//
//		{1,Alto,One_Beat},
//		{5,Bass,One_Beat},
//		{1,Alto,One_Beat},
//		{0,Alto,One_Beat},
//
//
//		{1,Alto,One_Beat},
//		{5,Bass,One_Beat},
//		{1,Alto,One_Beat},
//		{0,Alto,One_Beat},
//

//	};
// 中速 每分钟65拍 一拍920ms
const uint16_t Music_Lone_Brave[][3] = {
    // 曲信息
    {0, 0, 920},

    //	#define Bass		0
    //	#define	Alto		1
    //	#define	Teble		2

    //	#define	One_Beat				1
    //	#define	One_TWO_Beat		2
    //	#define	One_FOUR_Beat		4

    // 第一小节
    {2, Alto, One_TWO_Beat},
    {7, Bass, One_TWO_Beat},
    {1, Alto, One_TWO_Beat},
    {6, Bass, One_TWO_Beat},
    {2, Alto, One_TWO_Beat},
    {7, Bass, One_TWO_Beat},
    {1, Alto, One_TWO_Beat},
    {6, Bass, One_TWO_Beat},

    // 第二小节
    {2, Alto, One_TWO_Beat},
    {7, Bass, One_TWO_Beat},
    {1, Alto, One_TWO_Beat},
    {6, Bass, One_TWO_Beat},
    {2, Alto, One_TWO_Beat},
    {7, Bass, One_TWO_Beat},
    {1, Alto, One_TWO_Beat},
    {6, Bass, One_TWO_Beat},

    // 第三小节
    {2, Alto, One_TWO_Beat},
    {7, Bass, One_TWO_Beat},
    {1, Alto, One_TWO_Beat},
    {6, Bass, One_TWO_Beat},
    {2, Alto, One_TWO_Beat},
    {7, Bass, One_TWO_Beat},
    {1, Alto, One_TWO_Beat},
    {6, Bass, One_TWO_Beat},

    // 第四小节
    {2, Alto, One_TWO_Beat},
    {7, Bass, One_TWO_Beat},
    {1, Alto, One_TWO_Beat},
    {6, Bass, One_TWO_Beat},
    {2, Alto, One_TWO_Beat},
    {7, Bass, One_TWO_Beat},
    {1, Alto, One_TWO_Beat},
    {6, Bass, One_TWO_Beat},

    // 第五小节
    {2, Alto, One_TWO_Beat},
    {7, Bass, One_TWO_Beat},
    {1, Alto, One_TWO_Beat},
    {6, Bass, One_TWO_Beat},
    {2, Alto, One_TWO_Beat},
    {7, Bass, One_TWO_Beat},
    {1, Alto, One_TWO_Beat},
    {6, Bass, One_TWO_Beat},

    // 第六小节
    {3, Alto, One_Beat},
    {3, Alto, One_Beat},
    {0, Alto, One_Beat},
    {0, Alto, One_FOUR_Beat},
    {1, Alto, One_FOUR_Beat},
    {2, Alto, One_FOUR_Beat},
    {1, Alto, One_FOUR_Beat},

    // 第七小节
    {3, Alto, One_Beat},
    {3, Alto, One_Beat},
    {0, Alto, One_TWO_Beat},
    {1, Alto, One_FOUR_Beat},
    {2, Alto, One_FOUR_Beat},
    {1, Alto, One_FOUR_Beat},
    {2, Alto, One_FOUR_Beat},
    {3, Alto, One_FOUR_Beat},

    // 第八小节
    {6, Bass, One_TWO_Beat},
    {1, Alto, One_FOUR_Beat},
    {6, Bass, One_TWO_Beat},
    {1, Alto, One_FOUR_Beat},
    {6, Bass, One_TWO_Beat},
    {1, Alto, One_FOUR_Beat},
    {2, Alto, One_TWO_Beat},
    {1, Alto, One_TWO_Beat},

    // 第九小节
    {7, Bass, One_TWO_Beat},
    {7, Bass, One_FOUR_Beat},
    {0, Alto, One_TWO_Beat},
    {0, Alto, One_FOUR_Beat},

    // 第十小节
    {3, Alto, One_Beat},
    {3, Alto, One_Beat},
    {0, Alto, One_Beat},
    {0, Alto, One_FOUR_Beat},
    {1, Alto, One_FOUR_Beat},
    {2, Alto, One_FOUR_Beat},
    {1, Alto, One_FOUR_Beat},

    // 第十一小节
    {3, Alto, One_Beat},
    {3, Alto, One_Beat},
    {0, Alto, One_TWO_Beat},
    {1, Alto, One_FOUR_Beat},
    {2, Alto, One_FOUR_Beat},
    {1, Alto, One_FOUR_Beat},
    {2, Alto, One_FOUR_Beat},
    {3, Alto, One_FOUR_Beat},

    // 第十二小节
    {6, Bass, One_TWO_Beat},
    {1, Alto, One_FOUR_Beat},
    {6, Bass, One_TWO_Beat},
    {1, Alto, One_FOUR_Beat},
    {6, Bass, One_TWO_Beat},
    {1, Alto, One_FOUR_Beat},
    {3, Alto, One_TWO_Beat},
    {2, Alto, One_TWO_Beat},

    // 第十三小节
    {7, Bass, One_TWO_Beat},
    {7, Bass, One_FOUR_Beat},
    {0, Alto, One_TWO_Beat},
    {0, Alto, One_FOUR_Beat},

    // 第十四小节
    {6, Bass, One_FOUR_Beat},
    {1, Alto, One_FOUR_Beat},
    {6, Alto, One_TWO_Beat},
    {6, Alto, One_FOUR_Beat},
    {0, Alto, 20 /*小衔接*/},
    {6, Alto, One_FOUR_Beat},
    {6, Alto, One_FOUR_Beat},
    {5, Alto, One_FOUR_Beat},
    {6, Alto, One_TWO_Beat},
    {6, Alto, One_FOUR_Beat},
    {5, Alto, One_FOUR_Beat},
    {6, Alto, One_FOUR_Beat},
    {5, Alto, One_FOUR_Beat},
    {6, Alto, One_FOUR_Beat},
    {5, Alto, One_FOUR_Beat},

    // 第十五小节
    {3, Alto, One_FOUR_Beat},
    {3, Alto, One_TWO_Beat},
    {3, Alto, One_Beat},
    {0, Alto, 20 /*小衔接*/},
    {0, Alto, One_Beat},
    {0, Alto, One_TWO_Beat},
    {6, Bass, One_FOUR_Beat},
    {1, Alto, One_FOUR_Beat},

    // 第十六小节
    {6, Alto, One_TWO_Beat},
    {6, Alto, One_FOUR_Beat},
    {0, Alto, 20 /*小衔接*/},
    {6, Alto, One_FOUR_Beat},
    {5, Alto, One_FOUR_Beat},
    {6, Alto, One_FOUR_Beat},
    {5, Alto, One_FOUR_Beat},
    {7, Alto, One_TWO_Beat},
    {7, Alto, One_FOUR_Beat},
    {0, Alto, 20 /*小衔接*/},
    {7, Alto, One_FOUR_Beat},
    {6, Alto, One_FOUR_Beat},
    {7, Alto, One_TWO_Beat},

    // 第十七小节
    {7, Alto, One_FOUR_Beat},
    {6, Alto, One_TWO_Beat},
    {3, Alto, One_FOUR_Beat},
    {3, Alto, One_TWO_Beat},
    {3, Alto, One_TWO_Beat},
    {0, Alto, One_FOUR_Beat},
    {3, Alto, One_FOUR_Beat},
    {5, Alto, One_FOUR_Beat},
    {3, Alto, One_FOUR_Beat},

    // 第十八小节
    {2, Alto, One_TWO_Beat},
    {3, Alto, One_FOUR_Beat},
    {2, Alto, One_TWO_Beat},
    {3, Alto, One_FOUR_Beat},
    {2, Alto, One_TWO_Beat},
    {3, Alto, One_FOUR_Beat},
    {5, Alto, One_FOUR_Beat},
    {3, Alto, One_FOUR_Beat},
    {5, Alto, One_FOUR_Beat},
    {3, Alto, One_FOUR_Beat},

    // 第十九小节
    {2, Alto, One_TWO_Beat},
    {3, Alto, One_FOUR_Beat},
    {2, Alto, One_TWO_Beat},
    {3, Alto, One_FOUR_Beat},
    {2, Alto, One_Beat},
    {0, Alto, One_TWO_Beat},
    {1, Alto, One_FOUR_Beat},
    {2, Alto, One_FOUR_Beat},

    // 第二十小节
    {3, Alto, One_TWO_Beat},
    {6, Bass, One_TWO_Beat},
    {1, Alto, One_TWO_Beat},
    {3, Alto, One_TWO_Beat},
    {2, Alto, One_TWO_Beat},
    {3, Alto, One_FOUR_Beat},
    {2, Alto, One_FOUR_Beat},
    {1, Alto, One_FOUR_Beat},
    {1, Alto, One_TWO_Beat},

    // 第二十一小节
    {6, Bass, One_Beat},
    {6, Bass, One_Beat},
    {0, Alto, One_Beat},
    {0, Alto, One_TWO_Beat},
    {6, Alto, One_FOUR_Beat},
    {7, Alto, One_FOUR_Beat},

    // 第二十二小节
    {1, Teble, One_FOUR_Beat},
    {2, Teble, One_FOUR_Beat},
    {7, Alto, One_FOUR_Beat},
    {1, Teble, One_FOUR_Beat},
    {0, Alto, 306 /*小衔接*/},
    {1, Teble, One_TWO_Beat},
    {0, Alto, 306 /*小衔接*/},
    {1, Teble, One_FOUR_Beat},
    {7, Alto, One_FOUR_Beat},
    {1, Teble, One_FOUR_Beat},
    {2, Teble, One_FOUR_Beat},
    {7, Alto, One_FOUR_Beat},
    {1, Teble, One_FOUR_Beat},
    {0, Alto, 306 /*小衔接*/},
    {1, Teble, One_TWO_Beat},
    {0, Alto, 306 /*小衔接*/},
    {1, Teble, One_FOUR_Beat},
    {2, Teble, One_FOUR_Beat},

    // 第二十三小节
    {3, Teble, One_FOUR_Beat},
    {2, Teble, One_FOUR_Beat},
    {3, Teble, One_FOUR_Beat},
    {2, Teble, One_FOUR_Beat},
    {3, Teble, One_TWO_Beat},
    {0, Alto, 306 /*小衔接*/},
    {3, Teble, One_FOUR_Beat},
    {2, Teble, One_FOUR_Beat},
    {3, Teble, One_TWO_Beat},
    {5, Teble, One_TWO_Beat},
    {3, Teble, One_TWO_Beat},
    {6, Alto, One_FOUR_Beat},
    {7, Alto, One_FOUR_Beat},

    // 第二十四小节
    {1, Teble, One_FOUR_Beat},
    {2, Teble, One_FOUR_Beat},
    {7, Alto, One_FOUR_Beat},
    {1, Teble, One_FOUR_Beat},
    {0, Alto, 306 /*小衔接*/},
    {1, Teble, One_TWO_Beat},
    {0, Alto, 306 /*小衔接*/},
    {1, Teble, One_FOUR_Beat},
    {7, Alto, One_FOUR_Beat},
    {1, Teble, One_FOUR_Beat},
    {2, Teble, One_FOUR_Beat},
    {7, Alto, One_FOUR_Beat},
    {1, Teble, One_FOUR_Beat},
    {0, Alto, 306 /*小衔接*/},
    {1, Teble, One_TWO_Beat},
    {0, Alto, 306 /*小衔接*/},
    {1, Teble, One_FOUR_Beat},
    {2, Teble, One_FOUR_Beat},

    // 第二十五小节
    {3, Teble, One_FOUR_Beat},
    {2, Teble, One_FOUR_Beat},
    {3, Teble, One_FOUR_Beat},
    {2, Teble, One_FOUR_Beat},
    {3, Teble, One_TWO_Beat},
    {0, Alto, 306 /*小衔接*/},
    {3, Teble, One_FOUR_Beat},
    {2, Teble, One_FOUR_Beat},
    {3, Teble, One_TWO_Beat},
    {5, Teble, One_TWO_Beat},
    {3, Teble, One_TWO_Beat},
    {5, Teble, One_TWO_Beat},

    // 第二十六小节
    {3, Teble, One_TWO_Beat},
    {5, Teble, One_FOUR_Beat},
    {3, Teble, One_TWO_Beat},
    {5, Teble, One_FOUR_Beat},
    {3, Teble, One_FOUR_Beat},
    {5, Teble, One_FOUR_Beat},
    {6, Teble, One_FOUR_Beat},
    {3, Teble, One_FOUR_Beat},
    {5, Teble, One_TWO_Beat},
    {0, Alto, 184 /*小衔接*/},
    {5, Teble, One_TWO_Beat},

    // 第二十七小节
    {3, Teble, One_TWO_Beat},
    {5, Teble, One_FOUR_Beat},
    {3, Teble, One_TWO_Beat},
    {5, Teble, One_FOUR_Beat},
    {3, Teble, One_FOUR_Beat},
    {5, Teble, One_FOUR_Beat},
    {6, Teble, One_FOUR_Beat},
    {3, Teble, One_FOUR_Beat},
    {5, Teble, One_TWO_Beat},
    {0, Alto, 184 /*小衔接*/},
    {5, Teble, One_TWO_Beat},
    {0, Alto, 184 /*小衔接*/},
    {5, Teble, One_TWO_Beat},

    // 第二十八小节
    //{3,Teble,One_FOUR_Beat},{2,Teble,One_FOUR_Beat},{0,Alto,184 /*小衔接*/},{2,Teble,One_TWO_Beat} ,{0,Alto,184 /*小衔接*/},{2,Teble,One_TWO_Beat} ,
    //{1,Teble,One_FOUR_Beat},{3,Teble,One_FOUR_Beat},{0,Alto,184 /*小衔接*/},{3,Teble,One_FOUR_Beat},{2,Teble,One_FOUR_Beat},{0,Alto,184 /*小衔接*/},
    //{2,Teble,One_TWO_Beat} ,{0,Alto,184 /*小衔接*/},{2,Teble,One_TWO_Beat} ,{1,Teble,One_FOUR_Beat},{0,Alto,184 /*小衔接*/},{1,Teble,One_FOUR_Beat},
    {3, Teble, One_FOUR_Beat},
    {3, Teble, One_FOUR_Beat},
    {2, Teble, One_TWO_Beat},
    {0, Alto, 184 /*小衔接*/},
    {2, Teble, One_TWO_Beat},
    {1, Teble, One_FOUR_Beat},
    {3, Teble, One_FOUR_Beat},
    {3, Teble, One_FOUR_Beat},
    {2, Teble, One_TWO_Beat},
    {0, Alto, 184 /*小衔接*/},
    {2, Teble, One_TWO_Beat},
    {1, Teble, One_FOUR_Beat},
    {0, Alto, 184 /*小衔接*/},
    {1, Teble, One_FOUR_Beat},

    // 第二十九小节
    {1, Teble, One_TWO_Beat},
    {0, Alto, One_FOUR_Beat},
    {0, Alto, One_TWO_Beat},
    {0, Alto, One_TWO_Beat},
    {5, Teble, One_FOUR_Beat},
    {0, Alto, 184 /*小衔接*/},
    {5, Teble, One_FOUR_Beat},

    // 第三十小节
    //{3,Teble,One_FOUR_Beat},{2,Teble,One_FOUR_Beat},{0,Alto,184 /*小衔接*/},{2,Teble,One_TWO_Beat} ,{0,Alto,184 /*小衔接*/},{2,Teble,One_TWO_Beat} ,
    //{1,Teble,One_FOUR_Beat},{3,Teble,One_FOUR_Beat},{0,Alto,184 /*小衔接*/},{3,Teble,One_FOUR_Beat},{2,Teble,One_FOUR_Beat},{0,Alto,184 /*小衔接*/},
    //{2,Teble,One_TWO_Beat} ,{0,Alto,184 /*小衔接*/},{2,Teble,One_TWO_Beat} ,{1,Teble,One_FOUR_Beat},{0,Alto,184 /*小衔接*/},{1,Teble,One_FOUR_Beat},
    {3, Teble, One_FOUR_Beat},
    {3, Teble, One_FOUR_Beat},
    {2, Teble, One_TWO_Beat},
    {0, Alto, 184 /*小衔接*/},
    {2, Teble, One_TWO_Beat},
    {1, Teble, One_FOUR_Beat},
    {3, Teble, One_FOUR_Beat},
    {3, Teble, One_FOUR_Beat},
    {2, Teble, One_TWO_Beat},
    {0, Alto, 184 /*小衔接*/},
    {2, Teble, One_TWO_Beat},
    {1, Teble, One_FOUR_Beat},
    {0, Alto, 184 /*小衔接*/},
    {1, Teble, One_FOUR_Beat},

    // 第三十一小节
    {1, Teble, One_TWO_Beat},
    {0, Alto, One_Beat},
    {0, Alto, One_Beat},
    {0, Alto, One_Beat},
    //
    //	//第三十二小节
    //	{0,Alto,One_Beat}      ,{0,Alto,One_Beat}      ,{0,Alto,One_Beat}      ,{0,Alto,One_Beat}      ,
    //
    //	//第三十三小节
    //	{0,Alto,One_Beat}      ,{0,Alto,One_Beat}      ,{0,Alto,One_Beat}      ,{0,Alto,One_Beat}      ,

    // 第三十四小节
    {0, Alto, One_Beat},
    {0, Alto, One_Beat},
    {0, Alto, One_Beat},
    {0, Alto, One_TWO_Beat},
    {6, Teble, One_FOUR_Beat},
    {5, Alto, One_FOUR_Beat},

    // 第三十五小节
    {6, Alto, One_TWO_Beat},
    {5, Alto, One_FOUR_Beat},
    {6, Alto, One_FOUR_Beat},
    {5, Alto, One_FOUR_Beat},
    {6, Alto, One_FOUR_Beat},
    {5, Alto, One_FOUR_Beat},
    {6, Alto, One_TWO_Beat},
    {0, Alto, 184 /*小衔接*/},
    {6, Alto, One_FOUR_Beat},
    {5, Alto, One_FOUR_Beat},
    {6, Alto, One_FOUR_Beat},
    {5, Alto, One_FOUR_Beat},
    {6, Alto, One_FOUR_Beat},
    {5, Alto, One_FOUR_Beat},

    // 第三十六小节
    //	{3,Alto,One_FOUR_Beat} ,{3,Alto,One_TWO_Beat}  ,{3,Alto,One_Beat}       ,{0,Alto,One_Beat}     ,
    //	{0,Alto,One_TWO_Beat}  ,{6,Alto,One_FOUR_Beat} ,{5,Alto,One_FOUR_Beat}  ,
    {3, Alto, One_TWO_Beat},
    {3, Alto, One_Beat},
    {0, Alto, One_Beat},
    {0, Alto, One_TWO_Beat},
    {6, Alto, One_FOUR_Beat},
    {5, Alto, One_FOUR_Beat},

    // 第三十七小节
    {6, Alto, One_TWO_Beat},
    {5, Alto, One_FOUR_Beat},
    {6, Alto, One_FOUR_Beat},
    {5, Alto, One_FOUR_Beat},
    {6, Alto, One_FOUR_Beat},
    {5, Alto, One_FOUR_Beat},
    {7, Alto, One_TWO_Beat},
    {0, Alto, 184 /*小衔接*/},
    {7, Alto, One_FOUR_Beat},
    {0, Alto, 184 /*小衔接*/},
    {7, Alto, One_FOUR_Beat},
    {6, Alto, One_FOUR_Beat},
    {7, Alto, One_FOUR_Beat},
    {6, Alto, One_FOUR_Beat},

    // 第三十八小节
    //	{3,Alto,One_FOUR_Beat} ,{3,Alto,One_TWO_Beat}  ,{3,Alto,One_Beat}  ,{0,Alto,One_Beat}  ,
    //	{0,Alto,One_FOUR_Beat},{3,Alto,One_FOUR_Beat}  ,{5,Alto,One_FOUR_Beat} ,{3,Alto,One_FOUR_Beat} ,
    {3, Alto, One_TWO_Beat},
    {3, Alto, One_Beat},
    {0, Alto, One_Beat},
    {0, Alto, One_FOUR_Beat},
    {3, Alto, One_FOUR_Beat},
    {5, Alto, One_FOUR_Beat},
    {3, Alto, One_FOUR_Beat},

    // 第三十九小节
    {2, Alto, One_TWO_Beat},
    {3, Alto, One_FOUR_Beat},
    {2, Alto, One_TWO_Beat},
    {3, Alto, One_FOUR_Beat},
    {2, Alto, One_TWO_Beat},
    {3, Alto, One_FOUR_Beat},
    {5, Alto, One_FOUR_Beat},
    {3, Alto, One_FOUR_Beat},
    {5, Alto, One_FOUR_Beat},
    {3, Alto, One_FOUR_Beat},

    // 第四十小节
    {2, Alto, One_TWO_Beat},
    {3, Alto, One_FOUR_Beat},
    {2, Alto, One_TWO_Beat},
    {3, Alto, One_FOUR_Beat},
    {2, Alto, One_Beat},
    {0, Alto, One_TWO_Beat},
    {1, Alto, One_FOUR_Beat},
    {2, Alto, One_FOUR_Beat},

    // 第四十一小节
    {3, Alto, One_TWO_Beat},
    {6, Alto, One_TWO_Beat},
    {1, Teble, One_TWO_Beat},
    {3, Teble, One_TWO_Beat},
    {2, Teble, One_TWO_Beat},
    {3, Teble, One_FOUR_Beat},
    {2, Teble, One_FOUR_Beat},
    {1, Teble, One_FOUR_Beat},
    {1, Teble, One_TWO_Beat},

    // 第四十二小节
    {6, Alto, One_Beat},
    {0, Alto, One_Beat},
    {0, Alto, One_Beat},
    {0, Alto, One_TWO_Beat},
    {6, Alto, One_FOUR_Beat},
    {7, Alto, One_FOUR_Beat},

    // 开始第一遍循环
    // 第二十二小节
    {1, Teble, One_FOUR_Beat},
    {2, Teble, One_FOUR_Beat},
    {7, Alto, One_FOUR_Beat},
    {1, Teble, One_FOUR_Beat},
    {0, Alto, 306 /*小衔接*/},
    {1, Teble, One_TWO_Beat},
    {0, Alto, 306 /*小衔接*/},
    {1, Teble, One_FOUR_Beat},
    {7, Alto, One_FOUR_Beat},
    {1, Teble, One_FOUR_Beat},
    {2, Teble, One_FOUR_Beat},
    {7, Alto, One_FOUR_Beat},
    {1, Teble, One_FOUR_Beat},
    {0, Alto, 306 /*小衔接*/},
    {1, Teble, One_TWO_Beat},
    {0, Alto, 306 /*小衔接*/},
    {1, Teble, One_FOUR_Beat},
    {2, Teble, One_FOUR_Beat},

    // 第二十三小节
    {3, Teble, One_FOUR_Beat},
    {2, Teble, One_FOUR_Beat},
    {3, Teble, One_FOUR_Beat},
    {2, Teble, One_FOUR_Beat},
    {3, Teble, One_TWO_Beat},
    {0, Alto, 306 /*小衔接*/},
    {3, Teble, One_FOUR_Beat},
    {2, Teble, One_FOUR_Beat},
    {3, Teble, One_TWO_Beat},
    {5, Teble, One_TWO_Beat},
    {3, Teble, One_TWO_Beat},
    {6, Alto, One_FOUR_Beat},
    {7, Alto, One_FOUR_Beat},

    // 第二十四小节
    {1, Teble, One_FOUR_Beat},
    {2, Teble, One_FOUR_Beat},
    {7, Alto, One_FOUR_Beat},
    {1, Teble, One_FOUR_Beat},
    {0, Alto, 306 /*小衔接*/},
    {1, Teble, One_TWO_Beat},
    {0, Alto, 306 /*小衔接*/},
    {1, Teble, One_FOUR_Beat},
    {7, Alto, One_FOUR_Beat},
    {1, Teble, One_FOUR_Beat},
    {2, Teble, One_FOUR_Beat},
    {7, Alto, One_FOUR_Beat},
    {1, Teble, One_FOUR_Beat},
    {0, Alto, 306 /*小衔接*/},
    {1, Teble, One_TWO_Beat},
    {0, Alto, 306 /*小衔接*/},
    {1, Teble, One_FOUR_Beat},
    {2, Teble, One_FOUR_Beat},

    // 第二十五小节
    {3, Teble, One_FOUR_Beat},
    {2, Teble, One_FOUR_Beat},
    {3, Teble, One_FOUR_Beat},
    {2, Teble, One_FOUR_Beat},
    {3, Teble, One_TWO_Beat},
    {0, Alto, 306 /*小衔接*/},
    {3, Teble, One_FOUR_Beat},
    {2, Teble, One_FOUR_Beat},
    {3, Teble, One_TWO_Beat},
    {5, Teble, One_TWO_Beat},
    {3, Teble, One_TWO_Beat},
    {5, Teble, One_TWO_Beat},

    // 第二十六小节
    {3, Teble, One_TWO_Beat},
    {5, Teble, One_FOUR_Beat},
    {3, Teble, One_TWO_Beat},
    {5, Teble, One_FOUR_Beat},
    {3, Teble, One_FOUR_Beat},
    {5, Teble, One_FOUR_Beat},
    {6, Teble, One_FOUR_Beat},
    {3, Teble, One_FOUR_Beat},
    {5, Teble, One_TWO_Beat},
    {0, Alto, 184 /*小衔接*/},
    {5, Teble, One_TWO_Beat},

    // 第二十七小节
    {3, Teble, One_TWO_Beat},
    {5, Teble, One_FOUR_Beat},
    {3, Teble, One_TWO_Beat},
    {5, Teble, One_FOUR_Beat},
    {3, Teble, One_FOUR_Beat},
    {5, Teble, One_FOUR_Beat},
    {6, Teble, One_FOUR_Beat},
    {3, Teble, One_FOUR_Beat},
    {5, Teble, One_TWO_Beat},
    {0, Alto, 184 /*小衔接*/},
    {5, Teble, One_TWO_Beat},
    {0, Alto, 184 /*小衔接*/},
    {5, Teble, One_TWO_Beat},

    // 第二十八小节
    //{3,Teble,One_FOUR_Beat},{2,Teble,One_FOUR_Beat},{0,Alto,184 /*小衔接*/},{2,Teble,One_TWO_Beat} ,{0,Alto,184 /*小衔接*/},{2,Teble,One_TWO_Beat} ,
    //{1,Teble,One_FOUR_Beat},{3,Teble,One_FOUR_Beat},{0,Alto,184 /*小衔接*/},{3,Teble,One_FOUR_Beat},{2,Teble,One_FOUR_Beat},{0,Alto,184 /*小衔接*/},
    //{2,Teble,One_TWO_Beat} ,{0,Alto,184 /*小衔接*/},{2,Teble,One_TWO_Beat} ,{1,Teble,One_FOUR_Beat},{0,Alto,184 /*小衔接*/},{1,Teble,One_FOUR_Beat},
    {3, Teble, One_FOUR_Beat},
    {3, Teble, One_FOUR_Beat},
    {2, Teble, One_TWO_Beat},
    {0, Alto, 184 /*小衔接*/},
    {2, Teble, One_TWO_Beat},
    {1, Teble, One_FOUR_Beat},
    {3, Teble, One_FOUR_Beat},
    {3, Teble, One_FOUR_Beat},
    {2, Teble, One_TWO_Beat},
    {0, Alto, 184 /*小衔接*/},
    {2, Teble, One_TWO_Beat},
    {1, Teble, One_FOUR_Beat},
    {0, Alto, 184 /*小衔接*/},
    {1, Teble, One_FOUR_Beat},

    // 第二十九小节
    {1, Teble, One_TWO_Beat},
    {0, Alto, One_FOUR_Beat},
    {0, Alto, One_TWO_Beat},
    {0, Alto, One_TWO_Beat},
    {5, Teble, One_FOUR_Beat},
    {0, Alto, 184 /*小衔接*/},
    {5, Teble, One_FOUR_Beat},

    // 第三十小节
    //{3,Teble,One_FOUR_Beat},{2,Teble,One_FOUR_Beat},{0,Alto,184 /*小衔接*/},{2,Teble,One_TWO_Beat} ,{0,Alto,184 /*小衔接*/},{2,Teble,One_TWO_Beat} ,
    //{1,Teble,One_FOUR_Beat},{3,Teble,One_FOUR_Beat},{0,Alto,184 /*小衔接*/},{3,Teble,One_FOUR_Beat},{2,Teble,One_FOUR_Beat},{0,Alto,184 /*小衔接*/},
    //{2,Teble,One_TWO_Beat} ,{0,Alto,184 /*小衔接*/},{2,Teble,One_TWO_Beat} ,{1,Teble,One_FOUR_Beat},{0,Alto,184 /*小衔接*/},{1,Teble,One_FOUR_Beat},
    {3, Teble, One_FOUR_Beat},
    {3, Teble, One_FOUR_Beat},
    {2, Teble, One_TWO_Beat},
    {0, Alto, 184 /*小衔接*/},
    {2, Teble, One_TWO_Beat},
    {1, Teble, One_FOUR_Beat},
    {3, Teble, One_FOUR_Beat},
    {3, Teble, One_FOUR_Beat},
    {2, Teble, One_TWO_Beat},
    {0, Alto, 184 /*小衔接*/},
    {2, Teble, One_TWO_Beat},
    {1, Teble, One_FOUR_Beat},
    {0, Alto, 184 /*小衔接*/},
    {1, Teble, One_FOUR_Beat},

    // 第一遍循环结束

    // 第四十三小节
    {6, Alto, One_TWO_Beat},
    {6, Alto, One_TWO_Beat},
    {1, Alto, One_TWO_Beat},
    {3, Alto, One_TWO_Beat},
    {7, Alto, One_Beat},
    {0, Alto, 184 /*小衔接*/},
    {7, Alto, One_TWO_Beat},
    {0, Alto, 184 /*小衔接*/},
    {7, Alto, One_FOUR_Beat},
    {0, Alto, 184 /*小衔接*/},
    {7, Alto, One_FOUR_Beat},

    // 第四十四小节
    //{6,Alto,One_FOUR_Beat}  ,{6,Alto,One_TWO_Beat} ,{6,Alto,One_Beat}  ,{0,Alto,One_Beat}  ,
    //{0,Alto,One_Beat},{0,Alto,One_Beat},
    {6, Alto, One_TWO_Beat},
    {6, Alto, One_TWO_Beat},
    {6, Alto, One_TWO_Beat},
    {0, Alto, One_Beat},
    {0, Alto, One_Beat},
    {0, Alto, One_Beat},

    // 第四十五小节
    {6, Alto, One_TWO_Beat},
    {6, Alto, One_TWO_Beat},
    {1, Alto, One_TWO_Beat},
    {3, Alto, One_TWO_Beat},
    {7, Alto, One_Beat},
    {0, Alto, 184 /*小衔接*/},
    {7, Alto, One_TWO_Beat},
    {0, Alto, 184 /*小衔接*/},
    {7, Alto, One_FOUR_Beat},
    {0, Alto, 184 /*小衔接*/},
    {7, Alto, One_FOUR_Beat},

    // 第四十六小节
    {7, Alto, One_FOUR_Beat},
    {6, Alto, One_TWO_Beat},
    {6, Alto, One_Beat},
    {0, Alto, One_Beat},
    {0, Alto, One_TWO_Beat},
    {6, Alto, One_FOUR_Beat},
    {7, Alto, One_FOUR_Beat},

    // 第二次循环
    // 第二十二小节
    {1, Teble, One_FOUR_Beat},
    {2, Teble, One_FOUR_Beat},
    {7, Alto, One_FOUR_Beat},
    {1, Teble, One_FOUR_Beat},
    {0, Alto, 306 /*小衔接*/},
    {1, Teble, One_TWO_Beat},
    {0, Alto, 306 /*小衔接*/},
    {1, Teble, One_FOUR_Beat},
    {7, Alto, One_FOUR_Beat},
    {1, Teble, One_FOUR_Beat},
    {2, Teble, One_FOUR_Beat},
    {7, Alto, One_FOUR_Beat},
    {1, Teble, One_FOUR_Beat},
    {0, Alto, 306 /*小衔接*/},
    {1, Teble, One_TWO_Beat},
    {0, Alto, 306 /*小衔接*/},
    {1, Teble, One_FOUR_Beat},
    {2, Teble, One_FOUR_Beat},

    // 第二十三小节
    {3, Teble, One_FOUR_Beat},
    {2, Teble, One_FOUR_Beat},
    {3, Teble, One_FOUR_Beat},
    {2, Teble, One_FOUR_Beat},
    {3, Teble, One_TWO_Beat},
    {0, Alto, 306 /*小衔接*/},
    {3, Teble, One_FOUR_Beat},
    {2, Teble, One_FOUR_Beat},
    {3, Teble, One_TWO_Beat},
    {5, Teble, One_TWO_Beat},
    {3, Teble, One_TWO_Beat},
    {6, Alto, One_FOUR_Beat},
    {7, Alto, One_FOUR_Beat},

    // 第二十四小节
    {1, Teble, One_FOUR_Beat},
    {2, Teble, One_FOUR_Beat},
    {7, Alto, One_FOUR_Beat},
    {1, Teble, One_FOUR_Beat},
    {0, Alto, 306 /*小衔接*/},
    {1, Teble, One_TWO_Beat},
    {0, Alto, 306 /*小衔接*/},
    {1, Teble, One_FOUR_Beat},
    {7, Alto, One_FOUR_Beat},
    {1, Teble, One_FOUR_Beat},
    {2, Teble, One_FOUR_Beat},
    {7, Alto, One_FOUR_Beat},
    {1, Teble, One_FOUR_Beat},
    {0, Alto, 306 /*小衔接*/},
    {1, Teble, One_TWO_Beat},
    {0, Alto, 306 /*小衔接*/},
    {1, Teble, One_FOUR_Beat},
    {2, Teble, One_FOUR_Beat},

    // 第二十五小节
    {3, Teble, One_FOUR_Beat},
    {2, Teble, One_FOUR_Beat},
    {3, Teble, One_FOUR_Beat},
    {2, Teble, One_FOUR_Beat},
    {3, Teble, One_TWO_Beat},
    {0, Alto, 306 /*小衔接*/},
    {3, Teble, One_FOUR_Beat},
    {2, Teble, One_FOUR_Beat},
    {3, Teble, One_TWO_Beat},
    {5, Teble, One_TWO_Beat},
    {3, Teble, One_TWO_Beat},
    {5, Teble, One_TWO_Beat},

    // 第二十六小节
    {3, Teble, One_TWO_Beat},
    {5, Teble, One_FOUR_Beat},
    {3, Teble, One_TWO_Beat},
    {5, Teble, One_FOUR_Beat},
    {3, Teble, One_FOUR_Beat},
    {5, Teble, One_FOUR_Beat},
    {6, Teble, One_FOUR_Beat},
    {3, Teble, One_FOUR_Beat},
    {5, Teble, One_TWO_Beat},
    {0, Alto, 184 /*小衔接*/},
    {5, Teble, One_TWO_Beat},

    // 第二十七小节
    {3, Teble, One_TWO_Beat},
    {5, Teble, One_FOUR_Beat},
    {3, Teble, One_TWO_Beat},
    {5, Teble, One_FOUR_Beat},
    {3, Teble, One_FOUR_Beat},
    {5, Teble, One_FOUR_Beat},
    {6, Teble, One_FOUR_Beat},
    {3, Teble, One_FOUR_Beat},
    {5, Teble, One_TWO_Beat},
    {0, Alto, 184 /*小衔接*/},
    {5, Teble, One_TWO_Beat},
    {0, Alto, 184 /*小衔接*/},
    {5, Teble, One_TWO_Beat},

    // 第二十八小节
    //{3,Teble,One_FOUR_Beat},{2,Teble,One_FOUR_Beat},{0,Alto,184 /*小衔接*/},{2,Teble,One_TWO_Beat} ,{0,Alto,184 /*小衔接*/},{2,Teble,One_TWO_Beat} ,
    //{1,Teble,One_FOUR_Beat},{3,Teble,One_FOUR_Beat},{0,Alto,184 /*小衔接*/},{3,Teble,One_FOUR_Beat},{2,Teble,One_FOUR_Beat},{0,Alto,184 /*小衔接*/},
    //{2,Teble,One_TWO_Beat} ,{0,Alto,184 /*小衔接*/},{2,Teble,One_TWO_Beat} ,{1,Teble,One_FOUR_Beat},{0,Alto,184 /*小衔接*/},{1,Teble,One_FOUR_Beat},
    {3, Teble, One_FOUR_Beat},
    {3, Teble, One_FOUR_Beat},
    {2, Teble, One_TWO_Beat},
    {0, Alto, 184 /*小衔接*/},
    {2, Teble, One_TWO_Beat},
    {1, Teble, One_FOUR_Beat},
    {3, Teble, One_FOUR_Beat},
    {3, Teble, One_FOUR_Beat},
    {2, Teble, One_TWO_Beat},
    {0, Alto, 184 /*小衔接*/},
    {2, Teble, One_TWO_Beat},
    {1, Teble, One_FOUR_Beat},
    {0, Alto, 184 /*小衔接*/},
    {1, Teble, One_FOUR_Beat},

    // 第二十九小节
    {1, Teble, One_TWO_Beat},
    {0, Alto, One_FOUR_Beat},
    {0, Alto, One_TWO_Beat},
    {0, Alto, One_TWO_Beat},
    {5, Teble, One_FOUR_Beat},
    {0, Alto, 184 /*小衔接*/},
    {5, Teble, One_FOUR_Beat},

    // 第三十小节
    //{3,Teble,One_FOUR_Beat},{2,Teble,One_FOUR_Beat},{0,Alto,184 /*小衔接*/},{2,Teble,One_TWO_Beat} ,{0,Alto,184 /*小衔接*/},{2,Teble,One_TWO_Beat} ,
    //{1,Teble,One_FOUR_Beat},{3,Teble,One_FOUR_Beat},{0,Alto,184 /*小衔接*/},{3,Teble,One_FOUR_Beat},{2,Teble,One_FOUR_Beat},{0,Alto,184 /*小衔接*/},
    //{2,Teble,One_TWO_Beat} ,{0,Alto,184 /*小衔接*/},{2,Teble,One_TWO_Beat} ,{1,Teble,One_FOUR_Beat},{0,Alto,184 /*小衔接*/},{1,Teble,One_FOUR_Beat},
    {3, Teble, One_FOUR_Beat},
    {3, Teble, One_FOUR_Beat},
    {2, Teble, One_TWO_Beat},
    {0, Alto, 184 /*小衔接*/},
    {2, Teble, One_TWO_Beat},
    {1, Teble, One_FOUR_Beat},
    {3, Teble, One_FOUR_Beat},
    {3, Teble, One_FOUR_Beat},
    {2, Teble, One_TWO_Beat},
    {0, Alto, 184 /*小衔接*/},
    {2, Teble, One_TWO_Beat},
    {1, Teble, One_FOUR_Beat},
    {0, Alto, 184 /*小衔接*/},
    {1, Teble, One_FOUR_Beat},

    // 第二次循环结束

    // 第四十七小节
    {1, Teble, One_TWO_Beat},
    {0, Alto, One_Beat},
    {0, Alto, One_Beat},
    {0, Alto, One_Beat},

    {0, Alto, One_Beat},
};

/* USER CODE END PT */
/* Function definition -------------------------------------------------------*/
/* USER CODE BEGIN FD */
/**
 * @Function name  MUSIC_Begin
 * @Introduce  		开始播放音乐
 * @Return 				NULL
 */
void MUSIC_Analysis(void)
{
    uint16_t MusicBeatNum = ((((sizeof(Music_Lone_Brave)) / 2) / 3) - 1);

    uint16_t MusicSpeed = Music_Lone_Brave[0][2];
    for (uint16_t i = 1; i <= MusicBeatNum; i++) {
        // BSP_Buzzer_SetFrequency(Tone_Index[Music_Lone_Brave[i][0]][Music_Lone_Brave[i][1]]);
        PassiveBuzzer_Set_Freq_Duty(Tone_Index[Music_Lone_Brave[i][0]][Music_Lone_Brave[i][1]], 50);
        // HAL_Delay(MusicSpeed/Music_Lone_Brave[i][2]);
        // mdelay(MusicSpeed/Music_Lone_Brave[i][2]);
        vTaskDelay(MusicSpeed / Music_Lone_Brave[i][2]);
    }
}

void MUSIC_Alarming()
{
    uint16_t MusicBeatNum = ((((sizeof(Music_Jingdi)) / 2) / 3) - 1);
    uint16_t MusicSpeed   = Music_Jingdi[0][2];
    for (uint16_t i = 1; i <= MusicBeatNum; i++) {
        PassiveBuzzer_Set_Freq_Duty(Tone_Index[Music_Jingdi[i][0]][Music_Jingdi[i][1]], 50);
        vTaskDelay(MusicSpeed / Music_Jingdi[i][2]);
    }
}

/* USER CODE END FD */
/************************ (C) COPYRIGHT Lesterbor *****END OF FILE****/



bool play_guyongzhe=1;
bool play_jingdi;

bool suspend;

void PlayMusic()
{
    PassiveBuzzer_Init();

    while (1) {

        MUSIC_Analysis();

        // MUSIC_Alarming();
    }
}

void Alarm()
{
    PassiveBuzzer_Init();
    while (1) {
        MUSIC_Alarming();
    }
}
