/**
 ****************************************************************************************************
 * @file        lcd.c
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

#include "./BSP/LCD/lcd.h"
#include "./BSP/SPI/spi.h"
#include "./BSP/LCD/font.h"
#include "./SYSTEM/delay/delay.h"


/* LCD缓存大小设置，修改此值时请注意！！！！修改这两个值时可能会影响以下函数 lcd_clear/lcd_fill/lcd_draw_line */
#define LCD_TOTAL_BUF_SIZE      (LCD_WIDTH * LCD_HEIGHT * 2)
#define LCD_BUF_SIZE            11520
static uint8_t lcd_buf[LCD_BUF_SIZE];

uint16_t    g_point_color = BLACK;                                      /* 画笔颜色 默认为黑色 */
uint16_t    g_back_color = WHITE;                                       /* 背景颜色 默认为白色 */

/**
 * @brief       LCD控制接口初始化
 * @param       无
 * @retval      无
 */
static void lcd_gpio_init(void)
{
    GPIO_InitTypeDef gpio_init_struct = {0};
    
    LCD_PWR_GPIO_CLK_ENABLE();
    LCD_RST_GPIO_CLK_ENABLE();
    LCD_WR_GPIO_CLK_ENABLE();
    LCD_CS_GPIO_CLK_ENABLE();
    
    gpio_init_struct.Pin = LCD_PWR_GPIO_PIN;                            /* LCD_PWR引脚 */
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;                        /* 推挽输出 */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;                      /* 高速 */
    gpio_init_struct.Pull = GPIO_PULLUP;                                /* 上拉 */
    HAL_GPIO_Init(LCD_PWR_GPIO_PORT, &gpio_init_struct);                /* 初始化LCD_PWR引脚 */
    
    gpio_init_struct.Pin = LCD_RST_GPIO_PIN;                            /* LCD_RST引脚 */
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;                        /* 推挽输出 */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;                      /* 高速 */
    gpio_init_struct.Pull = GPIO_PULLUP;                                /* 上拉 */
    HAL_GPIO_Init(LCD_RST_GPIO_PORT, &gpio_init_struct);                /* 初始化LCD_RST引脚 */
    
    gpio_init_struct.Pin = LCD_WR_GPIO_PIN;                             /* LCD_WR引脚 */
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;                        /* 推挽输出 */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;                      /* 高速 */
    gpio_init_struct.Pull = GPIO_PULLUP;                                /* 上拉 */
    HAL_GPIO_Init(LCD_WR_GPIO_PORT, &gpio_init_struct);                 /* 初始化LCD_WR引脚 */
    
    gpio_init_struct.Pin = LCD_CS_GPIO_PIN;                             /* LCD_CS引脚 */
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;                        /* 推挽输出 */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;                      /* 高速 */
    gpio_init_struct.Pull = GPIO_PULLUP;                                /* 上拉 */
    HAL_GPIO_Init(LCD_CS_GPIO_PORT, &gpio_init_struct);                 /* 初始化LCD_CS引脚 */
    
    LCD_CS(0);                                                          /* 使能片选 */
    LCD_PWR(0);

    LCD_RST(0);
    delay_ms(120);
    LCD_RST(1);

    spi1_init();                                                        /* 初始化SPI1接口 */
}

/**
 * @brief       LCD底层SPI发送数据函数
 * @param       data    数据的起始地址
 * @param       size    发送数据大小
 * @retval      无
 */
static void lcd_spi_send(uint8_t *data, uint32_t size)
{
    uint32_t i;
    uint32_t delta;
    LCD_CS(0);                          /* 使能片选 */
    delta = size / 0xFFFF;
    
    for (i = 0; i <= delta; i ++)
    {
        if( i == delta )                /* 发送最后一帧数据 */
        {
            spi1_write_data(&data[i * 0xFFFF], size % 0xFFFF);
        }
        else                            /* 超长数据一次发送0xFFFF字节数据 */
        {
            spi1_write_data(&data[i * 0xFFFF], 0xFFFF);
        }
    }
    LCD_CS(1);                          /* 取消片选 */
}

/**
 * @brief       写命令到LCD
 * @param       cmd 需要发送的命令
 * @retval      无
 */
static void lcd_write_cmd(uint8_t cmd)
{
    LCD_WR(0);

    lcd_spi_send(&cmd, 1);
}

/**
 * @brief       写数据到LCD
 * @param       cmd 需要发送的数据
 * @retval      无
 */
static void lcd_write_data(uint8_t data)
{
    LCD_WR(1);

    lcd_spi_send(&data, 1);
}

