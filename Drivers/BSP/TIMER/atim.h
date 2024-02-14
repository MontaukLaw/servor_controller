/**
 ****************************************************************************************************
 * @file        atim.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2023-08-01
 * @brief       高级定时器 驱动代码
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 STM32G474开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com/forum.php
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 * 修改说明
 * V1.0 20230801
 * 第一次发布
 *
 ****************************************************************************************************
 */

#ifndef __ATIM_H
#define __ATIM_H

#include "./SYSTEM/sys/sys.h"

/********************************* 高级定时器 PWM输出实验相关宏定义 *************************************/

/* TIMX PWM输出定义 */

#define ATIM_TIMX_PWM_CH1_GPIO_PORT            GPIOC
#define ATIM_TIMX_PWM_CH1_GPIO_PIN             GPIO_PIN_6
#define ATIM_TIMX_PWM_CH1_GPIO_CLK_ENABLE()    do{  __HAL_RCC_GPIOC_CLK_ENABLE(); }while(0)     /* PC口时钟使能 */

#define ATIM_TIMX_PWM_CH2_GPIO_PORT            GPIOC
#define ATIM_TIMX_PWM_CH2_GPIO_PIN             GPIO_PIN_7
#define ATIM_TIMX_PWM_CH2_GPIO_CLK_ENABLE()    do{  __HAL_RCC_GPIOC_CLK_ENABLE(); }while(0)     /* PC口时钟使能 */

/* TIMX 引脚复用设置
 * 因为PC6 PC7默认并不是TIM8的功能脚, 必须开启复用, PC6 PC7才能用作TIM8的PWM输出引脚.
 */
#define ATIM_TIMX_PWM_CHY_GPIO_AF              GPIO_AF4_TIM8

#define ATIM_TIMX_PWM                          TIM8
#define ATIM_TIMX_PWM_IRQn                     TIM8_UP_IRQn
#define ATIM_TIMX_PWM_IRQHandler               TIM8_UP_IRQHandler
#define ATIM_TIMX_PWM_CH1                      TIM_CHANNEL_1                                    /* 通道1 */
#define ATIM_TIMX_PWM_CH2                      TIM_CHANNEL_2                                    /* 通道2 */
#define ATIM_TIMX_PWM_CHY_CLK_ENABLE()         do{ __HAL_RCC_TIM8_CLK_ENABLE(); }while(0)       /* TIM8 时钟使能 */

/******************************************************************************************/

void atim_timx_pwm_chy_init(uint16_t arr, uint16_t psc);                                        /* 高级定时器 PWM输出初始化函数 */

#endif

















