/**
 ****************************************************************************************************
 * @file        adc.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.3
 * @date        2020-04-24
 * @brief       ADC 驱动代码
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 STM32F103开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 * 修改说明
 * V1.0 20200423
 * 第一次发布
 * V1.1 20200423
 * 1,支持ADC单通道DMA采集 
 * 2,新增adc_dma_init和adc_dma_enable函数.
 * V1.2 20200423
 * 1,支持ADC多通道DMA采集 
 * 2,新增adc_nch_dma_init函数.
 * V1.3 20200424
 * 1,支持内部温度传感器温度采集
 * 2,新增adc_temperature_init和adc_get_temperature函数.
 *
 ****************************************************************************************************
 */

#include "./BSP/ADC/adc.h"
#include "./BSP/DMA/dma.h"
#include "./SYSTEM/delay/delay.h"

/* 单通道ADC采集 */
ADC_HandleTypeDef g_adc_handle;   /* ADC句柄 */

/* 单通道ADC采集 DMA读取 */
DMA_HandleTypeDef g_dma_adc_handle;     /* 与ADC关联的DMA句柄 */
uint8_t g_adc_dma_sta = 0;              /* DMA传输状态标志, 0,未完成; 1, 已完成 */


/********************************************************************/
/**
 * @brief       ADC初始化函数
 *   @note      本函数支持ADC1/ADC2任意通道, 但是不支持ADC3
 *              我们使用12位精度, ADC采样时钟=12M, 转换时间为: 采样周期 + 12.5个ADC周期
 *              设置最大采样周期: 239.5, 则转换时间 = 252 个ADC周期 = 21us
 * @param       无
 * @retval      无
 */
void adc_init(void)
{
    RCC_PeriphCLKInitTypeDef adc_clkinit;

    adc_clkinit.PeriphClockSelection = RCC_PERIPHCLK_ADC;   /* ADC外设时钟 */
    adc_clkinit.AdcClockSelection = RCC_ADCPCLK2_DIV6;      /* 分频因子6时钟为72M/6=12MHz */
    HAL_RCCEx_PeriphCLKConfig(&adc_clkinit);                /* 设置ADC时钟 */

    g_adc_handle.Instance = ADC_ADCX;
    g_adc_handle.Init.DataAlign = ADC_DATAALIGN_RIGHT;       /* 右对齐 */
    g_adc_handle.Init.ScanConvMode = DISABLE;                /* 非扫描模式 */
    g_adc_handle.Init.ContinuousConvMode = DISABLE;          /* 关闭连续转换 */
    g_adc_handle.Init.NbrOfConversion = 1;                   /* 1个转换在规则序列中 也就是只转换规则序列1 */
    g_adc_handle.Init.DiscontinuousConvMode = DISABLE;       /* 禁止不连续采样模式 */
    g_adc_handle.Init.NbrOfDiscConversion = 0;               /* 不连续采样通道数为0 */
    g_adc_handle.Init.ExternalTrigConv = ADC_SOFTWARE_START; /* 软件触发 */
    HAL_ADC_Init(&g_adc_handle);                             /* 初始化 */

    HAL_ADCEx_Calibration_Start(&g_adc_handle);              /* 校准ADC */
}

/**
 * @brief       ADC底层驱动，引脚配置，时钟使能
                此函数会被HAL_ADC_Init()调用
 * @param       hadc:ADC句柄
 * @retval      无
 */
void HAL_ADC_MspInit(ADC_HandleTypeDef *hadc)
{
    if(hadc->Instance == ADC_ADCX)
    {
        GPIO_InitTypeDef GPIO_Initure;
        ADC_ADCX_CHY_CLK_ENABLE();      /* 使能ADCx时钟 */
        ADC_ADCX_CHY_GPIO_CLK_ENABLE(); /* 开启GPIO时钟 */

        /* AD采集引脚模式设置,模拟输入 */
        GPIO_Initure.Pin = ADC_ADCX_CHY_GPIO_PIN;        
        GPIO_Initure.Mode = GPIO_MODE_ANALOG; 
        GPIO_Initure.Pull = GPIO_PULLUP;      
        HAL_GPIO_Init(ADC_ADCX_CHY_GPIO_PORT, &GPIO_Initure);
    }
}

