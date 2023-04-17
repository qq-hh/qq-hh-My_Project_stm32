/**
 ****************************************************************************************************
 * @file        dma.h
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

#ifndef __DMA_H
#define	__DMA_H

#include "./SYSTEM/sys/sys.h"

void dma_map_config(DMA_Channel_TypeDef* DMAx_CHx,uint32_t src,uint32_t dest);                 /* 内存映射外设初始化函数 */
void dma_usart1_tx_config(uint32_t mar);                        /* 串口1 TX DMA初始化 */
void dma_enable(DMA_Channel_TypeDef *dmax_chy, uint16_t cndtr); /* 使能一次DMA传输 */
void dma_basic_config(DMA_Channel_TypeDef *dmax_chy,  uint32_t par, uint32_t mar);  /* DMA基本配置 */
#endif






























