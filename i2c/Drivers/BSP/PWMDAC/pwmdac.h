/**
 ****************************************************************************************************
 * @file        pwmdac.h
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-24
 * @brief       PWM DAC��� ��������
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
 *
 ****************************************************************************************************
 */

#ifndef __PWMDAC_H
#define __PWMDAC_H

#include "./SYSTEM/sys/sys.h"


/******************************************************************************************/
/* PWM DAC ���� �� ��ʱ�� ���� */

/* PWMDAC Ĭ����ʹ�� PA8, ��Ӧ�Ķ�ʱ��Ϊ TIM1_CH1, �����Ҫ�޸ĳ�����IO���, ����Ӧ
 * �Ķ�ʱ����ͨ��ҲҪ�����޸�. �����ʵ����������޸�.
 */
#define PWMDAC_GPIO_PORT                    GPIOA
#define PWMDAC_GPIO_PIN                     GPIO_PIN_8
#define PWMDAC_GPIO_CLK_ENABLE()            do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)   /* PA��ʱ��ʹ�� */

#define PWMDAC_TIMX                         TIM1
#define PWMDAC_TIMX_CHY                     TIM_CHANNEL_1                           /* ͨ��Y,  1<= Y <=4 */
#define PWMDAC_TIMX_CCRX                    PWMDAC_TIMX->CCR1           /* ͨ��Y������ȽϼĴ��� */
#define PWMDAC_TIMX_CLK_ENABLE()            do{ __HAL_RCC_TIM1_CLK_ENABLE(); }while(0)  /* TIM1 ʱ��ʹ�� */

/******************************************************************************************/

void pwmdac_init(uint16_t arr, uint16_t psc);   /* PWM DAC��ʼ�� */
void pwmdac_set_voltage(uint16_t vol);          /* PWM DAC���������ѹ */

#endif

