/**
 * @brief       设置ADC通道采样时间
 * @param       adcx : adc句柄指针,ADC_HandleTypeDef
 * @param       ch   : 通道号, ADC_CHANNEL_0~ADC_CHANNEL_17
 * @param       stime: 采样时间  0~7, 对应关系为:
 *   @arg       ADC_SAMPLETIME_1CYCLE_5, 1.5个ADC时钟周期        ADC_SAMPLETIME_7CYCLES_5, 7.5个ADC时钟周期
 *   @arg       ADC_SAMPLETIME_13CYCLES_5, 13.5个ADC时钟周期     ADC_SAMPLETIME_28CYCLES_5, 28.5个ADC时钟周期
 *   @arg       ADC_SAMPLETIME_41CYCLES_5, 41.5个ADC时钟周期       ADC_SAMPLETIME_55CYCLES_5, 55.5个ADC时钟周期
 *   @arg       ADC_SAMPLETIME_71CYCLES_5, 71.5个ADC时钟周期       ADC_SAMPLETIME_239CYCLES_5, 239.5个ADC时钟周期
 * @param       rank: 多通道采集时需要设置的采集编号,
                假设你定义channle1的rank=1，channle2 的rank=2，
                那么对应你在DMA缓存空间的变量数组AdcDMA[0] 就i是channle1的转换结果，AdcDMA[1]就是通道2的转换结果。 
                单通道DMA设置为 ADC_REGULAR_RANK_1
 *   @arg       编号1~16：ADC_REGULAR_RANK_1~ADC_REGULAR_RANK_16
 * @retval      无
 */
void adc_channel_set(ADC_HandleTypeDef *adc_handle, uint32_t ch,uint32_t rank, uint32_t stime)
{
    /* 配置对应ADC通道 */
    ADC_ChannelConfTypeDef adc_channel;
    adc_channel.Channel = ch;
    adc_channel.Rank = rank;
    adc_channel.SamplingTime = stime;
    HAL_ADC_ConfigChannel( adc_handle, &adc_channel);    /* 设置ADCX对通道0(对应PA0脚)的采样时间为239.5个时钟周期 */
}

/**
 * @brief       获得ADC转换后的结果
 * @param       ch: 通道值 0~17，取值范围为：ADC_CHANNEL_0~ADC_CHANNEL_17
 * @retval      无
 */
uint32_t adc_get_result(uint32_t ch)
{
    adc_channel_set(&g_adc_handle , ch, ADC_REGULAR_RANK_1, ADC_SAMPLETIME_239CYCLES_5);    /* 设置通道，序列和采样时间 */
    HAL_ADC_Start(&g_adc_handle);                            /* 开启ADC */

    HAL_ADC_PollForConversion(&g_adc_handle, 10);            /* 轮询转换 */

    return (uint16_t)HAL_ADC_GetValue(&g_adc_handle);        /* 返回最近一次ADC1规则组的转换结果 */
    
}

/**
 * @brief       获取通道ch的转换值，取times次,然后平均
 * @param       ch      : 通道号, 0~17
 * @param       times   : 获取次数
 * @retval      通道ch的times次转换结果平均值
 */
uint32_t adc_get_result_average(uint32_t ch, uint8_t times)
{
    uint32_t temp_val = 0;
    uint8_t t;

    for (t = 0; t < times; t++)     /* 获取times次数据 */
    {
        temp_val += adc_get_result(ch);
        delay_ms(5);
    }

    return temp_val / times;        /* 返回平均值 */
}


/***************************************单通道ADC采集(DMA读取)程序*****************************************/

/**
 * @brief       ADC DMA读取 初始化函数
 *   @note      本函数还是使用adc_init对ADC进行大部分配置,有差异的地方再单独配置
 * @param       par         : 外设地址
 * @param       mar         : 存储器地址
 * @retval      无
 */
