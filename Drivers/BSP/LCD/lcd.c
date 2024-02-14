/**
 ****************************************************************************************************
 * @file        lcd.c
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

#include "./BSP/LCD/lcd.h"
#include "./BSP/SPI/spi.h"
#include "./BSP/LCD/font.h"
#include "./SYSTEM/delay/delay.h"


/* LCD�����С���ã��޸Ĵ�ֵʱ��ע�⣡�������޸�������ֵʱ���ܻ�Ӱ�����º��� lcd_clear/lcd_fill/lcd_draw_line */
#define LCD_TOTAL_BUF_SIZE      (LCD_WIDTH * LCD_HEIGHT * 2)
#define LCD_BUF_SIZE            11520
static uint8_t lcd_buf[LCD_BUF_SIZE];

uint16_t    g_point_color = BLACK;                                      /* ������ɫ Ĭ��Ϊ��ɫ */
uint16_t    g_back_color = WHITE;                                       /* ������ɫ Ĭ��Ϊ��ɫ */

/**
 * @brief       LCD���ƽӿڳ�ʼ��
 * @param       ��
 * @retval      ��
 */
static void lcd_gpio_init(void)
{
    GPIO_InitTypeDef gpio_init_struct = {0};
    
    LCD_PWR_GPIO_CLK_ENABLE();
    LCD_RST_GPIO_CLK_ENABLE();
    LCD_WR_GPIO_CLK_ENABLE();
    LCD_CS_GPIO_CLK_ENABLE();
    
    gpio_init_struct.Pin = LCD_PWR_GPIO_PIN;                            /* LCD_PWR���� */
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;                        /* ������� */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;                      /* ���� */
    gpio_init_struct.Pull = GPIO_PULLUP;                                /* ���� */
    HAL_GPIO_Init(LCD_PWR_GPIO_PORT, &gpio_init_struct);                /* ��ʼ��LCD_PWR���� */
    
    gpio_init_struct.Pin = LCD_RST_GPIO_PIN;                            /* LCD_RST���� */
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;                        /* ������� */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;                      /* ���� */
    gpio_init_struct.Pull = GPIO_PULLUP;                                /* ���� */
    HAL_GPIO_Init(LCD_RST_GPIO_PORT, &gpio_init_struct);                /* ��ʼ��LCD_RST���� */
    
    gpio_init_struct.Pin = LCD_WR_GPIO_PIN;                             /* LCD_WR���� */
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;                        /* ������� */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;                      /* ���� */
    gpio_init_struct.Pull = GPIO_PULLUP;                                /* ���� */
    HAL_GPIO_Init(LCD_WR_GPIO_PORT, &gpio_init_struct);                 /* ��ʼ��LCD_WR���� */
    
    gpio_init_struct.Pin = LCD_CS_GPIO_PIN;                             /* LCD_CS���� */
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;                        /* ������� */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;                      /* ���� */
    gpio_init_struct.Pull = GPIO_PULLUP;                                /* ���� */
    HAL_GPIO_Init(LCD_CS_GPIO_PORT, &gpio_init_struct);                 /* ��ʼ��LCD_CS���� */
    
    LCD_CS(0);                                                          /* ʹ��Ƭѡ */
    LCD_PWR(0);

    LCD_RST(0);
    delay_ms(120);
    LCD_RST(1);

    spi1_init();                                                        /* ��ʼ��SPI1�ӿ� */
}

/**
 * @brief       LCD�ײ�SPI�������ݺ���
 * @param       data    ���ݵ���ʼ��ַ
 * @param       size    �������ݴ�С
 * @retval      ��
 */
static void lcd_spi_send(uint8_t *data, uint32_t size)
{
    uint32_t i;
    uint32_t delta;
    LCD_CS(0);                          /* ʹ��Ƭѡ */
    delta = size / 0xFFFF;
    
    for (i = 0; i <= delta; i ++)
    {
        if( i == delta )                /* �������һ֡���� */
        {
            spi1_write_data(&data[i * 0xFFFF], size % 0xFFFF);
        }
        else                            /* ��������һ�η���0xFFFF�ֽ����� */
        {
            spi1_write_data(&data[i * 0xFFFF], 0xFFFF);
        }
    }
    LCD_CS(1);                          /* ȡ��Ƭѡ */
}

/**
 * @brief       д���LCD
 * @param       cmd ��Ҫ���͵�����
 * @retval      ��
 */
static void lcd_write_cmd(uint8_t cmd)
{
    LCD_WR(0);

    lcd_spi_send(&cmd, 1);
}

/**
 * @brief       д���ݵ�LCD
 * @param       cmd ��Ҫ���͵�����
 * @retval      ��
 */
