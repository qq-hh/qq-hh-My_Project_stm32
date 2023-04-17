/**
 ****************************************************************************************************
 * @file        dac.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.2
 * @date        2020-04-24
 * @brief       DAC ��������
 * @license     Copyright (c) 2020-2032, ������������ӿƼ����޹�˾
 ****************************************************************************************************
 * @attention
 *
 * ʵ��ƽ̨:����ԭ�� STM32F103������
 * ������Ƶ:www.yuanzige.com
 * ������̳:www.openedv.com
 * ��˾��ַ:www.alientek.com
 * �����ַ:openedv.taobao.com
 *
 * �޸�˵��
 * V1.0 20200424
 * ��һ�η���
 * V1.1 20200424
 * ����dac_triangular_wave����
 * V1.2 20200424
 * 1, ֧��DMA DAC������Ⲩ��
 * 2, ����dac_dma_wave_init��dac_dma_wave_enable����
 *
 ****************************************************************************************************
 */

#include "./BSP/DAC/dac.h"
#include "./SYSTEM/delay/delay.h"

DAC_HandleTypeDef dac1_handler;             /* DAC��� */

/* ���Ҳ����ͼ */
DAC_HandleTypeDef dac1_dma_handler;         /* DAC��� */
DMA_HandleTypeDef g_dma_dac_handle;         /* ��DAC������DMA��� */

static TIM_HandleTypeDef g_timx_trig_handle;            /* ��ʱ��x��� */

/**
 * @brief       DAC��ʼ������
 *   @note      ������֧��DAC1_OUT1/2ͨ����ʼ��
 *              DAC������ʱ������APB1, ʱ��Ƶ��=36Mhz=27.8ns
 *              DAC�����buffer�رյ�ʱ��, �������ʱ��: tSETTLING = 4us (F103�����ֲ���д)
 *              ���DAC���������ٶ�ԼΪ:250Khz, ��10����Ϊһ������, ��������25Khz���ҵĲ���
 *
 * @param       outx: Ҫ��ʼ����ͨ��. 1,ͨ��1; 2,ͨ��2
 * @retval      ��
 */
void dac_init(uint8_t outx)
{
    __HAL_RCC_DAC_CLK_ENABLE();          /* ʹ��DAC1��ʱ�� */
    GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_GPIOA_CLK_ENABLE();   /* ʹ��DAC OUT1/2��IO��ʱ��(����PA��,PA4/PA5) */
    GPIO_Initure.Pin = (outx==1)? GPIO_PIN_4 : GPIO_PIN_5;      /* STM32��Ƭ��, ����PA4=DAC1_OUT1, PA5=DAC1_OUT2 */
    GPIO_Initure.Mode = GPIO_MODE_ANALOG; 
    GPIO_Initure.Pull = GPIO_PULLUP;      
    HAL_GPIO_Init(GPIOA, &GPIO_Initure);

    dac1_handler.Instance = DAC;
    HAL_DAC_Init(&dac1_handler);                                 /* ��ʼ��DAC */

    DAC_ChannelConfTypeDef DACCHx_Config;
    DACCHx_Config.DAC_Trigger = DAC_TRIGGER_NONE;                 /* ��ʹ�ô������� */
    DACCHx_Config.DAC_OutputBuffer = DAC_OUTPUTBUFFER_DISABLE;    /* DAC1�������ر� */
    
    switch(outx)
    {
        case 1:
            HAL_DAC_ConfigChannel(&dac1_handler,&DACCHx_Config,DAC_CHANNEL_1);      /* DACͨ��1���� */
            HAL_DAC_Start(&dac1_handler,DAC_CHANNEL_1);                             /* ����DACͨ��1 */
            break;
        case 2:
            HAL_DAC_ConfigChannel(&dac1_handler,&DACCHx_Config,DAC_CHANNEL_2);  /* DACͨ��1���� */
            HAL_DAC_Start(&dac1_handler,DAC_CHANNEL_2);                         /* ����DACͨ��1 */
            break;
        default:break;
    }

}