void adc_dma_init(uint32_t mar)
{
    if ((uint32_t)ADC_ADCX_DMACx > (uint32_t)DMA1_Channel7)     /* 大于DMA1_Channel7, 则为DMA2的通道了 */
    {
        __HAL_RCC_DMA2_CLK_ENABLE();                            /* DMA2时钟使能 */
    }
    else 
    {
        __HAL_RCC_DMA1_CLK_ENABLE();                            /* DMA1时钟使能 */
    }

    /* DMA配置 */
    g_dma_adc_handle.Instance = ADC_ADCX_DMACx;                             /* 设置DMA通道 */
    g_dma_adc_handle.Init.Direction = DMA_PERIPH_TO_MEMORY;                 /* DIR = 1 ,  外设到存储器模式 */
    g_dma_adc_handle.Init.PeriphInc = DMA_PINC_DISABLE;                     /* 外设非增量模式 */
    g_dma_adc_handle.Init.MemInc = DMA_MINC_ENABLE;                         /* 存储器增量模式 */
    g_dma_adc_handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;    /* 外设数据长度:16位 */
    g_dma_adc_handle.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;       /* 存储器数据长度:16位 */
    g_dma_adc_handle.Init.Mode = DMA_NORMAL;                                /* 外设流控模式 */
    g_dma_adc_handle.Init.Priority = DMA_PRIORITY_MEDIUM;                   /* 中等优先级 */
    HAL_DMA_Init(&g_dma_adc_handle);
    HAL_DMA_Start(&g_dma_adc_handle, (uint32_t)&ADC1->DR, mar, 0);          /* 配置DMA传输参数 */

    /* 配置对应ADC 端口 */
    g_adc_handle.DMA_Handle = &g_dma_adc_handle;
    adc_init();
    /* 
        需要在配置的时候开，但这里为了保证不变更之前的代码，
        另加一行设置g_adc_handle.Init.ContinuousConvMode = ENABLE;
        配置ADC连续转换, DMA传输ADC数据
     */
    SET_BIT(g_adc_handle.Instance->CR2, ADC_CR2_CONT);      /* CONT = 1, 连续转换模式 */

    /* 配置对应ADC通道 */
    adc_channel_set(&g_adc_handle , ADC_ADCX_CHY, ADC_REGULAR_RANK_1, ADC_SAMPLETIME_239CYCLES_5);    /* 设置通道，序列和采样时间 */

    HAL_NVIC_SetPriority(ADC_ADCX_DMACx_IRQn, 3, 3);
    HAL_NVIC_EnableIRQ(ADC_ADCX_DMACx_IRQn);
    HAL_ADC_Start_DMA(&g_adc_handle,&mar,sizeof(uint16_t));
    __HAL_DMA_ENABLE_IT(&g_dma_adc_handle, DMA_IT_TC);  /* TCIE =1 , 使能传输完成中断 */

}

/**
 * @brief       使能一次ADC DMA传输 
 * @param       ndtr: DMA传输的次数
 * @retval      无
 */
void adc_dma_enable(uint16_t cndtr)
{
    __HAL_ADC_DISABLE(&g_adc_handle);               /* 先关闭ADC */
    
    __HAL_DMA_DISABLE(&g_dma_adc_handle);           /* 关闭DMA传输 */
    g_dma_adc_handle.Instance->CNDTR = cndtr;       /* 重设DMA传输数据量 */
    __HAL_DMA_ENABLE(&g_dma_adc_handle);            /* 开启DMA传输 */
    
    __HAL_ADC_ENABLE(&g_adc_handle);                /* 重新启动ADC */
    ADC_ADCX->CR2 |= 1 << 22;                       /* 启动规则转换通道 */
}

/**
 * @brief       ADC DMA采集中断服务函数
 * @param       无 
 * @retval      无
 */
void ADC_ADCX_DMACx_IRQHandler(void)
{
    if (ADC_ADCX_DMACx_IS_TC())
    {
        g_adc_dma_sta = 1;          /* 标记DMA传输完成 */
        ADC_ADCX_DMACx_CLR_TC();    /* 清除DMA1 数据流7 传输完成中断 */
    }
}

