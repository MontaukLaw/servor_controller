/**
 ****************************************************************************************************
 * @file        spi.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2023-08-01
 * @brief       SPI 驱动代码
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


#ifndef __SPI_H
#define __SPI_H

#include "./SYSTEM/sys/sys.h"


/* SPI1相关定义 */
#define SPI1_SCK_GPIO_PORT              GPIOB
#define SPI1_SCK_GPIO_PIN               GPIO_PIN_3
#define SPI1_SCK_GPIO_AF                GPIO_AF5_SPI1
#define SPI1_SCK_GPIO_CLK_ENABLE()      do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)   /* PB口时钟使能 */

#define SPI1_MOSI_GPIO_PORT             GPIOB
#define SPI1_MOSI_GPIO_PIN              GPIO_PIN_5
#define SPI1_MOSI_GPIO_AF               GPIO_AF5_SPI1
#define SPI1_MOSI_GPIO_CLK_ENABLE()     do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)   /* PB口时钟使能 */

#define SPI1_SPI                        SPI1
#define SPI1_SPI_CLK_ENABLE()           do{ __HAL_RCC_SPI1_CLK_ENABLE(); }while(0)    /* SPI1时钟使能 */

/* SPI总线速度设置 */
#define SPI_SPEED_2         0
#define SPI_SPEED_4         1
#define SPI_SPEED_8         2
#define SPI_SPEED_16        3
#define SPI_SPEED_32        4
#define SPI_SPEED_64        5
#define SPI_SPEED_128       6
#define SPI_SPEED_256       7

/******************************************************************************************/

void spi1_init(void);                                   /* 初始化SPI1 */
void spi1_set_speed(uint8_t speed);                     /* 设置SPI1速度 */
uint8_t spi1_read_write_byte(uint8_t txdata);           /* SPI1读写一个字节数据 */
void spi1_write_data(uint8_t *tx_data,uint16_t size);   /* SPI1读一个字节数据 */

#endif
