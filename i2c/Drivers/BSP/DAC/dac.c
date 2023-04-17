/**
 ****************************************************************************************************
 * @file        dac.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.2
 * @date        2020-04-24
 * @brief       DAC 驱动代码
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
 * V1.0 20200424
 * 第一次发布
 * V1.1 20200424
 * 新增dac_triangular_wave函数
 * V1.2 20200424
 * 1, 支持DMA DAC输出任意波形
 * 2, 新增dac_dma_wave_init和dac_dma_wave_enable函数
 *
 ****************************************************************************************************
 */

#include "./BSP/DAC/dac.h"
#include "./SYSTEM/delay/delay.h"

DAC_HandleTypeDef dac1_handler;             /* DAC句柄 */

/* 正弦波相关图 */
DAC_HandleTypeDef dac1_dma_handler;         /* DAC句柄 */
DMA_HandleTypeDef g_dma_dac_handle;         /* 与DAC关联的DMA句柄 */

static TIM_HandleTypeDef g_timx_trig_handle;            /* 定时器x句柄 */

/**
 * @brief       DAC初始化函数
 *   @note      本函数支持DAC1_OUT1/2通道初始化
 *              DAC的输入时钟来自APB1, 时钟频率=36Mhz=27.8ns
 *              DAC在输出buffer关闭的时候, 输出建立时间: tSETTLING = 4us (F103数据手册有写)
 *              因此DAC输出的最高速度约为:250Khz, 以10个点为一个周期, 最大能输出25Khz左右的波形
 *
 * @param       outx: 要初始化的通道. 1,通道1; 2,通道2
 * @retval      无
 */
void dac_init(uint8_t outx)
{
    __HAL_RCC_DAC_CLK_ENABLE();          /* 使能DAC1的时钟 */
    GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_GPIOA_CLK_ENABLE();   /* 使能DAC OUT1/2的IO口时钟(都在PA口,PA4/PA5) */
    GPIO_Initure.Pin = (outx==1)? GPIO_PIN_4 : GPIO_PIN_5;      /* STM32单片机, 总是PA4=DAC1_OUT1, PA5=DAC1_OUT2 */
    GPIO_Initure.Mode = GPIO_MODE_ANALOG; 
    GPIO_Initure.Pull = GPIO_PULLUP;      
    HAL_GPIO_Init(GPIOA, &GPIO_Initure);

    dac1_handler.Instance = DAC;
    HAL_DAC_Init(&dac1_handler);                                 /* 初始化DAC */

    DAC_ChannelConfTypeDef DACCHx_Config;
    DACCHx_Config.DAC_Trigger = DAC_TRIGGER_NONE;                 /* 不使用触发功能 */
    DACCHx_Config.DAC_OutputBuffer = DAC_OUTPUTBUFFER_DISABLE;    /* DAC1输出缓冲关闭 */
    
    switch(outx)
    {
        case 1:
            HAL_DAC_ConfigChannel(&dac1_handler,&DACCHx_Config,DAC_CHANNEL_1);      /* DAC通道1配置 */
            HAL_DAC_Start(&dac1_handler,DAC_CHANNEL_1);                             /* 开启DAC通道1 */
            break;
        case 2:
            HAL_DAC_ConfigChannel(&dac1_handler,&DACCHx_Config,DAC_CHANNEL_2);  /* DAC通道1配置 */
            HAL_DAC_Start(&dac1_handler,DAC_CHANNEL_2);                         /* 开启DAC通道1 */
            break;
        default:break;
    }

}

///**
// * @brief       DAC底层驱动，时钟配置，引脚 配置
//                此函数会被HAL_DAC_Init()调用
// * @param       hdac:DAC句柄
// * @retval      无
// */
//void HAL_DAC_MspInit(DAC_HandleTypeDef* hdac)
//{      
//    GPIO_InitTypeDef GPIO_Initure;
//    __HAL_RCC_DAC_CLK_ENABLE();             //使能DAC时钟
//    __HAL_RCC_GPIOA_CLK_ENABLE();            //开启GPIOA时钟
//    
//    GPIO_Initure.Pin=GPIO_PIN_4;            //PA4
//    GPIO_Initure.Mode=GPIO_MODE_ANALOG;     //模拟
//    GPIO_Initure.Pull=GPIO_NOPULL;          //不带上下拉
//    HAL_GPIO_Init(GPIOA,&GPIO_Initure);
//}


/**
 * @brief       设置通道1/2输出电压
 * @param       outx: 1,通道1; 2,通道2
 * @param       vol : 0~3300,代表0~3.3V
 * @retval      无
 */