///**
// * @brief       DAC�ײ�������ʱ�����ã����� ����
//                �˺����ᱻHAL_DAC_Init()����
// * @param       hdac:DAC���
// * @retval      ��
// */
//void HAL_DAC_MspInit(DAC_HandleTypeDef* hdac)
//{      
//    GPIO_InitTypeDef GPIO_Initure;
//    __HAL_RCC_DAC_CLK_ENABLE();             //ʹ��DACʱ��
//    __HAL_RCC_GPIOA_CLK_ENABLE();            //����GPIOAʱ��
//    
//    GPIO_Initure.Pin=GPIO_PIN_4;            //PA4
//    GPIO_Initure.Mode=GPIO_MODE_ANALOG;     //ģ��
//    GPIO_Initure.Pull=GPIO_NOPULL;          //����������
//    HAL_GPIO_Init(GPIOA,&GPIO_Initure);
//}


/**
 * @brief       ����ͨ��1/2�����ѹ
 * @param       outx: 1,ͨ��1; 2,ͨ��2
 * @param       vol : 0~3300,����0~3.3V
 * @retval      ��
 */
void dac_set_voltage(uint8_t outx, uint16_t vol)
{
    double temp = vol;
    temp /= 1000;
    temp = temp * 4096 / 3.3;

    if (temp >= 4096)temp = 4095;   /* ���ֵ���ڵ���4096, ��ȡ4095 */

    if (outx == 1)   /* ͨ��1 */
    {
        HAL_DAC_SetValue(&dac1_handler,DAC_CHANNEL_1,DAC_ALIGN_12B_R,temp);/* 12λ�Ҷ������ݸ�ʽ����DACֵ */
    }
    else            /* ͨ��2 */
    {
        HAL_DAC_SetValue(&dac1_handler,DAC_CHANNEL_2,DAC_ALIGN_12B_R,temp);/* 12λ�Ҷ������ݸ�ʽ����DACֵ */
    }
}

/**
 * @brief       ����DAC_OUT1������ǲ�
 *   @note      ���Ƶ�� �� 1000 / (dt * samples) Khz, ������dt��С��ʱ��,����С��5usʱ, ����delay_us
 *              ����Ͳ�׼��(���ú���,����ȶ���Ҫʱ��,��ʱ��С��ʱ��,��Щʱ���Ӱ�쵽��ʱ), Ƶ�ʻ�ƫС.
 * 
 * @param       maxval : ���ֵ(0 < maxval < 4096), (maxval + 1)������ڵ���samples/2
 * @param       dt     : ÿ�����������ʱʱ��(��λ: us)
 * @param       samples: ������ĸ���, samples����С�ڵ���(maxval + 1) * 2 , ��maxval���ܵ���0
 * @param       n      : ������θ���,0~65535
 *
 * @retval      ��
 */
void dac_triangular_wave(uint16_t maxval, uint16_t dt, uint16_t samples, uint16_t n)
{
    uint16_t i, j;
    float incval;           /* ������ */
    float Curval;           /* ��ǰֵ */
    
    if((maxval + 1) <= samples)return ; /* ���ݲ��Ϸ� */
        
    incval = (maxval + 1) / (samples / 2);  /* ��������� */
    
    for(j = 0; j < n; j++)
    { 
        Curval = 0;
        HAL_DAC_SetValue(&dac1_handler,DAC_CHANNEL_1,DAC_ALIGN_12B_R,Curval);    /* �����0 */
        for(i = 0; i < (samples / 2); i++)  /* ��������� */
        {
            Curval  +=  incval;         /* �µ����ֵ */
            HAL_DAC_SetValue(&dac1_handler,DAC_CHANNEL_1,DAC_ALIGN_12B_R,Curval);
            delay_us(dt);
        } 
        for(i = 0; i < (samples / 2); i++)  /* ����½��� */
        {
            Curval  -=  incval;         /* �µ����ֵ */
            HAL_DAC_SetValue(&dac1_handler,DAC_CHANNEL_1,DAC_ALIGN_12B_R,Curval);
            delay_us(dt);
        }
    }
}


