/**
 ****************************************************************************************************
 * @file        atim.c
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

#include "./BSP/TIMER/atim.h"
#include "./BSP/LED/led.h"


/********************************* 高级定时器 PWM输出 *************************************/

/* 高级定时器PWM */
TIM_HandleTypeDef g_atimx_pwm_chy_handle;      /* 定时器x句柄 */
TIM_OC_InitTypeDef g_atimx_oc_pwm_chy_handle;  /* 定时器输出句柄 */ 

/**
 * @brief       高级定时器TIMX PWM 初始化函数
 * @note
 *              高级定时器的时钟来自APB2, 而PCLK2 = 170Mhz, 我们设置PPRE2不分频, 因此
 *              高级定时器时钟 = 170Mhz
 *              定时器溢出时间计算方法: Tout = ((arr + 1) * (psc + 1)) / Ft us.
 *              Ft=定时器工作频率,单位:Mhz
 *
 * @param       arr: 自动重装值
 * @param       psc: 时钟预分频数
 * @retval      无
 */
void atim_timx_pwm_chy_init(uint16_t arr, uint16_t psc)
{
    ATIM_TIMX_PWM_CHY_CLK_ENABLE();                                     /* TIMX 时钟使能 */
    ATIM_TIMX_PWM_CH1_GPIO_CLK_ENABLE();                                /* IO时钟使能 */
    ATIM_TIMX_PWM_CH2_GPIO_CLK_ENABLE();                                /* IO时钟使能 */
    
    g_atimx_pwm_chy_handle.Instance = ATIM_TIMX_PWM;                    /* 定时器x */
    g_atimx_pwm_chy_handle.Init.Prescaler = psc;                        /* 定时器分频 */
    g_atimx_pwm_chy_handle.Init.CounterMode = TIM_COUNTERMODE_UP;       /* 向上计数模式 */
    g_atimx_pwm_chy_handle.Init.Period = arr;                           /* 自动重装载值 */
    g_atimx_pwm_chy_handle.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;   /* 分频因子 */
    g_atimx_pwm_chy_handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE; /*使能TIMx_ARR进行缓冲*/
    g_atimx_pwm_chy_handle.Init.RepetitionCounter = 0;                  /* 开始时不计数*/
    HAL_TIM_PWM_Init(&g_atimx_pwm_chy_handle);                          /* 初始化PWM */
    
    g_atimx_oc_pwm_chy_handle.OCMode = TIM_OCMODE_PWM2;                 /* 模式选择PWM2 */
    g_atimx_oc_pwm_chy_handle.Pulse = 0.5/20*(arr+1);                   /* ST芯片输出的频率是准的，但是经过三极管8050之后频率有所下降，所以需要加个参数弥补缺失部分，这里参数加4 */
    g_atimx_oc_pwm_chy_handle.OCPolarity = TIM_OCPOLARITY_HIGH;         /* 输出比较极性为高 */
    g_atimx_oc_pwm_chy_handle.OCNPolarity = TIM_OCNPOLARITY_HIGH;
    g_atimx_oc_pwm_chy_handle.OCFastMode = TIM_OCFAST_DISABLE;
    g_atimx_oc_pwm_chy_handle.OCIdleState = TIM_OCIDLESTATE_RESET;
    g_atimx_oc_pwm_chy_handle.OCNIdleState = TIM_OCNIDLESTATE_RESET;
    HAL_TIM_PWM_ConfigChannel(&g_atimx_pwm_chy_handle, &g_atimx_oc_pwm_chy_handle, ATIM_TIMX_PWM_CH1); /* 配置TIMx通道y */
    HAL_TIM_PWM_ConfigChannel(&g_atimx_pwm_chy_handle, &g_atimx_oc_pwm_chy_handle, ATIM_TIMX_PWM_CH2); /* 配置TIMx通道y */   

    HAL_TIM_PWM_Start(&g_atimx_pwm_chy_handle, ATIM_TIMX_PWM_CH1);     /* 开启对应PWM通道 */
    HAL_TIM_PWM_Start(&g_atimx_pwm_chy_handle, ATIM_TIMX_PWM_CH2);     /* 开启对应PWM通道 */
}


/**
 * @brief       定时器底层驱动，时钟使能，引脚配置
                此函数会被HAL_TIM_PWM_Init()调用
 * @param       htim:定时器句柄
 * @retval      无
 */
void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == ATIM_TIMX_PWM)
    {
        GPIO_InitTypeDef gpio_init_struct;
        ATIM_TIMX_PWM_CHY_CLK_ENABLE();                             /* 开启通道y的CPIO时钟 */
        ATIM_TIMX_PWM_CH1_GPIO_CLK_ENABLE();

        gpio_init_struct.Pin = ATIM_TIMX_PWM_CH1_GPIO_PIN;          /* 通道y的CPIO口 */
        gpio_init_struct.Mode = GPIO_MODE_AF_PP;                    /* 复用推完输出 */
        gpio_init_struct.Pull = GPIO_PULLUP;                        /* 上拉 */
        gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;              /* 高速 */
        gpio_init_struct.Alternate = ATIM_TIMX_PWM_CHY_GPIO_AF;     /* 端口复用 */
        HAL_GPIO_Init(ATIM_TIMX_PWM_CH1_GPIO_PORT, &gpio_init_struct);
        
        gpio_init_struct.Pin = ATIM_TIMX_PWM_CH2_GPIO_PIN;          /* 通道y的CPIO口 */
        HAL_GPIO_Init(ATIM_TIMX_PWM_CH2_GPIO_PORT, &gpio_init_struct);
    }
}