void dac_set_voltage(uint8_t outx, uint16_t vol)
{
    double temp = vol;
    temp /= 1000;
    temp = temp * 4096 / 3.3;

    if (temp >= 4096)temp = 4095;   /* 如果值大于等于4096, 则取4095 */

    if (outx == 1)   /* 通道1 */
    {
        HAL_DAC_SetValue(&dac1_handler,DAC_CHANNEL_1,DAC_ALIGN_12B_R,temp);/* 12位右对齐数据格式设置DAC值 */
    }
    else            /* 通道2 */
    {
        HAL_DAC_SetValue(&dac1_handler,DAC_CHANNEL_2,DAC_ALIGN_12B_R,temp);/* 12位右对齐数据格式设置DAC值 */
    }
}

/**
 * @brief       设置DAC_OUT1输出三角波
 *   @note      输出频率 ≈ 1000 / (dt * samples) Khz, 不过在dt较小的时候,比如小于5us时, 由于delay_us
 *              本身就不准了(调用函数,计算等都需要时间,延时很小的时候,这些时间会影响到延时), 频率会偏小.
 * 
 * @param       maxval : 最大值(0 < maxval < 4096), (maxval + 1)必须大于等于samples/2
 * @param       dt     : 每个采样点的延时时间(单位: us)
 * @param       samples: 采样点的个数, samples必须小于等于(maxval + 1) * 2 , 且maxval不能等于0
 * @param       n      : 输出波形个数,0~65535
 *
 * @retval      无
 */
void dac_triangular_wave(uint16_t maxval, uint16_t dt, uint16_t samples, uint16_t n)
{
    uint16_t i, j;
    float incval;           /* 递增量 */
    float Curval;           /* 当前值 */
    
    if((maxval + 1) <= samples)return ; /* 数据不合法 */
        
    incval = (maxval + 1) / (samples / 2);  /* 计算递增量 */
    
    for(j = 0; j < n; j++)
    { 
        Curval = 0;
        HAL_DAC_SetValue(&dac1_handler,DAC_CHANNEL_1,DAC_ALIGN_12B_R,Curval);    /* 先输出0 */
        for(i = 0; i < (samples / 2); i++)  /* 输出上升沿 */
        {
            Curval  +=  incval;         /* 新的输出值 */
            HAL_DAC_SetValue(&dac1_handler,DAC_CHANNEL_1,DAC_ALIGN_12B_R,Curval);
            delay_us(dt);
        } 
        for(i = 0; i < (samples / 2); i++)  /* 输出下降沿 */
        {
            Curval  -=  incval;         /* 新的输出值 */
            HAL_DAC_SetValue(&dac1_handler,DAC_CHANNEL_1,DAC_ALIGN_12B_R,Curval);
            delay_us(dt);
        }
    }
}


/**
 * @brief       DAC DMA输出波形初始化函数
 *   @note      本函数支持DAC1_OUT1/2通道初始化
 *              DAC的输入时钟来自APB1, 时钟频率=36Mhz=27.7ns
 *              DAC在输出buffer关闭的时候, 输出建立时间: tSETTLING = 4us (F103数据手册有写)
 *              因此DAC输出的最高速度约为:250Khz, 以10个点为一个周期, 最大能输出25Khz左右的波形
 *
 * @param       outx: 要初始化的通道. 1,通道1; 2,通道2
 * @param       par         : 外设地址
 * @param       mar         : 存储器地址
 * @retval      无
 */
