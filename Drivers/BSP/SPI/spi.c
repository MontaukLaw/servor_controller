/**
 ****************************************************************************************************
 * @file        spi.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2023-08-01
 * @brief       SPI ��������
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

#include "./BSP/SPI/spi.h"


SPI_HandleTypeDef g_spi1_handler;                                      /* SPI1��� */

/**
 * @brief       SPI��ʼ������
 * @note        ����ģʽ,8λ����,��ֹӲ��Ƭѡ
 * @param       ��
 * @retval      ��
 */
void spi1_init(void)
{
    SPI1_SPI_CLK_ENABLE();                                             /* SPI1ʱ��ʹ�� */

    g_spi1_handler.Instance = SPI1_SPI;                                /* SPI1 */
    g_spi1_handler.Init.Mode = SPI_MODE_MASTER;                        /* ����SPI����ģʽ������Ϊ��ģʽ */
    g_spi1_handler.Init.Direction = SPI_DIRECTION_2LINES;              /* ����SPI�������˫�������ģʽ:SPI����Ϊ˫��ģʽ */
    g_spi1_handler.Init.DataSize = SPI_DATASIZE_8BIT;                  /* ����SPI�����ݴ�С:SPI���ͽ���8λ֡�ṹ */
    g_spi1_handler.Init.CLKPolarity = SPI_POLARITY_HIGH;               /* ����ͬ��ʱ�ӵĿ���״̬Ϊ�ߵ�ƽ */
    g_spi1_handler.Init.CLKPhase = SPI_PHASE_2EDGE;                    /* ����ͬ��ʱ�ӵĵڶ��������أ��������½������ݱ����� */
    g_spi1_handler.Init.NSS = SPI_NSS_SOFT;                            /* NSS�ź���Ӳ����NSS�ܽţ����������ʹ��SSIλ������:�ڲ�NSS�ź���SSIλ���� */
    g_spi1_handler.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;   /* ���岨����Ԥ��Ƶ��ֵ:������Ԥ��ƵֵΪ4 */
    g_spi1_handler.Init.FirstBit = SPI_FIRSTBIT_MSB;                   /* ָ�����ݴ����MSBλ����LSBλ��ʼ:���ݴ����MSBλ��ʼ */
    g_spi1_handler.Init.TIMode = SPI_TIMODE_DISABLE;                   /* �ر�TIģʽ */
    g_spi1_handler.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;   /* �ر�Ӳ��CRCУ�� */
    g_spi1_handler.Init.CRCPolynomial = 10;                            /* CRCֵ����Ķ���ʽ */
    HAL_SPI_Init(&g_spi1_handler);                                     /* ��ʼ�� */

    __HAL_SPI_ENABLE(&g_spi1_handler);                                 /* ʹ��SPI1 */

    spi1_read_write_byte(0XFF);                                        /* ��������, ʵ���Ͼ��ǲ���8��ʱ������, �ﵽ���DR������, �Ǳ��� */
}

/**
 * @brief       SPI�ײ�������ʱ��ʹ�ܣ���������
 * @note        �˺����ᱻHAL_SPI_Init()����
 * @param       hspi:SPI���
 * @retval      ��
 */
void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
{
    GPIO_InitTypeDef gpio_init_struct;
    
    if (hspi->Instance == SPI1_SPI)
    {
        SPI1_SCK_GPIO_CLK_ENABLE();     /* SPI1_SCK��ʱ��ʹ�� */
        SPI1_MOSI_GPIO_CLK_ENABLE();    /* SPI1_MOSI��ʱ��ʹ�� */

        /* SCK����ģʽ����(�������) */
        gpio_init_struct.Pin = SPI1_SCK_GPIO_PIN;
        gpio_init_struct.Mode = GPIO_MODE_AF_PP;
        gpio_init_struct.Pull = GPIO_PULLUP;
        gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
        gpio_init_struct.Alternate = SPI1_SCK_GPIO_AF;
        HAL_GPIO_Init(SPI1_SCK_GPIO_PORT, &gpio_init_struct);

        /* MOSI����ģʽ����(�������) */
        gpio_init_struct.Pin = SPI1_MOSI_GPIO_PIN;
        gpio_init_struct.Alternate = SPI1_MOSI_GPIO_AF;
        HAL_GPIO_Init(SPI1_MOSI_GPIO_PORT, &gpio_init_struct);
    }
}

/**
 * @brief       SPI1�ٶ����ú���
 * @note        SPI1ʱ��ѡ������APB2, ��PCLK2, Ϊ170Mhz
 *              SPI�ٶ� = PCLK2 / 2^(speed + 1)
 * @param       speed   : SPI1ʱ�ӷ�Ƶϵ��
                        ȡֵΪSPI_BAUDRATEPRESCALER_2~SPI_BAUDRATEPRESCALER_2 256
 * @retval      ��
 */
void spi1_set_speed(uint8_t speed)
{
    assert_param(IS_SPI_BAUDRATE_PRESCALER(speed)); /* �ж���Ч�� */
    __HAL_SPI_DISABLE(&g_spi1_handler);             /* �ر�SPI */
    g_spi1_handler.Instance->CR1 &= 0XFFC7;         /* λ3-5���㣬�������ò����� */
    g_spi1_handler.Instance->CR1 |= speed << 3;     /* ����SPI�ٶ� */
    __HAL_SPI_ENABLE(&g_spi1_handler);              /* ʹ��SPI */
}

/**
 * @brief       SPI1��дһ���ֽ�����
 * @param       txdata  : Ҫ���͵�����(1�ֽ�)
 * @retval      ���յ�������(1�ֽ�)
 */
uint8_t spi1_read_write_byte(uint8_t txdata)
{
    uint8_t rxdata;
    HAL_SPI_TransmitReceive(&g_spi1_handler, &txdata, &rxdata, 1, 1000);
    return rxdata;  /* �����յ������� */
}

/**
 * @brief       SPI1д����
 * @param       txdata: �����͵�����
 * @retval      ��
 */
void spi1_write_data(uint8_t *tx_data,uint16_t size)
{
    HAL_SPI_Transmit(&g_spi1_handler, tx_data, size, HAL_MAX_DELAY);
}