/**
 * @brief       写半个字的数据到LCD
 * @param       cmd 需要发送的数据
 * @retval      无
 */
void lcd_write_halfword(const uint16_t da)
{
    uint8_t data[2] = {0};

    data[0] = da >> 8;
    data[1] = da;

    LCD_WR(1);
    lcd_spi_send(data, 2);
}

/**
 * @brief       设置数据写入LCD缓存区域
 * @param       x1,y1 起点坐标
 * @param       x2,y2 终点坐标
 * @retval      无
 */
void lcd_address_set(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    lcd_write_cmd(0x2a);
    lcd_write_data(x1 >> 8);
    lcd_write_data(x1);
    lcd_write_data(x2 >> 8);
    lcd_write_data(x2);

    lcd_write_cmd(0x2b);
    lcd_write_data(y1 >> 8);
    lcd_write_data(y1);
    lcd_write_data(y2 >> 8);
    lcd_write_data(y2);

    lcd_write_cmd(0x2C);
}

/**
 * @brief       打开LCD显示
 * @param       无
 * @retval      无
 */
void lcd_display_on(void)
{
    LCD_PWR(1);
}

/**
 * @brief       关闭LCD显示
 * @param       无
 * @retval      无
 */
void lcd_display_off(void)
{
    LCD_PWR(0);
}

/**
 * @brief       以一种颜色清空LCD屏
 * @param       color 清屏颜色
 * @retval      无
 */
void lcd_clear(uint16_t color)
{
    uint16_t i, j;
    uint8_t data[2] = {0};

    data[0] = color >> 8;
    data[1] = color;

    lcd_address_set(0, 0, LCD_WIDTH - 1, LCD_HEIGHT - 1);

    for(j = 0; j < LCD_BUF_SIZE / 2; j++)
    {
        lcd_buf[j * 2] =  data[0];
        lcd_buf[j * 2 + 1] =  data[1];
    }

    LCD_WR(1);

    for(i = 0; i < (LCD_TOTAL_BUF_SIZE / LCD_BUF_SIZE); i++)
    {
        lcd_spi_send(lcd_buf, LCD_BUF_SIZE);
    }
}

/**
 * @brief       以一种颜色清空LCD屏
 * @param       x_start,y_start 起点坐标
 * @param       x_end,y_end 终点坐标
 * @param       color 填充颜色
 * @retval      无
 */
void lcd_fill(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end, uint16_t color)
{
    uint16_t i = 0;
    uint32_t size = 0, size_remain = 0;

    size = (x_end - x_start + 1) * (y_end - y_start + 1) * 2;

    if(size > LCD_BUF_SIZE)
    {
        size_remain = size - LCD_BUF_SIZE;
        size = LCD_BUF_SIZE;
    }

    lcd_address_set(x_start, y_start, x_end, y_end);

    while(1)
    {
        for(i = 0; i < size / 2; i++)
        {
            lcd_buf[2 * i] = color >> 8;
            lcd_buf[2 * i + 1] = color;
        }

        LCD_WR(1);
        lcd_spi_send(lcd_buf, size);

        if(size_remain == 0)
            break;

        if(size_remain > LCD_BUF_SIZE)
        {
            size_remain = size_remain - LCD_BUF_SIZE;
        }

        else
        {
            size = size_remain;
            size_remain = 0;
        }
    }
}

/**
 * @brief       画点函数
 * @param       x,y 画点坐标
 * @retval      无
 */
void lcd_draw_point(uint16_t x, uint16_t y,uint16_t color)
{
    lcd_address_set(x, y, x, y);
    lcd_write_halfword(color);
}

/**
 * @brief       画点带颜色函数
 * @param       x,y 画点坐标
 * @retval      无
 */
void lcd_draw_color_point(uint16_t x, uint16_t y,uint16_t color)
{
    lcd_address_set(x, y, x, y);
    lcd_write_halfword(color);
}

/**
 * @brief       画线函数(直线、斜线)
 * @param       x1,y1   起点坐标
 * @param       x2,y2   终点坐标
 * @retval      无
 */
