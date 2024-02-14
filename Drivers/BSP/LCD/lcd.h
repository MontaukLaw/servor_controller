/**
 ****************************************************************************************************
 * @file        lcd.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2023-08-01
 * @brief       LCD驱动代码
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

#ifndef __LCD_H
#define __LCD_H

#include "./SYSTEM/sys/sys.h"


/* LCD引脚定义 */
#define LCD_PWR_GPIO_PORT          GPIOD
#define LCD_PWR_GPIO_PIN           GPIO_PIN_8
#define LCD_PWR_GPIO_CLK_ENABLE()  do{ __HAL_RCC_GPIOD_CLK_ENABLE(); }while(0)

#define LCD_RST_GPIO_PORT          GPIOD
#define LCD_RST_GPIO_PIN           GPIO_PIN_9
#define LCD_RST_GPIO_CLK_ENABLE()  do{ __HAL_RCC_GPIOD_CLK_ENABLE(); }while(0)

#define LCD_WR_GPIO_PORT           GPIOB
#define LCD_WR_GPIO_PIN            GPIO_PIN_4
#define LCD_WR_GPIO_CLK_ENABLE()   do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)

#define LCD_CS_GPIO_PORT           GPIOD
#define LCD_CS_GPIO_PIN            GPIO_PIN_10
#define LCD_CS_GPIO_CLK_ENABLE()   do{ __HAL_RCC_GPIOD_CLK_ENABLE(); }while(0)

#define LCD_PWR(x)                 do{ x ? \
                                    (HAL_GPIO_WritePin(LCD_PWR_GPIO_PORT, LCD_PWR_GPIO_PIN, GPIO_PIN_SET)):       \
                                    (HAL_GPIO_WritePin(LCD_PWR_GPIO_PORT, LCD_PWR_GPIO_PIN, GPIO_PIN_RESET));     \
                                    }while(0)
#define LCD_RST(x)                 do{ x ? \
                                    (HAL_GPIO_WritePin(LCD_RST_GPIO_PORT, LCD_RST_GPIO_PIN, GPIO_PIN_SET)):       \
                                    (HAL_GPIO_WritePin(LCD_RST_GPIO_PORT, LCD_RST_GPIO_PIN, GPIO_PIN_RESET));     \
                                    }while(0)
#define LCD_WR(x)                  do{ x ? \
                                    (HAL_GPIO_WritePin(LCD_WR_GPIO_PORT, LCD_WR_GPIO_PIN, GPIO_PIN_SET)):         \
                                    (HAL_GPIO_WritePin(LCD_WR_GPIO_PORT, LCD_WR_GPIO_PIN,GPIO_PIN_RESET));        \
                                    }while(0)
#define LCD_CS(x)                  do{ x ? \
                                    (HAL_GPIO_WritePin(LCD_CS_GPIO_PORT, LCD_CS_GPIO_PIN, GPIO_PIN_SET)):         \
                                    (HAL_GPIO_WritePin(LCD_CS_GPIO_PORT, LCD_CS_GPIO_PIN, GPIO_PIN_RESET));       \
                                    }while(0)

extern uint16_t             g_point_color;        /* 默认画笔颜色 */
extern uint16_t             g_back_color;         /* 默认背景颜色 */

/* LCD的宽和高定义 */
#define LCD_WIDTH           240
#define LCD_HEIGHT          240

/* 常用画笔颜色 */
#define WHITE               0xFFFF      /* 白色 */
#define BLACK               0x0000      /* 黑色 */
#define RED                 0xF800      /* 红色 */
#define BLUE                0x001F      /* 蓝色 */ 
#define YELLOW              0xFFE0      /* 黄色 = GREEN + RED */
#define GBLUE               0X07FF      /* 青色 = GREEN + BLUE */  
#define MAGENTA             0xF81F      /* 品红色/紫红色 = BLUE + RED */
#define GREEN               0x07E0      /* 绿色 */
#define CYAN                0x7FFF      /* 青蓝色 */

/* 非常用颜色 */
#define BROWN               0XBC40      /* 棕色 */
#define BRRED               0XFC07      /* 棕红色 */
#define GRAY                0X8430      /* 灰色 */
#define DARKBLUE            0X01CF      /* 深蓝色 */
#define LIGHTBLUE           0X7D7C      /* 浅蓝色 */
#define GRAYBLUE            0X5458      /* 灰蓝色 */

#define LIGHTGREEN          0X841F      /* 浅绿色 */
#define LGRAY               0XC618      /* 浅灰色(PANNEL),窗体背景色 */

#define LGRAYBLUE           0XA651      /* 浅灰蓝色(中间层颜色) */
#define LBBLUE              0X2B12      /* 浅棕蓝色(选择条目的反色) */

/******************************************************************************************/

void lcd_init(void);
void lcd_show_image(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const uint8_t *p);
void lcd_show_string(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t size, char *p, uint16_t color);
void lcd_show_xnum(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size, uint8_t mode, uint16_t color);
void lcd_show_num(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size, uint16_t color);
void lcd_show_char(uint16_t x, uint16_t y, char chr, uint8_t size, uint8_t mode ,uint16_t color);
void lcd_draw_circle(uint16_t x0, uint16_t y0, uint8_t r,uint16_t color);
void lcd_draw_rectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,uint16_t color);
void lcd_draw_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,uint16_t color);
void lcd_draw_color_point(uint16_t x, uint16_t y,uint16_t color);
void lcd_draw_point(uint16_t x, uint16_t y,uint16_t color);
void lcd_fill(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end, uint16_t color);
void lcd_clear(uint16_t color);
void lcd_display_off(void);
void lcd_display_on(void);
void lcd_address_set(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void lcd_write_halfword(const uint16_t da);

#endif
