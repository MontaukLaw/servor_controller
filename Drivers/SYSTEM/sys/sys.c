/**
 ****************************************************************************************************
 * @file        sys.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2023-08-01
 * @brief       ϵͳ��ʼ������(����ʱ������/�жϹ���/GPIO���õ�)
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
 ****************************************************************************************************
 */

#include "./SYSTEM/sys/sys.h"


/**
 * @brief       �����ж�������ƫ�Ƶ�ַ
 * @param       baseaddr: ��ַ
 * @param       offset: ƫ����
 * @retval      ��
 */
void sys_nvic_set_vector_table(uint32_t baseaddr, uint32_t offset)
{
    /* ����NVIC��������ƫ�ƼĴ���,VTOR��9λ����,��[8:0]���� */
    SCB->VTOR = baseaddr | (offset & (uint32_t)0xFFFFFE00);
}

/**
 * @brief       ִ��: WFIָ��(ִ�����ָ�����͹���״̬, �ȴ��жϻ���)
 * @param       ��
 * @retval      ��
 */
void sys_wfi_set(void)
{
    __ASM volatile("wfi");
}

/**
 * @brief       �ر������ж�(���ǲ�����fault��NMI�ж�)
 * @param       ��
 * @retval      ��
 */
void sys_intx_disable(void)
{
    __ASM volatile("cpsid i");
}

/**
 * @brief       ���������ж�
 * @param       ��
 * @retval      ��
 */
void sys_intx_enable(void)
{
    __ASM volatile("cpsie i");
}

/**
 * @brief       ����ջ����ַ
 * @note        ��������ֺ�X, ����MDK��, ʵ����û�����
 * @param       addr: ջ����ַ
 * @retval      ��
 */
void sys_msr_msp(uint32_t addr)
{
    __set_MSP(addr);    /* ����ջ����ַ */
}

/**
 * @brief       �������ģʽ
 * @param       ��
 * @retval      ��
 */
void sys_standby(void)
{
    __HAL_RCC_PWR_CLK_ENABLE();                 /* ʹ�ܵ�Դʱ�� */
    SET_BIT(PWR->CR1, PWR_CR1_LPMS_STANDBY);    /* �������ģʽ */
}

/**
 * @brief       ϵͳ��λ
 * @param       ��
 * @retval      ��
 */
void sys_soft_reset(void)
{
    NVIC_SystemReset();
}

/**
 * @brief       ʱ�����ú���
 * @param       plln: ��PLL��Ƶϵ��(PLL��Ƶ), ȡֵ��Χ: 8~127.
 * @param       pllm: ��PLL����ƵPLLԤ��Ƶϵ��(��PLL֮ǰ�ķ�Ƶ), ȡֵ��Χ: 1~16.
 * @param       pllr: ��PLL��r��Ƶϵ��(PLL֮��ķ�Ƶ), ��Ƶ����Ϊϵͳʱ��, ȡֵ��Χ: 2, 4, 6, 8.(������4��ֵ)
 * @param       pllp: ��PLL��p��Ƶϵ��(PLL֮��ķ�Ƶ), ȡֵ��Χ: 2~31.
 * @param       pllq: ��PLL��q��Ƶϵ��(PLL֮��ķ�Ƶ), ȡֵ��Χ: 2, 4, 6, 8.(������4��ֵ)
 * @note
 *
 *              Fvco: VCOƵ��
 *              Fsys: ϵͳʱ��Ƶ��, Ҳ����PLL��p��Ƶ���ʱ��Ƶ��
 *              Fq:   ��PLL��q��Ƶ���ʱ��Ƶ��
 *              Fs:   ��PLL����ʱ��Ƶ��, ������HSI, HSE��.
 *              Fvco = Fs * (plln / pllm);
 *              Fsys = Fvco / pllr = Fs * (plln / (pllm * pllr));
 *              Fq   = Fvco / pllq = Fs * (plln / (pllm * pllq));
 *
 *              �ⲿ����Ϊ 8M��ʱ��, �Ƽ�ֵ: plln = 85, pllm = 2, pllr = 2, pllp = 4, pllq = 8.
 *              �õ�:Fvco = 8 * (85 / 2) = 340Mhz
 *                   Fsys = pll_p_ck = 340 / 2 = 170Mhz
 *                   Fq   = pll_q_ck = 340 / 8 = 42.5Mhz
 *
 *              G474Ĭ����Ҫ���õ�Ƶ������:
 *              CPUƵ��(HCLK) = pll_p_ck = 170Mhz
 *              AHB1/2/3(rcc_hclk1/2/3) = 170Mhz
 *              APB1(rcc_pclk1) = pll_p_ck / 1 = 170Mhz
 *              APB1(rcc_pclk2) = pll_p_ck / 1 = 170Mhz
 *
 * @retval      �������: 0, �ɹ�; 1, ����;
 */