static void lcd_write_data(uint8_t data)
{
    LCD_WR(1);

    lcd_spi_send(&data, 1);
}

/**
 * @brief       д����ֵ����ݵ�LCD
 * @param       cmd ��Ҫ���͵�����
 * @retval      ��
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
 * @brief       ��������д��LCD��������
 * @param       x1,y1 �������
 * @param       x2,y2 �յ�����
 * @retval      ��
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
 * @brief       ��LCD��ʾ
 * @param       ��
 * @retval      ��
 */
void lcd_display_on(void)
{
    LCD_PWR(1);
}

/**
 * @brief       �ر�LCD��ʾ
 * @param       ��
 * @retval      ��
 */
void lcd_display_off(void)
{
    LCD_PWR(0);
}

/**
 * @brief       ��һ����ɫ���LCD��
 * @param       color ������ɫ
 * @retval      ��
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
 * @brief       ��һ����ɫ���LCD��
 * @param       x_start,y_start �������
 * @param       x_end,y_end �յ�����
 * @param       color �����ɫ
 * @retval      ��
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
 * @brief       ���㺯��
 * @param       x,y ��������
 * @retval      ��
 */
void lcd_draw_point(uint16_t x, uint16_t y,uint16_t color)
{
    lcd_address_set(x, y, x, y);
    lcd_write_halfword(color);
}

/**
 * @brief       �������ɫ����
 * @param       x,y ��������
 * @retval      ��
 */
void lcd_draw_color_point(uint16_t x, uint16_t y,uint16_t color)
{
    lcd_address_set(x, y, x, y);
    lcd_write_halfword(color);
}

/**
 * @brief       ���ߺ���(ֱ�ߡ�б��)
 * @param       x1,y1   �������
 * @param       x2,y2   �յ�����
 * @retval      ��
 */
