/**
 ****************************************************************************************************
 * @file        adc.h
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

#ifndef __ADC_H
#define __ADC_H

#include "./SYSTEM/sys/sys.h"


/******************************************************************************************/
/* ADC及引脚 定义 */

#define ADC3_CHY_GPIO_PORT                  GPIOA
#define ADC3_CHY_GPIO_PIN                   GPIO_PIN_1 
#define ADC3_CHY_GPIO_CLK_ENABLE()          do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0)   /* PA口时钟使能 */

#define ADC_ADCX                            ADC3 
#define ADC3_CHY                            ADC_CHANNEL_1                               /* 通道Y,  0 <= Y <= 17 */ 
#define ADC3_CHY_CLK_ENABLE()               do{ __HAL_RCC_ADC3_CLK_ENABLE(); }while(0)   /* ADC1 时钟使能 */


/* ADC单通道/多通道 DMA采集 DMA及通道 定义
 * 注意: ADC1的DMA通道只能是: DMA1_Channel1, 因此只要是ADC1, 这里是不能改动的
 *       ADC2不支持DMA采集
 *       ADC3的DMA通道只能是: DMA2_Channel5, 因此如果使用 ADC3 则需要修改
 */
#define ADC_ADCX_DMACx                      DMA1_Channel1
#define ADC_ADCX_DMACx_IRQn                 DMA1_Channel1_IRQn
#define ADC_ADCX_DMACx_IRQHandler           DMA1_Channel1_IRQHandler

#define ADC_ADCX_DMACx_IS_TC()              ( DMA1->ISR & (1 << 1) )    /* 判断 DMA1_Channel1 传输完成标志, 这是一个假函数形式,
                                                                         * 不能当函数使用, 只能用在if等语句里面 
                                                                         */
#define ADC_ADCX_DMACx_CLR_TC()             do{ DMA1->IFCR |= 1 << 1; }while(0) /* 清除 DMA1_Channel1 传输完成标志 */

/* ADC3温度传感器通道 定义 */ 

#define ADC_TEMPSENSOR_CHX                  ADC_CHANNEL_16

/******************************************************************************************/

void adc3_init(void);                               /* ADC3初始化 */
void adc3_channel_set(ADC_HandleTypeDef *adc_handle, uint32_t ch,uint32_t rank, uint32_t stime);   /* ADC3通道设置 */
uint32_t adc3_get_result(uint32_t ch);               /* 获得某个通道值  */
uint32_t adc3_get_result_average(uint32_t ch, uint8_t times);/* 得到某个通道给定次数采样的平均值 */

#endif 















