/**
 ****************************************************************************************************
 * @file        main.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2023-08-01
 * @brief       舵机控制 实验
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
 ****************************************************************************************************
 */

#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/LED/led.h"
#include "./BSP/KEY/key.h"
#include "./BSP/LCD/lcd.h"
#include "./BSP/TIMER/atim.h"
#include "./BSP/STEERING_ENGINE/steering_engine.h"

#include <stdio.h>
#include <stdint.h>

uint8_t rxBuffer[100];
uint8_t txBuffer[100];
uint8_t rxBytes[100];
uint8_t gotNewByte = 0;
uint8_t rxCounter = 0;

uint8_t x = 0, y = 0;

uint8_t parse_cmd(void)
{

    uint8_t len;

    int num;
    if (g_usart_rx_sta & 0x8000)
    {
        len = g_usart_rx_sta & 0x3fff;

        if (sscanf(g_usart_rx_buf, "X=%d", &num) == 1)
        {
            // 将解析出的数字转换为 uint8_t
            x = (uint8_t)num;

            // 打印结果
            printf("Extracted number: %d\n", x);
            memset(g_usart_rx_buf, 0, sizeof(g_usart_rx_buf));
            return 1;
        }
        g_usart_rx_sta = 0;
    }

    return 0;
}

int main(void)
{
    uint8_t key, t, id = 1;
    char buf[32];
    float angle = 90; /* 初始化角度0° */

    HAL_Init();                                 /* 初始化HAL库 */
    sys_stm32_clock_init(85, 2, 2, 4, 8);       /* 设置时钟,170Mhz */
    delay_init(170);                            /* 延时初始化 */
    usart_init(115200);                         /* 初始化串口 */
    key_init();                                 /* 初始化按键 */
    lcd_init();                                 /* 初始化LCD */
    atim_timx_pwm_chy_init(20000 - 1, 170 - 1); /* 1Mhz的计数频率，计数20000次为20ms */

    /* 显示提示信息 */
    printf("KEY0 选择控制的舵机接口\r\n");
    printf("KEY1 舵机旋转角度+45°\r\n");
    printf("KEY2 舵机旋转角度-45°\r\n");

    // 设置初始角度
    servo_angle_set(1, 90);
    uint8_t getCmd = 0;
    while (1)
    {
#if 0
        key = key_scan(0);
        if (key == KEY0_PRES)
        {
            id++;
            if (id == 3)
            {
                id = 1;
            }
            sprintf(buf, "Servo ID: %1d", id); /* 按下KEY0：选择控制哪个舵机，并显示当前ID */
            printf("所选舵机接口为%d\r\n", id);
        }
        else if (key == KEY1_PRES)
        {
            // angle[id - 1] += 45;
            angle[id - 1] += 5;
            if (angle[id - 1] > 180)
            {
                angle[id - 1] = 180;
            }
            servo_angle_set(id, angle[id - 1]); /* 控制该ID的舵机，并设置角度值 */
            printf("Servo 1: %.1f\r\n", angle[0]);
            printf("Servo 2: %.1f\r\n", angle[1]);
            printf("\r\n");
        }
        else if (key == KEY2_PRES)
        {
            // angle[id - 1] -= 45;
            angle[id - 1] -= 5;
            if (angle[id - 1] < 0)
            {
                angle[id - 1] = 0;
            }
            servo_angle_set(id, angle[id - 1]); /* 控制该ID的舵机，并设置角度值 */
            printf("Servo 1: %.1f\r\n", angle[0]);
            printf("Servo 2: %.1f\r\n", angle[1]);
            printf("\r\n");
        }

        t++;
        if (t % 20 == 0)
        {
            LED0_TOGGLE(); /* LED0(红灯) 翻转 */
        }
#endif
        delay_ms(10);

        getCmd = parse_cmd();

        if (getCmd)
        {
            servo_angle_set(1, (float)x);
            getCmd = 0;
        }
    }
}

int main__(void)
{
    uint8_t key, t, id = 1;
    char buf[32];
    float angle[2] = {90}; /* 初始化角度0° */

    HAL_Init();                                 /* 初始化HAL库 */
    sys_stm32_clock_init(85, 2, 2, 4, 8);       /* 设置时钟,170Mhz */
    delay_init(170);                            /* 延时初始化 */
    usart_init(115200);                         /* 初始化串口 */
    led_init();                                 /* 初始化LED */
    key_init();                                 /* 初始化按键 */
    lcd_init();                                 /* 初始化LCD */
    atim_timx_pwm_chy_init(20000 - 1, 170 - 1); /* 1Mhz的计数频率，计数20000次为20ms */

    /* 显示提示信息 */
    lcd_show_string(10, 10, 200, 16, 16, "Servo Test", RED);
    lcd_show_string(10, 30, 200, 16, 16, "KEY0:ID + +", RED);
    lcd_show_string(10, 50, 200, 16, 16, "KEY1:Angle +   ", RED);
    lcd_show_string(10, 70, 200, 16, 16, "KEY2:Angle -  ", RED);
    lcd_show_string(10, 90, 200, 16, 16, "Servo ID: 1", BLUE);
    printf("KEY0 选择控制的舵机接口\r\n");
    printf("KEY1 舵机旋转角度+45°\r\n");
    printf("KEY2 舵机旋转角度-45°\r\n");

    // 设置初始角度
    servo_angle_set(id, angle[id - 1]);

    while (1)
    {
        if (t % 10 == 1)
        {
            sprintf(buf, "Servo 1: %.1f", angle[0]);
            lcd_show_string(10, 110, 200, 16, 16, buf, BLUE);
            sprintf(buf, "Servo 2: %.1f", angle[1]);
            lcd_show_string(10, 130, 200, 16, 16, buf, BLUE);
        }

        key = key_scan(0);
        if (key == KEY0_PRES)
        {
            id++;
            if (id == 3)
            {
                id = 1;
            }
            sprintf(buf, "Servo ID: %1d", id); /* 按下KEY0：选择控制哪个舵机，并显示当前ID */
            lcd_show_string(10, 90, 200, 16, 16, buf, BLUE);
            printf("所选舵机接口为%d\r\n", id);
        }
        else if (key == KEY1_PRES)
        {
            // angle[id - 1] += 45;
            angle[id - 1] += 5;
            if (angle[id - 1] > 180)
            {
                angle[id - 1] = 180;
            }
            servo_angle_set(id, angle[id - 1]); /* 控制该ID的舵机，并设置角度值 */
            printf("Servo 1: %.1f\r\n", angle[0]);
            printf("Servo 2: %.1f\r\n", angle[1]);
            printf("\r\n");
        }
        else if (key == KEY2_PRES)
        {
            // angle[id - 1] -= 45;
            angle[id - 1] -= 5;
            if (angle[id - 1] < 0)
            {
                angle[id - 1] = 0;
            }
            servo_angle_set(id, angle[id - 1]); /* 控制该ID的舵机，并设置角度值 */
            printf("Servo 1: %.1f\r\n", angle[0]);
            printf("Servo 2: %.1f\r\n", angle[1]);
            printf("\r\n");
        }

        t++;
        if (t % 20 == 0)
        {
            LED0_TOGGLE(); /* LED0(红灯) 翻转 */
        }
        delay_ms(10);
    }
}