/**
 * @brief       DAC DMA������γ�ʼ������
 *   @note      ������֧��DAC1_OUT1/2ͨ����ʼ��
 *              DAC������ʱ������APB1, ʱ��Ƶ��=36Mhz=27.7ns
 *              DAC�����buffer�رյ�ʱ��, �������ʱ��: tSETTLING = 4us (F103�����ֲ���д)
 *              ���DAC���������ٶ�ԼΪ:250Khz, ��10����Ϊһ������, ��������25Khz���ҵĲ���
 *
 * @param       outx: Ҫ��ʼ����ͨ��. 1,ͨ��1; 2,ͨ��2
 * @param       par         : �����ַ
 * @param       mar         : �洢����ַ
 * @retval      ��
 */
void dac_dma_wave_init(uint8_t outx, uint32_t par, uint32_t mar)
{
    DMA_Channel_TypeDef *dmax_chy;

    if (outx == 1)
    {
        dmax_chy = DMA2_Channel3;       /* OUT1��ӦDMA2_Channel3 */
    }
    else
    {
        dmax_chy = DMA2_Channel4;       /* OUT2��ӦDMA2_Channel4 */
    }

    /* ʹ��DMAʱ��, ������CPAR��CMAR�Ĵ��� */
    __HAL_RCC_DMA2_CLK_ENABLE();                            /* DMA1ʱ��ʹ�� */

    /* DMA���� */
    g_dma_dac_handle.Instance = dmax_chy;                                   /* ����DMAͨ�� */
    g_dma_dac_handle.Init.Direction = DMA_MEMORY_TO_PERIPH;                 /* DIR = 1 , �洢��������ģʽ */
    g_dma_dac_handle.Init.PeriphInc = DMA_PINC_DISABLE;                     /* ���������ģʽ */
    g_dma_dac_handle.Init.MemInc = DMA_MINC_ENABLE;                         /* �洢������ģʽ */
    g_dma_dac_handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;    /* �������ݳ���:16λ */
    g_dma_dac_handle.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;       /* �洢�����ݳ���:16λ */
    g_dma_dac_handle.Init.Mode = DMA_CIRCULAR;                              /* ��������ģʽ */
    g_dma_dac_handle.Init.Priority = DMA_PRIORITY_MEDIUM;                   /* �е����ȼ� */
    HAL_DMA_Init(&g_dma_dac_handle);
    HAL_DMA_Start(&g_dma_dac_handle, mar, par, 0);          /* ����DMA������� */


    __HAL_RCC_DAC_CLK_ENABLE();          /* ʹ��DAC1��ʱ�� */
    GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_GPIOA_CLK_ENABLE();   /* ʹ��DAC OUT1/2��IO��ʱ��(����PA��,PA4/PA5) */
    GPIO_Initure.Pin = (outx==1)? GPIO_PIN_4 : GPIO_PIN_5;      /* STM32��Ƭ��, ����PA4=DAC1_OUT1, PA5=DAC1_OUT2 */
    GPIO_Initure.Mode = GPIO_MODE_ANALOG; 
    GPIO_Initure.Pull = GPIO_PULLUP;      
    HAL_GPIO_Init(GPIOA, &GPIO_Initure);

    dac1_dma_handler.Instance = DAC;
    dac1_dma_handler.DMA_Handle1 = &g_dma_dac_handle;
    HAL_DAC_Init(&dac1_dma_handler);                                 /* ��ʼ��DAC */

    DAC_ChannelConfTypeDef DACCHx_Config;
    DACCHx_Config.DAC_Trigger = DAC_TRIGGER_T7_TRGO;                 /* ��ʹ�ô������� */
    DACCHx_Config.DAC_OutputBuffer = DAC_OUTPUTBUFFER_DISABLE;    /* DAC1�������ر� */

    switch(outx)
    {
        case 1:
            HAL_DAC_ConfigChannel(&dac1_dma_handler,&DACCHx_Config,DAC_CHANNEL_1);      /* DACͨ��1���� */
            HAL_DAC_Start(&dac1_dma_handler,DAC_CHANNEL_1);                             /* ����DACͨ��1 */
            SET_BIT(dac1_dma_handler.Instance->CR, DAC_CR_DMAEN1);
            //HAL_DAC_Start_DMA(&dac1_dma_handler, DAC_CHANNEL_1, &mar, 16,DAC_ALIGN_12B_R);
            break;
        case 2:
            HAL_DAC_ConfigChannel(&dac1_dma_handler,&DACCHx_Config,DAC_CHANNEL_2);  /* DACͨ��1���� */
            HAL_DAC_Start(&dac1_dma_handler,DAC_CHANNEL_2);                         /* ����DACͨ��1 */
            SET_BIT(dac1_dma_handler.Instance->CR, DAC_CR_DMAEN2);
            //HAL_DAC_Start_DMA(&dac1_dma_handler, DAC_CHANNEL_2, &mar, 16,DAC_ALIGN_12B_R);
            break;
        default:break;
    }

    __HAL_RCC_TIM7_CLK_ENABLE();             /* TIM7ʱ��ʹ��,�����õ���������ͼ */ 
    g_timx_trig_handle.Instance = TIM7;                            /* ͨ�ö�ʱ��x */
}