void dac_dma_wave_init(uint8_t outx, uint32_t par, uint32_t mar)
{
    DMA_Channel_TypeDef *dmax_chy;

    if (outx == 1)
    {
        dmax_chy = DMA2_Channel3;       /* OUT1对应DMA2_Channel3 */
    }
    else
    {
        dmax_chy = DMA2_Channel4;       /* OUT2对应DMA2_Channel4 */
    }

    /* 使能DMA时钟, 并设置CPAR和CMAR寄存器 */
    __HAL_RCC_DMA2_CLK_ENABLE();                            /* DMA1时钟使能 */

    /* DMA配置 */
    g_dma_dac_handle.Instance = dmax_chy;                                   /* 设置DMA通道 */
    g_dma_dac_handle.Init.Direction = DMA_MEMORY_TO_PERIPH;                 /* DIR = 1 , 存储器到外设模式 */
    g_dma_dac_handle.Init.PeriphInc = DMA_PINC_DISABLE;                     /* 外设非增量模式 */
    g_dma_dac_handle.Init.MemInc = DMA_MINC_ENABLE;                         /* 存储器增量模式 */
    g_dma_dac_handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;    /* 外设数据长度:16位 */
    g_dma_dac_handle.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;       /* 存储器数据长度:16位 */
    g_dma_dac_handle.Init.Mode = DMA_CIRCULAR;                              /* 外设流控模式 */
    g_dma_dac_handle.Init.Priority = DMA_PRIORITY_MEDIUM;                   /* 中等优先级 */
    HAL_DMA_Init(&g_dma_dac_handle);
    HAL_DMA_Start(&g_dma_dac_handle, mar, par, 0);          /* 配置DMA传输参数 */


    __HAL_RCC_DAC_CLK_ENABLE();          /* 使能DAC1的时钟 */
    GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_GPIOA_CLK_ENABLE();   /* 使能DAC OUT1/2的IO口时钟(都在PA口,PA4/PA5) */
    GPIO_Initure.Pin = (outx==1)? GPIO_PIN_4 : GPIO_PIN_5;      /* STM32单片机, 总是PA4=DAC1_OUT1, PA5=DAC1_OUT2 */
    GPIO_Initure.Mode = GPIO_MODE_ANALOG; 
    GPIO_Initure.Pull = GPIO_PULLUP;      
    HAL_GPIO_Init(GPIOA, &GPIO_Initure);

    dac1_dma_handler.Instance = DAC;
    dac1_dma_handler.DMA_Handle1 = &g_dma_dac_handle;
    HAL_DAC_Init(&dac1_dma_handler);                                 /* 初始化DAC */

    DAC_ChannelConfTypeDef DACCHx_Config;
    DACCHx_Config.DAC_Trigger = DAC_TRIGGER_T7_TRGO;                 /* 不使用触发功能 */
    DACCHx_Config.DAC_OutputBuffer = DAC_OUTPUTBUFFER_DISABLE;    /* DAC1输出缓冲关闭 */

    switch(outx)
    {
        case 1:
            HAL_DAC_ConfigChannel(&dac1_dma_handler,&DACCHx_Config,DAC_CHANNEL_1);      /* DAC通道1配置 */
            HAL_DAC_Start(&dac1_dma_handler,DAC_CHANNEL_1);                             /* 开启DAC通道1 */
            SET_BIT(dac1_dma_handler.Instance->CR, DAC_CR_DMAEN1);
            //HAL_DAC_Start_DMA(&dac1_dma_handler, DAC_CHANNEL_1, &mar, 16,DAC_ALIGN_12B_R);
            break;
        case 2:
            HAL_DAC_ConfigChannel(&dac1_dma_handler,&DACCHx_Config,DAC_CHANNEL_2);  /* DAC通道1配置 */
            HAL_DAC_Start(&dac1_dma_handler,DAC_CHANNEL_2);                         /* 开启DAC通道1 */
            SET_BIT(dac1_dma_handler.Instance->CR, DAC_CR_DMAEN2);
            //HAL_DAC_Start_DMA(&dac1_dma_handler, DAC_CHANNEL_2, &mar, 16,DAC_ALIGN_12B_R);
            break;
        default:break;
    }

    __HAL_RCC_TIM7_CLK_ENABLE();             /* TIM7时钟使能,用续用到它辅助绘图 */ 
    g_timx_trig_handle.Instance = TIM7;                            /* 通用定时器x */
}

/**
 * @brief       DAC DMA使能波形输出
 *   @note      TIM7的输入时钟频率(f)来自APB1, f = 36M * 2 = 72Mhz.
 *              DAC触发频率 ftrgo = f / ((psc + 1) * (arr + 1))
 *              波形频率 = ftrgo / ndtr; 
 *
 * @param       outx        : 要初始化的通道. 1,通道1; 2,通道2
 * @param       ndtr        : DMA通道单次传输数据量
 * @param       arr         : TIM7的自动重装载值
 * @param       psc         : TIM7的分频系数
 * @retval      无
 */
void dac_dma_wave_enable(uint8_t outx, uint16_t cndtr, uint16_t arr, uint16_t psc)
{
    __HAL_TIM_ENABLE(&g_timx_trig_handle);     /* 关闭定时器TIMX,停止触发 */
    __HAL_DMA_DISABLE(&g_dma_dac_handle);           /* 关闭DMA传输 */
    g_dma_dac_handle.Instance->CNDTR = cndtr;       /* 重设DMA传输数据量 */
    __HAL_DMA_ENABLE(&g_dma_dac_handle);            /* 开启DMA传输 */

    TIM_HallSensor_InitTypeDef timSensorcfg;
    TIM_MasterConfigTypeDef mod;
    mod.MasterOutputTrigger = TIM_TRGO_UPDATE;
    mod.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;

    g_timx_trig_handle.Instance = TIM7;                            /* 通用定时器x */
    g_timx_trig_handle.Init.Prescaler = psc;                         /* 分频 */
    g_timx_trig_handle.Init.CounterMode = TIM_COUNTERMODE_UP;        /* 向上计数器 */
    g_timx_trig_handle.Init.Period = arr;                            /* 自动装载值 */
    g_timx_trig_handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;  /* 时钟分频因子 */
    HAL_TIM_Base_Init(&g_timx_trig_handle);
    HAL_TIMEx_MasterConfigSynchronization(&g_timx_trig_handle,&mod); /*  配置定时器7的更新事件触发DAC转换 */ 
    __HAL_TIM_ENABLE(&g_timx_trig_handle);
}






