uint8_t sys_stm32_clock_init(uint32_t plln, uint32_t pllm, uint32_t pllr,uint32_t pllp, uint32_t pllq)
{
    HAL_StatusTypeDef ret = HAL_OK;
    RCC_OscInitTypeDef rcc_osc_init = {0};
    RCC_ClkInitTypeDef rcc_clk_init = {0};

    __HAL_RCC_PWR_CLK_ENABLE();                                         /* ʹ��PWRʱ�� */

    HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1_BOOST);/* ���õ�ѹ�������ѹ�����Ա�������δ�����Ƶ�ʹ��� */
    
    /* ʹ��HSE����ѡ��HSE��ΪPLLʱ��Դ */
    rcc_osc_init.OscillatorType = RCC_OSCILLATORTYPE_HSE;        /* ʱ��ԴΪHSE */
    rcc_osc_init.HSEState = RCC_HSE_ON;                          /* ��HSE */
    rcc_osc_init.PLL.PLLState = RCC_PLL_ON;                      /* ��PLL */
    rcc_osc_init.PLL.PLLSource = RCC_PLLSOURCE_HSE;              /* PLLʱ��Դѡ��HSE */
    rcc_osc_init.PLL.PLLN = plln;
    rcc_osc_init.PLL.PLLM = pllm;
    rcc_osc_init.PLL.PLLP = pllp;
    rcc_osc_init.PLL.PLLQ = pllq;
    rcc_osc_init.PLL.PLLR = pllr;
    ret = HAL_RCC_OscConfig(&rcc_osc_init);                      /* ��ʼ��RCC */
    if(ret != HAL_OK)
    {
        return 1;                                                /* ʱ�ӳ�ʼ��ʧ�ܣ���������������Լ��Ĵ��� */
    }

    /* ѡ��PLL��Ϊϵͳʱ��Դ��������HCLK,PCLK1��PCLK2 */
    rcc_clk_init.ClockType = ( RCC_CLOCKTYPE_SYSCLK \
                                    | RCC_CLOCKTYPE_HCLK \
                                    | RCC_CLOCKTYPE_PCLK1 \
                                    | RCC_CLOCKTYPE_PCLK2);

    rcc_clk_init.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;         /* ����ϵͳʱ��ʱ��ԴΪPLL */
    rcc_clk_init.AHBCLKDivider = RCC_SYSCLK_DIV1;                /* AHB��Ƶϵ��Ϊ1 */
    rcc_clk_init.APB1CLKDivider = RCC_HCLK_DIV1;                 /* APB1��Ƶϵ��Ϊ1 */
    rcc_clk_init.APB2CLKDivider = RCC_HCLK_DIV1;                 /* APB2��Ƶϵ��Ϊ1 */
    ret = HAL_RCC_ClockConfig(&rcc_clk_init, FLASH_LATENCY_4);   /* ͬʱ����FLASH��ʱ����Ϊ4WS��Ҳ����5��CPU���� */
    if(ret != HAL_OK)
    {
        return 1;                                                /* ʱ�ӳ�ʼ��ʧ�� */
    }
    
    /* STM32G4x Z�汾������֧��Ԥȡ���� */
    if (HAL_GetREVID() == 0x1001)
    {
        __HAL_FLASH_PREFETCH_BUFFER_ENABLE();                    /* ʹ��flashԤȡ */
    }
    return 0;
}


#ifdef  USE_FULL_ASSERT

/**
 * @brief       ��������ʾ�����ʱ��˺����������������ļ���������
 * @param       file��ָ��Դ�ļ�
 *              line��ָ�����ļ��е�����
 * @retval      ��
 */
void assert_failed(uint8_t* file, uint32_t line)
{ 
    while (1)
    {
    }
}
#endif