/**
 * @brief       DAC DMAʹ�ܲ������
 *   @note      TIM7������ʱ��Ƶ��(f)����APB1, f = 36M * 2 = 72Mhz.
 *              DAC����Ƶ�� ftrgo = f / ((psc + 1) * (arr + 1))
 *              ����Ƶ�� = ftrgo / ndtr; 
 *
 * @param       outx        : Ҫ��ʼ����ͨ��. 1,ͨ��1; 2,ͨ��2
 * @param       ndtr        : DMAͨ�����δ���������
 * @param       arr         : TIM7���Զ���װ��ֵ
 * @param       psc         : TIM7�ķ�Ƶϵ��
 * @retval      ��
 */
void dac_dma_wave_enable(uint8_t outx, uint16_t cndtr, uint16_t arr, uint16_t psc)
{
    __HAL_TIM_ENABLE(&g_timx_trig_handle);     /* �رն�ʱ��TIMX,ֹͣ���� */
    __HAL_DMA_DISABLE(&g_dma_dac_handle);           /* �ر�DMA���� */
    g_dma_dac_handle.Instance->CNDTR = cndtr;       /* ����DMA���������� */
    __HAL_DMA_ENABLE(&g_dma_dac_handle);            /* ����DMA���� */

    TIM_HallSensor_InitTypeDef timSensorcfg;
    TIM_MasterConfigTypeDef mod;
    mod.MasterOutputTrigger = TIM_TRGO_UPDATE;
    mod.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;

    g_timx_trig_handle.Instance = TIM7;                            /* ͨ�ö�ʱ��x */
    g_timx_trig_handle.Init.Prescaler = psc;                         /* ��Ƶ */
    g_timx_trig_handle.Init.CounterMode = TIM_COUNTERMODE_UP;        /* ���ϼ����� */
    g_timx_trig_handle.Init.Period = arr;                            /* �Զ�װ��ֵ */
    g_timx_trig_handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;  /* ʱ�ӷ�Ƶ���� */
    HAL_TIM_Base_Init(&g_timx_trig_handle);
    HAL_TIMEx_MasterConfigSynchronization(&g_timx_trig_handle,&mod); /*  ���ö�ʱ��7�ĸ����¼�����DACת�� */ 
    __HAL_TIM_ENABLE(&g_timx_trig_handle);
}






































