/**
 ****************************************************************************************************
 * @file        atim.h
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

#ifndef __ATIM_H
#define __ATIM_H

#include "./SYSTEM/sys/sys.h"

/********************************* �߼���ʱ�� PWM���ʵ����غ궨�� *************************************/

/* TIMX PWM������� */

#define ATIM_TIMX_PWM_CH1_GPIO_PORT            GPIOC
#define ATIM_TIMX_PWM_CH1_GPIO_PIN             GPIO_PIN_6
#define ATIM_TIMX_PWM_CH1_GPIO_CLK_ENABLE()    do{  __HAL_RCC_GPIOC_CLK_ENABLE(); }while(0)     /* PC��ʱ��ʹ�� */

#define ATIM_TIMX_PWM_CH2_GPIO_PORT            GPIOC
#define ATIM_TIMX_PWM_CH2_GPIO_PIN             GPIO_PIN_7
#define ATIM_TIMX_PWM_CH2_GPIO_CLK_ENABLE()    do{  __HAL_RCC_GPIOC_CLK_ENABLE(); }while(0)     /* PC��ʱ��ʹ�� */

/* TIMX ���Ÿ�������
 * ��ΪPC6 PC7Ĭ�ϲ�����TIM8�Ĺ��ܽ�, ���뿪������, PC6 PC7��������TIM8��PWM�������.
 */
#define ATIM_TIMX_PWM_CHY_GPIO_AF              GPIO_AF4_TIM8

#define ATIM_TIMX_PWM                          TIM8
#define ATIM_TIMX_PWM_IRQn                     TIM8_UP_IRQn
#define ATIM_TIMX_PWM_IRQHandler               TIM8_UP_IRQHandler
#define ATIM_TIMX_PWM_CH1                      TIM_CHANNEL_1                                    /* ͨ��1 */
#define ATIM_TIMX_PWM_CH2                      TIM_CHANNEL_2                                    /* ͨ��2 */
#define ATIM_TIMX_PWM_CHY_CLK_ENABLE()         do{ __HAL_RCC_TIM8_CLK_ENABLE(); }while(0)       /* TIM8 ʱ��ʹ�� */

/******************************************************************************************/

void atim_timx_pwm_chy_init(uint16_t arr, uint16_t psc);                                        /* �߼���ʱ�� PWM�����ʼ������ */

#endif

