void lcd_draw_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,uint16_t color)
{
    uint16_t t;
    int xerr = 0, yerr = 0, delta_x, delta_y, distance;
    int incx, incy, row, col;
    uint32_t i = 0;

    if (y1 == y2)
    {
        /*快速画水平线*/
        lcd_address_set(x1, y1, x2, y2);

        for (i = 0; i < x2 - x1; i++)
        {
            lcd_buf[2 * i] = g_point_color >> 8;
            lcd_buf[2 * i + 1] = g_point_color;
        }

        LCD_WR(1);
        lcd_spi_send(lcd_buf, (x2 - x1) * 2);
        return;
    }

    delta_x = x2 - x1;
    delta_y = y2 - y1;
    row = x1;
    col = y1;

    if (delta_x > 0)
    {
        incx = 1;
    }
    else if (delta_x == 0)
    {
        incx = 0;
    }
    else
    {
        incx = -1;
        delta_x = -delta_x;
    }

    if (delta_y > 0)
    {
        incy = 1;
    }
    else if (delta_y == 0)
    {
        incy = 0;
    }
    else
    {
        incy = -1;
        delta_y = -delta_y;
    }

    if (delta_x > delta_y)
    {
        distance = delta_x;
    }
    else
    {
        distance = delta_y;
    }

    for (t = 0; t <= distance + 1; t++)
    {
        lcd_draw_point(row, col,color);
        xerr += delta_x ;
        yerr += delta_y ;

        if (xerr > distance)
        {
            xerr -= distance;
            row += incx;
        }

        if (yerr > distance)
        {
            yerr -= distance;
            col += incy;
        }
    }
}

/**
 * @brief       画一个矩形
 * @param       x1,y1   起点坐标
 * @param       x2,y2   终点坐标
 * @retval      无
 */
void lcd_draw_rectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,uint16_t color)
{
    lcd_draw_line(x1, y1, x2, y1,color);
    lcd_draw_line(x1, y1, x1, y2,color);
    lcd_draw_line(x1, y2, x2, y2,color);
    lcd_draw_line(x2, y1, x2, y2,color);
}

/**
 * @brief       画一个圆
 * @param       x0,y0   圆心坐标
 * @param       r   圆半径
 * @retval      无
 */
void lcd_draw_circle(uint16_t x0, uint16_t y0, uint8_t r,uint16_t color)
{
    int a, b;
    int di;
    a = 0;
    b = r;
    di = 3 - (r << 1);

    while (a <= b)
    {
        lcd_draw_point(x0 - b, y0 - a, color);
        lcd_draw_point(x0 + b, y0 - a, color);
        lcd_draw_point(x0 - a, y0 + b, color);
        lcd_draw_point(x0 - b, y0 - a, color);
        lcd_draw_point(x0 - a, y0 - b, color);
        lcd_draw_point(x0 + b, y0 + a, color);
        lcd_draw_point(x0 + a, y0 - b, color);
        lcd_draw_point(x0 + a, y0 + b, color);
        lcd_draw_point(x0 - b, y0 + a, color);
        a++;

        if (di < 0)
        {
            di += 4 * a + 6;
        }
        else
        {
            di += 10 + 4 * (a - b);
            b--;
        }

        lcd_draw_point(x0 + a, y0 + b, color);
    }
}

/**
 * @brief       显示一个字符
 * @param       x,y     显示起始坐标
 * @param       chr     需要显示的字符
 * @param       size    字体大小(支持16/24/32号字体)
 * @param       mode    叠加方式(1); 非叠加方式(0);
 * @retval      无
 */
