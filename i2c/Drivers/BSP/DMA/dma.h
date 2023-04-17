/**
 ****************************************************************************************************
 * @file        dma.h
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-23
 * @brief       DMA ��������
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
 * V1.0 20200423
 * ��һ�η���
 *
 ****************************************************************************************************
 */

#ifndef __DMA_H
#define	__DMA_H

#include "./SYSTEM/sys/sys.h"

void dma_map_config(DMA_Channel_TypeDef* DMAx_CHx,uint32_t src,uint32_t dest);                 /* �ڴ�ӳ�������ʼ������ */
void dma_usart1_tx_config(uint32_t mar);                        /* ����1 TX DMA��ʼ�� */
void dma_enable(DMA_Channel_TypeDef *dmax_chy, uint16_t cndtr); /* ʹ��һ��DMA���� */
void dma_basic_config(DMA_Channel_TypeDef *dmax_chy,  uint32_t par, uint32_t mar);  /* DMA�������� */
#endif






























