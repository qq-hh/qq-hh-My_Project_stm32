/**
 ****************************************************************************************************
 * @file        dma.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-23
 * @brief       DMA 驱动代码
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
 *
 ****************************************************************************************************
 */

#include "./BSP/DMA/dma.h"
#include "./SYSTEM/delay/delay.h"

DMA_HandleTypeDef  g_dma_handle;                  /* DMA句柄 */
extern UART_HandleTypeDef g_uart1_handle;                /* UART句柄 */


/**
 * @brief       内存映射外设初始化函数
 * @param       par         : 硬件外设寄存器地址
 * @param       mar         : 存储器地址
 * @retval      无
 */
void dma_map_config(DMA_Channel_TypeDef* DMAx_CHx,uint32_t src,uint32_t dest)
{
    if ((uint32_t)DMAx_CHx > (uint32_t)DMA1_Channel7)     /* 大于DMA1_Channel7, 则为DMA2的通道了 */
    {
        __HAL_RCC_DMA2_CLK_ENABLE();                      /* DMA2时钟使能 */
    }
    else 
    {
        __HAL_RCC_DMA1_CLK_ENABLE();                      /* DMA1时钟使能 */
    }
    
    //HAL_DMA_DeInit(&g_dma_handle);    /* HardFault ??? */

    /* Tx DMA配置 */
    g_dma_handle.Instance = DMAx_CHx;                        /* USART1_TX使用的DMA通道为: DMA1_Channel4 */
    g_dma_handle.Init.Direction = DMA_MEMORY_TO_PERIPH;           /* DIR = 1 , 存储器到外设模式 */
    g_dma_handle.Init.PeriphInc = DMA_PINC_DISABLE;               /* 外设非增量模式 */
    g_dma_handle.Init.MemInc = DMA_MINC_ENABLE;                   /* 存储器增量模式 */
    g_dma_handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;  /* 外设数据长度:8位 */
    g_dma_handle.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;     /* 存储器数据长度:8位 */
    g_dma_handle.Init.Mode = DMA_NORMAL;                          /* 外设流控模式 */
    g_dma_handle.Init.Priority = DMA_PRIORITY_MEDIUM;             /* 中等优先级 */

    HAL_DMA_Init(&g_dma_handle);
    HAL_DMA_Start(&g_dma_handle, (uint32_t)src, dest, 0);      /* 配置DMA传输参数 */
}


/**
 * @brief       开启一次DMA传输
 * @param       dmax_chy    : DMA及通道, DMA1_Channel1 ~ DMA1_Channel7, DMA2_Channel1 ~ DMA2_Channel5
 *                            某个外设对应哪个DMA, 哪个通道, 请参考<<STM32中文参考手册 V10>> 10.3.7节
 *                            必须设置正确的DMA及通道, 才能正常使用! 
 * @param       cndtr       : 数据传输量
 * @retval      无
 */
void dma_enable(DMA_Channel_TypeDef *dmax_chy, uint16_t cndtr)
{
    __HAL_DMA_DISABLE(&g_dma_handle);           /* 关闭DMA传输 */
    g_dma_handle.Instance->CNDTR = cndtr;       /* 重设DMA传输数据量 */
    __HAL_DMA_ENABLE(&g_dma_handle);            /* 开启DMA传输 */
}


/**
 * @brief       串口1 TX DMA初始化函数
 * @param       mar         : 存储器地址
 * @retval      无
 */
void dma_usart1_tx_config(uint32_t mar)
{
    dma_map_config(DMA1_Channel4, mar, (uint32_t)&USART1->DR);   /* USART1_TX使用的DMA通道为: DMA1_Channel4 */
}
