void lcd_show_char(uint16_t x, uint16_t y, char chr, uint8_t size, uint8_t mode ,uint16_t color)
{
    uint8_t temp = 0,t1 = 0, t = 0;
    uint8_t *pfont = 0;
    uint8_t csize = 0;                                              /* 得到字体一个字符对应点阵集所占的字节数 */
    uint16_t colortemp = 0;
    uint8_t sta = 0;

    chr = chr - ' ';                                                /* 得到偏移后的值（ASCII字库是从空格开始取模，所以-' '就是对应字符的字库） */

    if ((x > (LCD_WIDTH - size / 2)) || (y > (LCD_HEIGHT - size)))
    {
        return;
    }

    lcd_address_set(x, y, x + size / 2 - 1, y + size - 1);          /* (x,y,x+8-1,y+16-1) */

    switch (size)
    {
        case 12:
            pfont = (uint8_t *)asc2_1206[chr];                      /* 调用1206字体 */
            break;

        case 16:
            pfont = (uint8_t *)asc2_1608[chr];                      /* 调用1608字体 */
            break;

        case 24:
            pfont = (uint8_t *)asc2_2412[chr];                      /* 调用2412字体 */
            break;

        case 32:
            pfont = (uint8_t *)asc2_3216[chr];                      /* 调用3216字体 */
            break;

        default:
            return ;
    }
    
    if (size != 24)
    {
        csize = (size / 8 + ((size % 8) ? 1 : 0)) * (size / 2);
        
        for (t = 0; t < csize; t++)
        {
            temp = pfont[t];                                        /* 获取字符的点阵数据 */
            if(size == 12)
            {
                for(t1 = 0; t1 < 6; t1++)
                {
                    if(temp & 0x80)
                    {
                         colortemp = color;
                    }                       
                    else if (mode == 0) 
                    {
                        colortemp = g_back_color;
                    }
                    lcd_write_halfword(colortemp);
                    temp <<= 1;
                }
            }else if(size == 16 || size == 32)
            {
                for (t1 = 0; t1 < 8; t1++)
                {
                        if (temp & 0x80)
                        {
                            colortemp = color;
                        }
                        else if (mode == 0)                         /* 无效点,不显示 */
                        {
                            colortemp = g_back_color;
                        }

                        lcd_write_halfword(colortemp);
                        temp <<= 1;
                }
            }
            

        }
    }
    else
    {
        csize = (size * 16) / 8;
        
        for (t = 0; t < csize; t++)
        {
            temp = asc2_2412[chr][t];

            if (t % 2 == 0)
            {
                sta = 8;
            }
            else
            {
                sta = 4;
            }

            for (t1 = 0; t1 < sta; t1++)
            {
                if(temp & 0x80)
                {
                    colortemp = color;
                }
                else if (mode == 0)                                 /* 无效点,不显示 */
                {
                    colortemp = g_back_color;
                }

                lcd_write_halfword(colortemp);
                temp <<= 1;
            }
        }
    }
}

/**
 * @brief       m^n函数
 * @param       m,n     输入参数
 * @retval      m^n次方
 */
static uint32_t lcd_pow(uint8_t m, uint8_t n)
{
    uint32_t result = 1;

    while(n--)result *= m;

    return result;
}

/**
 * @brief       显示len个数字
 * @param       x,y : 起始坐标
 * @param       num : 数值(0 ~ 2^32)
 * @param       len : 显示数字的位数
 * @param       size: 选择字体 12/16/24/32
 * @retval      无
 */
void lcd_show_num(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size, uint16_t color)
{
    uint8_t t, temp;
    uint8_t enshow = 0;

    for (t = 0; t < len; t++)   /* 按总显示位数循环 */
    {
        temp = (num / lcd_pow(10, len - t - 1)) % 10;   /* 获取对应位的数字 */

        if (enshow == 0 && t < (len - 1))   /* 没有使能显示,且还有位要显示 */
        {
            if (temp == 0)
            {
                lcd_show_char(x + (size / 2)*t, y, ' ', size, 0, color);/* 显示空格,占位 */
                continue;   /* 继续下个一位 */
            }
            else
            {
                enshow = 1; /* 使能显示 */
            }

        }

        lcd_show_char(x + (size / 2)*t, y, temp + '0', size, 0, color); /* 显示字符 */
    }
}

/**
 * @brief       扩展显示len个数字(高位是0也显示)
 * @param       x,y : 起始坐标
 * @param       num : 数值(0 ~ 2^32)
 * @param       len : 显示数字的位数
 * @param       size: 选择字体 12/16/24/32
 * @param       mode: 显示模式
 *              [7]:0,不填充;1,填充0.
 *              [6:1]:保留
 *              [0]:0,非叠加显示;1,叠加显示.
 *
 * @retval      无
 */
void lcd_show_xnum(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size, uint8_t mode, uint16_t color)
{
    uint8_t t, temp;
    uint8_t enshow = 0;

    for (t = 0; t < len; t++)   /* 按总显示位数循环 */
    {
        temp = (num / lcd_pow(10, len - t - 1)) % 10;    /* 获取对应位的数字 */

        if (enshow == 0 && t < (len - 1))   /* 没有使能显示,且还有位要显示 */
        {
            if (temp == 0)
            {
                if (mode & 0X80)   /* 高位需要填充0 */
                {
                    lcd_show_char(x + (size / 2)*t, y, '0', size, mode & 0X01, color);  /* 用0占位 */
                }
                else
                {
                    lcd_show_char(x + (size / 2)*t, y, ' ', size, mode & 0X01, color);  /* 用空格占位 */
                }

                continue;
            }
            else
            {
                enshow = 1; /* 使能显示 */
            }

        }

        lcd_show_char(x + (size / 2)*t, y, temp + '0', size, mode & 0X01, color);
    }
}