void lcd_draw_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,uint16_t color)
{
    uint16_t t;
    int xerr = 0, yerr = 0, delta_x, delta_y, distance;
    int incx, incy, row, col;
    uint32_t i = 0;

    if (y1 == y2)
    {
        /*���ٻ�ˮƽ��*/
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
 * @brief       ��һ������
 * @param       x1,y1   �������
 * @param       x2,y2   �յ�����
 * @retval      ��
 */
void lcd_draw_rectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,uint16_t color)
{
    lcd_draw_line(x1, y1, x2, y1,color);
    lcd_draw_line(x1, y1, x1, y2,color);
    lcd_draw_line(x1, y2, x2, y2,color);
    lcd_draw_line(x2, y1, x2, y2,color);
}

/**
 * @brief       ��һ��Բ
 * @param       x0,y0   Բ������
 * @param       r   Բ�뾶
 * @retval      ��
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
 * @brief       ��ʾһ���ַ�
 * @param       x,y     ��ʾ��ʼ����
 * @param       chr     ��Ҫ��ʾ���ַ�
 * @param       size    �����С(֧��16/24/32������)
 * @param       mode    ���ӷ�ʽ(1); �ǵ��ӷ�ʽ(0);
 * @retval      ��
 */
void lcd_show_char(uint16_t x, uint16_t y, char chr, uint8_t size, uint8_t mode ,uint16_t color)
{
    uint8_t temp = 0,t1 = 0, t = 0;
    uint8_t *pfont = 0;
    uint8_t csize = 0;                                              /* �õ�����һ���ַ���Ӧ������ռ���ֽ��� */
    uint16_t colortemp = 0;
    uint8_t sta = 0;

    chr = chr - ' ';                                                /* �õ�ƫ�ƺ��ֵ��ASCII�ֿ��Ǵӿո�ʼȡģ������-' '���Ƕ�Ӧ�ַ����ֿ⣩ */

    if ((x > (LCD_WIDTH - size / 2)) || (y > (LCD_HEIGHT - size)))
    {
        return;
    }

    lcd_address_set(x, y, x + size / 2 - 1, y + size - 1);          /* (x,y,x+8-1,y+16-1) */

    switch (size)
    {
        case 12:
            pfont = (uint8_t *)asc2_1206[chr];                      /* ����1206���� */
            break;

        case 16:
            pfont = (uint8_t *)asc2_1608[chr];                      /* ����1608���� */
            break;

        case 24:
            pfont = (uint8_t *)asc2_2412[chr];                      /* ����2412���� */
            break;

        case 32:
            pfont = (uint8_t *)asc2_3216[chr];                      /* ����3216���� */
            break;

        default:
            return ;
    }
    
    if (size != 24)
    {
        csize = (size / 8 + ((size % 8) ? 1 : 0)) * (size / 2);
        
        for (t = 0; t < csize; t++)
        {
            temp = pfont[t];                                        /* ��ȡ�ַ��ĵ������� */
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
                        else if (mode == 0)                         /* ��Ч��,����ʾ */
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
                else if (mode == 0)                                 /* ��Ч��,����ʾ */
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
 * @brief       m^n����
 * @param       m,n     �������
 * @retval      m^n�η�
 */
static uint32_t lcd_pow(uint8_t m, uint8_t n)
{
    uint32_t result = 1;

    while(n--)result *= m;

    return result;
}

/**
 * @brief       ��ʾlen������
 * @param       x,y : ��ʼ����
 * @param       num : ��ֵ(0 ~ 2^32)
 * @param       len : ��ʾ���ֵ�λ��
 * @param       size: ѡ������ 12/16/24/32
 * @retval      ��
 */
void lcd_show_num(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size, uint16_t color)
{
    uint8_t t, temp;
    uint8_t enshow = 0;

    for (t = 0; t < len; t++)   /* ������ʾλ��ѭ�� */
    {
        temp = (num / lcd_pow(10, len - t - 1)) % 10;   /* ��ȡ��Ӧλ������ */

        if (enshow == 0 && t < (len - 1))   /* û��ʹ����ʾ,�һ���λҪ��ʾ */
        {
            if (temp == 0)
            {
                lcd_show_char(x + (size / 2)*t, y, ' ', size, 0, color);/* ��ʾ�ո�,ռλ */
                continue;   /* �����¸�һλ */
            }
            else
            {
                enshow = 1; /* ʹ����ʾ */
            }

        }

        lcd_show_char(x + (size / 2)*t, y, temp + '0', size, 0, color); /* ��ʾ�ַ� */
    }
}

/**
 * @brief       ��չ��ʾlen������(��λ��0Ҳ��ʾ)
 * @param       x,y : ��ʼ����
 * @param       num : ��ֵ(0 ~ 2^32)
 * @param       len : ��ʾ���ֵ�λ��
 * @param       size: ѡ������ 12/16/24/32
 * @param       mode: ��ʾģʽ
 *              [7]:0,�����;1,���0.
 *              [6:1]:����
 *              [0]:0,�ǵ�����ʾ;1,������ʾ.
 *
 * @retval      ��
 */
void lcd_show_xnum(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size, uint8_t mode, uint16_t color)
{
    uint8_t t, temp;
    uint8_t enshow = 0;

    for (t = 0; t < len; t++)   /* ������ʾλ��ѭ�� */
    {
        temp = (num / lcd_pow(10, len - t - 1)) % 10;    /* ��ȡ��Ӧλ������ */

        if (enshow == 0 && t < (len - 1))   /* û��ʹ����ʾ,�һ���λҪ��ʾ */
        {
            if (temp == 0)
            {
                if (mode & 0X80)   /* ��λ��Ҫ���0 */
                {
                    lcd_show_char(x + (size / 2)*t, y, '0', size, mode & 0X01, color);  /* ��0ռλ */
                }
                else
                {
                    lcd_show_char(x + (size / 2)*t, y, ' ', size, mode & 0X01, color);  /* �ÿո�ռλ */
                }

                continue;
            }
            else
            {
                enshow = 1; /* ʹ����ʾ */
            }

        }

        lcd_show_char(x + (size / 2)*t, y, temp + '0', size, mode & 0X01, color);
    }
}

/**
 * @brief       ��ʾ�ַ���
 * @param       x,y         : ��ʼ����
 * @param       width,height: �����С
 * @param       size        : ѡ������ 12/16/24/32
 * @param       p           : �ַ����׵�ַ
 * @retval      ��
 */
void lcd_show_string(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t size, char *p, uint16_t color)
{
    uint8_t x0 = x;
    width += x;
    height += y;

    while ((*p <= '~') && (*p >= ' '))   /* �ж��ǲ��ǷǷ��ַ�! */
    {
        if (x >= width)
        {
            x = x0;
            y += size;
        }

        if (y >= height)break;  /* �˳� */

        lcd_show_char(x, y, *p, size, 0, color);
        x += size / 2;
        p++;
    }
}

/**
 * @brief       ��ʾͼƬ
 *
 * @remark      Image2Lcdȡģ��ʽ�� C��������/ˮƽɨ��/16λ���ɫ(RGB565)/��λ��ǰ  �����Ĳ�Ҫѡ
 * @param       x,y     �������
 * @param       width   ͼƬ���
 * @param       height  ͼƬ�߶�
 * @param       p       ͼƬ����������ʼ��ַ
 * @retval      ��
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
 * @brief       LCD��ʼ��
 * @remark      ��
 * @retval      ��
 */
void lcd_init(void)
{
    lcd_gpio_init();            /* Ӳ���ӿڳ�ʼ�� */

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

    LCD_PWR(1);                 /* ����ʾ */
}