/***************************************多通道ADC采集(DMA读取)程序*****************************************/

/**
 * @brief       ADC N通道(6通道) DMA读取 初始化函数
 *   @note      本函数还是使用adc_init对ADC进行大部分配置,有差异的地方再单独配置
 *              另外,由于本函数用到了6个通道, 宏定义会比较多内容, 因此,本函数就不采用宏定义的方式来修改通道了,
 *              直接在本函数里面修改, 这里我们默认使用PA0~PA5这6个通道.
 *
 *              注意: 本函数还是使用 ADC_ADCX(默认=ADC1) 和 ADC_ADCX_DMACx( DMA1_Channel1 ) 及其相关定义
 *              不要乱修改adc.h里面的这两部分内容, 必须在理解原理的基础上进行修改, 否则可能导致无法正常使用.
 *
 * @param       mar         : 存储器地址 
 * @retval      无
 */
void adc_nch_dma_init(uint32_t mar)
{
    RCC_PeriphCLKInitTypeDef adc_clkinit;


    __HAL_RCC_DMA1_CLK_ENABLE();                            /* DMA1时钟使能 */

    /* DMA配置 */
    g_dma_adc_handle.Instance = ADC_ADCX_DMACx;                             /* 设置DMA通道 */
    g_dma_adc_handle.Init.Direction = DMA_PERIPH_TO_MEMORY;                 /* DIR = 1 ,  外设到存储器模式 */
    g_dma_adc_handle.Init.PeriphInc = DMA_PINC_DISABLE;                     /* 外设非增量模式 */
    g_dma_adc_handle.Init.MemInc = DMA_MINC_ENABLE;                         /* 存储器增量模式 */
    g_dma_adc_handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;    /* 外设数据长度:16位 */
    g_dma_adc_handle.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;       /* 存储器数据长度:16位 */
    g_dma_adc_handle.Init.Mode = DMA_NORMAL;                                /* 外设流控模式 */
    g_dma_adc_handle.Init.Priority = DMA_PRIORITY_MEDIUM;                   /* 中等优先级 */
    HAL_DMA_Init(&g_dma_adc_handle);
    HAL_DMA_Start(&g_dma_adc_handle, (uint32_t)&ADC1->DR, mar, 0);          /* 配置DMA传输参数 */

    ADC_ADCX_CHY_GPIO_CLK_ENABLE(); /* IO口时钟使能 */
    ADC_ADCX_CHY_CLK_ENABLE();      /* ADC时钟使能 */

    adc_clkinit.PeriphClockSelection = RCC_PERIPHCLK_ADC;   /* ADC外设时钟 */
    adc_clkinit.AdcClockSelection = RCC_ADCPCLK2_DIV6;      /* 分频因子6时钟为72M/6=12MHz */
    HAL_RCCEx_PeriphCLKConfig(&adc_clkinit);                /* 设置ADC时钟 */

    g_adc_handle.DMA_Handle = &g_dma_adc_handle;
    g_adc_handle.Instance = ADC_ADCX;
    g_adc_handle.Init.DataAlign = ADC_DATAALIGN_RIGHT;       /* 右对齐 */
    g_adc_handle.Init.ScanConvMode = ENABLE;                 /* 扫描模式 */
    g_adc_handle.Init.ContinuousConvMode = ENABLE;           /* 连续转换 */
    g_adc_handle.Init.NbrOfConversion = 6;                   /* 6个转换在规则序列中 也就是只转换规则序列1 */
    g_adc_handle.Init.DiscontinuousConvMode = DISABLE;       /* 禁止不连续采样模式 */
    g_adc_handle.Init.NbrOfDiscConversion = 1;               /* 连续采样通道数为0 */
    g_adc_handle.Init.ExternalTrigConv = ADC_SOFTWARE_START; /* 软件触发 */
    HAL_ADC_Init(&g_adc_handle);                             /* 初始化 */

    HAL_ADCEx_Calibration_Start(&g_adc_handle);              /* 校准ADC */

    /* 
        设置ADC1通道0~5对应的IO口模拟输入
        AD采集引脚模式设置,模拟输入
        PA0对应 ADC1_IN0
        PA1对应 ADC1_IN1
        PA2对应 ADC1_IN2 
        PA3对应 ADC1_IN3 
        PA4对应 ADC1_IN4 
        PA5对应 ADC1_IN5
    */
    GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_GPIOA_CLK_ENABLE(); /* 开启GPIO时钟 */
    GPIO_Initure.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5;        
    GPIO_Initure.Mode = GPIO_MODE_ANALOG; 
    GPIO_Initure.Pull = GPIO_PULLUP;      
    HAL_GPIO_Init(GPIOA, &GPIO_Initure);

    /* 配置对应ADC通道 */
    adc_channel_set(&g_adc_handle , ADC_CHANNEL_0, ADC_REGULAR_RANK_1, ADC_SAMPLETIME_239CYCLES_5);    /* 设置ADCX对通道0(对应PA0脚)的采样时间为239.5个时钟周期 */
    adc_channel_set(&g_adc_handle , ADC_CHANNEL_1, ADC_REGULAR_RANK_2, ADC_SAMPLETIME_239CYCLES_5);    /* 设置ADCX对通道0(对应PA1脚)的采样时间为239.5个时钟周期 */
    adc_channel_set(&g_adc_handle , ADC_CHANNEL_2, ADC_REGULAR_RANK_3, ADC_SAMPLETIME_239CYCLES_5);    /* 设置ADCX对通道0(对应PA2脚)的采样时间为239.5个时钟周期 */
    adc_channel_set(&g_adc_handle , ADC_CHANNEL_3, ADC_REGULAR_RANK_4, ADC_SAMPLETIME_239CYCLES_5);    /* 设置ADCX对通道0(对应PA3脚)的采样时间为239.5个时钟周期 */
    adc_channel_set(&g_adc_handle , ADC_CHANNEL_4, ADC_REGULAR_RANK_5, ADC_SAMPLETIME_239CYCLES_5);    /* 设置ADCX对通道0(对应PA4脚)的采样时间为239.5个时钟周期 */
    adc_channel_set(&g_adc_handle , ADC_CHANNEL_5, ADC_REGULAR_RANK_6, ADC_SAMPLETIME_239CYCLES_5);    /* 设置ADCX对通道0(对应PA5脚)的采样时间为239.5个时钟周期 */

    HAL_NVIC_SetPriority(ADC_ADCX_DMACx_IRQn, 3, 3);
    HAL_NVIC_EnableIRQ(ADC_ADCX_DMACx_IRQn);
    HAL_ADC_Start_DMA(&g_adc_handle,&mar,sizeof(uint16_t));
    __HAL_DMA_ENABLE_IT(&g_dma_adc_handle, DMA_IT_TC);  /* TCIE =1 , 使能传输完成中断 */

}


