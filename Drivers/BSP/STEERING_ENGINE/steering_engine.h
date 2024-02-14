/**
 ****************************************************************************************************
 * @file        steering_engine.h
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
#ifndef __STEERING_ENGINE_H
#define __STEERING_ENGINE_H

#include "./SYSTEM/sys/sys.h"


/******************************************************************************************/
/* 外部接口函数*/
uint16_t angle_to_tim_val(float angle);                     /* 角度转比较值 */
uint8_t servo_angle_set(uint8_t id,float angle);            /* 设置角度 */
#endif