/**
 * @brief       显示字符串
 * @param       x,y         : 起始坐标
 * @param       width,height: 区域大小
 * @param       size        : 选择字体 12/16/24/32
 * @param       p           : 字符串首地址
 * @retval      无
 */
void lcd_show_string(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t size, char *p, uint16_t color)
{
    uint8_t x0 = x;
    width += x;
    height += y;

    while ((*p <= '~') && (*p >= ' '))   /* 判断是不是非法字符! */
    {
        if (x >= width)
        {
            x = x0;
            y += size;
        }

        if (y >= height)break;  /* 退出 */

        lcd_show_char(x, y, *p, size, 0, color);
        x += size / 2;
        p++;
    }
}

/**
 * @brief       显示图片
 *
 * @remark      Image2Lcd取模方式： C语言数据/水平扫描/16位真彩色(RGB565)/高位在前  其他的不要选
 * @param       x,y     起点坐标
 * @param       width   图片宽度
 * @param       height  图片高度
 * @param       p       图片缓存数据起始地址
 * @retval      无
 */
void lcd_show_image(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const uint8_t *p)
{
    if (x + width > LCD_WIDTH || y + height > LCD_HEIGHT)
    {
        return;
    }

    lcd_address_set(x, y, x + width - 1, y + height - 1);

    LCD_WR(1);

    lcd_spi_send((uint8_t *)p, width * height * 2);
}

/**
 * @brief       LCD初始化
 * @remark      无
 * @retval      无
 */
void lcd_init(void)
{
    lcd_gpio_init();            /* 硬件接口初始化 */

    delay_ms(120);
    /* Sleep Out */
    lcd_write_cmd(0x11);
    /* wait for power stability */
    delay_ms(120);

    /* Memory Data Access Control */
    lcd_write_cmd(0x36);
    lcd_write_data(0x00);

    /* RGB 5-6-5-bit  */
    lcd_write_cmd(0x3A);
    lcd_write_data(0x65);

    /* Porch Setting */
    lcd_write_cmd(0xB2);
    lcd_write_data(0x0C);
    lcd_write_data(0x0C);
    lcd_write_data(0x00);
    lcd_write_data(0x33);
    lcd_write_data(0x33);

    /*  Gate Control */
    lcd_write_cmd(0xB7);
    lcd_write_data(0x72);

    /* VCOM Setting */
    lcd_write_cmd(0xBB);
    lcd_write_data(0x3D);

    /* LCM Control */
    lcd_write_cmd(0xC0);
    lcd_write_data(0x2C);

    /* VDV and VRH Command Enable */
    lcd_write_cmd(0xC2);
    lcd_write_data(0x01);

    /* VRH Set */
    lcd_write_cmd(0xC3);
    lcd_write_data(0x19);

    /* VDV Set */
    lcd_write_cmd(0xC4);
    lcd_write_data(0x20);

    /* Frame Rate Control in Normal Mode */
    lcd_write_cmd(0xC6);
    lcd_write_data(0x0F);

    /* Power Control 1 */
    lcd_write_cmd(0xD0);
    lcd_write_data(0xA4);
    lcd_write_data(0xA1);

    /* Positive Voltage Gamma Control */
    lcd_write_cmd(0xE0);
    lcd_write_data(0xD0);
    lcd_write_data(0x04);
    lcd_write_data(0x0D);
    lcd_write_data(0x11);
    lcd_write_data(0x13);
    lcd_write_data(0x2B);
    lcd_write_data(0x3F);
    lcd_write_data(0x54);
    lcd_write_data(0x4C);
    lcd_write_data(0x18);
    lcd_write_data(0x0D);
    lcd_write_data(0x0B);
    lcd_write_data(0x1F);
    lcd_write_data(0x23);

    /* Negative Voltage Gamma Control */
    lcd_write_cmd(0xE1);
    lcd_write_data(0xD0);
    lcd_write_data(0x04);
    lcd_write_data(0x0C);
    lcd_write_data(0x11);
    lcd_write_data(0x13);
    lcd_write_data(0x2C);
    lcd_write_data(0x3F);
    lcd_write_data(0x44);
    lcd_write_data(0x51);
    lcd_write_data(0x2F);
    lcd_write_data(0x1F);
    lcd_write_data(0x1F);
    lcd_write_data(0x20);
    lcd_write_data(0x23);

    /* Display Inversion On */
    lcd_write_cmd(0x21);

    lcd_write_cmd(0x29);

    lcd_address_set(0, 0, LCD_WIDTH - 1, LCD_HEIGHT - 1);

    lcd_clear(WHITE);

    LCD_PWR(1);                 /* 打开显示 */
}
