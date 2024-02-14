/**
 ****************************************************************************************************
 * @file        atim.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2023-08-01
 * @brief       �߼���ʱ�� ��������
 * @license     Copyright (c) 2020-2032, ������������ӿƼ����޹�˾
 ****************************************************************************************************
 * @attention
 *
 * ʵ��ƽ̨:����ԭ�� STM32G474������
 * ������Ƶ:www.yuanzige.com
 * ������̳:www.openedv.com/forum.php
 * ��˾��ַ:www.alientek.com
 * �����ַ:openedv.taobao.com
 *
 * �޸�˵��
 * V1.0 20230801
 * ��һ�η���
 *
 ****************************************************************************************************
 */

#include "./BSP/TIMER/atim.h"
#include "./BSP/LED/led.h"


/********************************* �߼���ʱ�� PWM��� *************************************/

/* �߼���ʱ��PWM */
TIM_HandleTypeDef g_atimx_pwm_chy_handle;      /* ��ʱ��x��� */
TIM_OC_InitTypeDef g_atimx_oc_pwm_chy_handle;  /* ��ʱ�������� */ 

/**
 * @brief       �߼���ʱ��TIMX PWM ��ʼ������
 * @note
 *              �߼���ʱ����ʱ������APB2, ��PCLK2 = 170Mhz, ��������PPRE2����Ƶ, ���
 *              �߼���ʱ��ʱ�� = 170Mhz
 *              ��ʱ�����ʱ����㷽��: Tout = ((arr + 1) * (psc + 1)) / Ft us.
 *              Ft=��ʱ������Ƶ��,��λ:Mhz
 *
 * @param       arr: �Զ���װֵ
 * @param       psc: ʱ��Ԥ��Ƶ��
 * @retval      ��
 */
void atim_timx_pwm_chy_init(uint16_t arr, uint16_t psc)
{
    ATIM_TIMX_PWM_CHY_CLK_ENABLE();                                     /* TIMX ʱ��ʹ�� */
    ATIM_TIMX_PWM_CH1_GPIO_CLK_ENABLE();                                /* IOʱ��ʹ�� */
    ATIM_TIMX_PWM_CH2_GPIO_CLK_ENABLE();                                /* IOʱ��ʹ�� */
    
    g_atimx_pwm_chy_handle.Instance = ATIM_TIMX_PWM;                    /* ��ʱ��x */
    g_atimx_pwm_chy_handle.Init.Prescaler = psc;                        /* ��ʱ����Ƶ */
    g_atimx_pwm_chy_handle.Init.CounterMode = TIM_COUNTERMODE_UP;       /* ���ϼ���ģʽ */
    g_atimx_pwm_chy_handle.Init.Period = arr;                           /* �Զ���װ��ֵ */
    g_atimx_pwm_chy_handle.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;   /* ��Ƶ���� */
    g_atimx_pwm_chy_handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE; /*ʹ��TIMx_ARR���л���*/
    g_atimx_pwm_chy_handle.Init.RepetitionCounter = 0;                  /* ��ʼʱ������*/
    HAL_TIM_PWM_Init(&g_atimx_pwm_chy_handle);                          /* ��ʼ��PWM */
    
    g_atimx_oc_pwm_chy_handle.OCMode = TIM_OCMODE_PWM2;                 /* ģʽѡ��PWM2 */
    g_atimx_oc_pwm_chy_handle.Pulse = 0.5/20*(arr+1);                   /* STоƬ�����Ƶ����׼�ģ����Ǿ���������8050֮��Ƶ�������½���������Ҫ�Ӹ������ֲ�ȱʧ���֣����������4 */
    g_atimx_oc_pwm_chy_handle.OCPolarity = TIM_OCPOLARITY_HIGH;         /* ����Ƚϼ���Ϊ�� */
    g_atimx_oc_pwm_chy_handle.OCNPolarity = TIM_OCNPOLARITY_HIGH;
    g_atimx_oc_pwm_chy_handle.OCFastMode = TIM_OCFAST_DISABLE;
    g_atimx_oc_pwm_chy_handle.OCIdleState = TIM_OCIDLESTATE_RESET;
    g_atimx_oc_pwm_chy_handle.OCNIdleState = TIM_OCNIDLESTATE_RESET;
    HAL_TIM_PWM_ConfigChannel(&g_atimx_pwm_chy_handle, &g_atimx_oc_pwm_chy_handle, ATIM_TIMX_PWM_CH1); /* ����TIMxͨ��y */
    HAL_TIM_PWM_ConfigChannel(&g_atimx_pwm_chy_handle, &g_atimx_oc_pwm_chy_handle, ATIM_TIMX_PWM_CH2); /* ����TIMxͨ��y */   

    HAL_TIM_PWM_Start(&g_atimx_pwm_chy_handle, ATIM_TIMX_PWM_CH1);     /* ������ӦPWMͨ�� */
    HAL_TIM_PWM_Start(&g_atimx_pwm_chy_handle, ATIM_TIMX_PWM_CH2);     /* ������ӦPWMͨ�� */
}


/**
 * @brief       ��ʱ���ײ�������ʱ��ʹ�ܣ���������
                �˺����ᱻHAL_TIM_PWM_Init()����
 * @param       htim:��ʱ�����
 * @retval      ��
 */
void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == ATIM_TIMX_PWM)
    {
        GPIO_InitTypeDef gpio_init_struct;
        ATIM_TIMX_PWM_CHY_CLK_ENABLE();                             /* ����ͨ��y��CPIOʱ�� */
        ATIM_TIMX_PWM_CH1_GPIO_CLK_ENABLE();

        gpio_init_struct.Pin = ATIM_TIMX_PWM_CH1_GPIO_PIN;          /* ͨ��y��CPIO�� */
        gpio_init_struct.Mode = GPIO_MODE_AF_PP;                    /* ����������� */
        gpio_init_struct.Pull = GPIO_PULLUP;                        /* ���� */
        gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;              /* ���� */
        gpio_init_struct.Alternate = ATIM_TIMX_PWM_CHY_GPIO_AF;     /* �˿ڸ��� */
        HAL_GPIO_Init(ATIM_TIMX_PWM_CH1_GPIO_PORT, &gpio_init_struct);
        
        gpio_init_struct.Pin = ATIM_TIMX_PWM_CH2_GPIO_PIN;          /* ͨ��y��CPIO�� */
        HAL_GPIO_Init(ATIM_TIMX_PWM_CH2_GPIO_PORT, &gpio_init_struct);
    }
}