/***************************************内部温度传感器实验*****************************************/
/**
 * @brief       ADC 内部温度传感器 初始化函数
 *   @note      本函数还是使用adc_init对ADC进行大部分配置,有差异的地方再单独配置
 *              注意: STM32F103内部温度传感器只连接在ADC1的通道16上, 其他ADC无法进行转换.
 *
 * @param       无
 * @retval      无
 */
void adc_temperature_init(void)
{
    adc_init(); /* 先初始化ADC */
    SET_BIT(g_adc_handle.Instance->CR2, ADC_CR2_TSVREFE);  /* TSVREFE = 1, 启用内部温度传感器和Vrefint */
}

/**
 * @brief       获取内部温度传感器温度值
 * @param       无
 * @retval      温度值(扩大了100倍,单位:℃.)
 */
short adc_get_temperature(void)
{
    uint32_t adcx;
    short result;
    double temperature;

    adcx = adc_get_result_average(ADC_TEMPSENSOR_CHX, 20);  /* 读取内部温度传感器通道,10次取平均 */
    temperature = (float)adcx * (3.3 / 4096);               /* 转化为电压值 */
    temperature = (1.43 - temperature) / 0.0043 + 25;       /* 计算温度 */
    result = temperature *= 100;    /* 扩大100倍. */
    return result;

}





