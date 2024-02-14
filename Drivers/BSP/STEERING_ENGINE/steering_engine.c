/**
 ****************************************************************************************************
 * @file        steering_engine.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2023-08-01
 * @brief       舵机 驱动代码
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 STM32G474开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 * 修改说明
 * V1.0 20230801
 * 第一次发布
 *
 ****************************************************************************************************
 */
#include "./BSP/STEERING_ENGINE/steering_engine.h"
#include "./BSP/TIMER/atim.h"

/***************************************************************************************************/
extern TIM_HandleTypeDef g_atimx_pwm_chy_handle; /* 定时器x句柄 */

/**
 * @brief       舵机角度转定时器装载值
 * @param       angle:角度
 * @retval      定时器装载值
 */
uint16_t angle_to_tim_val(float angle)
{
    uint16_t ret;
    if ((angle < 0) || (angle > 180))
    {
        ret = 0;
    }
    else
    {
        /* 0° -- 500，45° -- 1000，90° -- 1500，135° -- 2000，180° -- 2500 */
        ret = (1500 + (int)((float)((angle - 90) * 100 / 9))); /* ST芯片输出的频率是准的，但是经过三极管8050之后频率有所下降，所以需要加个参数弥补缺失部分，这里参数加4 */
        if ((ret < 500) || (ret > 2500))                       /* ST芯片输出的频率是准的，但是经过三极管8050之后频率有所下降，所以需要加个参数弥补缺失部分，这里参数加4 */
        {
            ret = 0;
        }
    }
    return ret;
}

/**
 * @brief       舵机角度设定
 * @param       id:舵机编号对应舵机的接口：1~3
 * @param       angle:角度
 * @retval      0:成功
 */
uint8_t servo_angle_set(uint8_t id, float angle)
{
    uint16_t val;
    switch (id)
    {
    case 1:
        val = angle_to_tim_val(angle); /* 得到角度转换的比较值 */
        if (val != 0)
        {
            __HAL_TIM_SetCompare(&g_atimx_pwm_chy_handle, TIM_CHANNEL_1, val); /* 设置比较值 */
        }
        break;
    case 2:
        val = angle_to_tim_val(angle);
        if (val != 0)
        {
            __HAL_TIM_SetCompare(&g_atimx_pwm_chy_handle, TIM_CHANNEL_2, val);
        }
        break;
    default:
        break;
    }
    return 0;
}
