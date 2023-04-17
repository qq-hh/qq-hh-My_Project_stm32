/**
 ****************************************************************************************************
 * @file        lsens.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-24
 * @brief       ���������� ��������
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
 ****************************************************************************************************
 */

#include "./BSP/ADC/adc3.h"
#include "./BSP/LSENS/lsens.h"
#include "./SYSTEM/delay/delay.h"


/**
 * @brief       ��ʼ������������
 * @param       ��
 * @retval      ��
 */
void lsens_init(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    LSENS_ADC3_CHX_GPIO_CLK_ENABLE();   /* IO��ʱ��ʹ�� */

    /* AD�ɼ�����ģʽ����,ģ������ */
    GPIO_Initure.Pin = LSENS_ADC3_CHX_GPIO_PIN;        
    GPIO_Initure.Mode = GPIO_MODE_ANALOG; 
    GPIO_Initure.Pull = GPIO_PULLUP;      
    HAL_GPIO_Init(LSENS_ADC3_CHX_GPIO_PORT, &GPIO_Initure);

    adc3_init();    /* ��ʼ��ADC */
}

/**
 * @brief       ��ȡ����������ֵ
 * @param       ��
 * @retval      0~100:0,�;100,����
 */
uint8_t lsens_get_val(void)
{
    uint32_t temp_val = 0;
    temp_val = adc3_get_result_average(LSENS_ADC3_CHX, 10); /* ��ȡƽ��ֵ */
    temp_val /= 40;

    if (temp_val > 100)temp_val = 100;

    return (uint8_t)(100 - temp_val);
}












