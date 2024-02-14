/**
 ****************************************************************************************************
 * @file        lcd.h
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2023-08-01
 * @brief       LCD��������
 * @license     Copyright (c) 2020-2032, ������������ӿƼ����޹�˾
 ****************************************************************************************************
 * @attention
 *
 * ʵ��ƽ̨:����ԭ�� STM32G474������
 * ������Ƶ:www.yuanzige.com
 * ������̳:www.openedv.com
 * ��˾��ַ:www.alientek.com
 * �����ַ:openedv.taobao.com
 * 
 * �޸�˵��
 * V1.0 20230801
 * ��һ�η���
 *
 ****************************************************************************************************
 */

#ifndef __LCD_H
#define __LCD_H

#include "./SYSTEM/sys/sys.h"


/* LCD���Ŷ��� */
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

extern uint16_t             g_point_color;        /* Ĭ�ϻ�����ɫ */
extern uint16_t             g_back_color;         /* Ĭ�ϱ�����ɫ */

/* LCD�Ŀ�͸߶��� */
#define LCD_WIDTH           240
#define LCD_HEIGHT          240

/* ���û�����ɫ */
#define WHITE               0xFFFF      /* ��ɫ */
#define BLACK               0x0000      /* ��ɫ */
#define RED                 0xF800      /* ��ɫ */
#define BLUE                0x001F      /* ��ɫ */ 
#define YELLOW              0xFFE0      /* ��ɫ = GREEN + RED */
#define GBLUE               0X07FF      /* ��ɫ = GREEN + BLUE */  
#define MAGENTA             0xF81F      /* Ʒ��ɫ/�Ϻ�ɫ = BLUE + RED */
#define GREEN               0x07E0      /* ��ɫ */
#define CYAN                0x7FFF      /* ����ɫ */

/* �ǳ�����ɫ */
#define BROWN               0XBC40      /* ��ɫ */
#define BRRED               0XFC07      /* �غ�ɫ */
#define GRAY                0X8430      /* ��ɫ */
#define DARKBLUE            0X01CF      /* ����ɫ */
#define LIGHTBLUE           0X7D7C      /* ǳ��ɫ */
#define GRAYBLUE            0X5458      /* ����ɫ */

#define LIGHTGREEN          0X841F      /* ǳ��ɫ */
#define LGRAY               0XC618      /* ǳ��ɫ(PANNEL),���屳��ɫ */

#define LGRAYBLUE           0XA651      /* ǳ����ɫ(�м����ɫ) */
#define LBBLUE              0X2B12      /* ǳ����ɫ(ѡ����Ŀ�ķ�ɫ) */

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
