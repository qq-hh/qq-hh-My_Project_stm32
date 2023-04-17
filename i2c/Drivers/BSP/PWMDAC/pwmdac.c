/**
 ****************************************************************************************************
 * @file        pwmdac.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-24
 * @brief       PWM DAC输出 驱动代码
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
 *
 ****************************************************************************************************
 */

#include "./BSP/PWMDAC/pwmdac.h"

TIM_HandleTypeDef g_tim1_handler;     //定时器句柄
TIM_OC_InitTypeDef g_tim1_ch1handler; //定时器1通道1句柄

/**
 * @brief       PWM DAC初始化, 实际上就是初始化定时器
 * @note
 *              定时器的时钟来自APB1 / APB2, 当APB1 / APB2 分频时, 定时器频率自动翻倍
 *              所以, 一般情况下, 我们所有定时器的频率, 都是72Mhz 等于系统时钟频率
 *              定时器溢出时间计算方法: Tout = ((arr + 1) * (psc + 1)) / Ft us.
 *              Ft = 定时器工作频率, 单位: Mhz
 *
 * @param       arr: 自动重装值。
 * @param       psc: 时钟预分频数
 * @retval      无
 */
void pwmdac_init(uint16_t arr, uint16_t psc)
{
    PWMDAC_TIMX_CLK_ENABLE();                             /* PWM DAC 定时器时钟使能 */

    g_tim1_handler.Instance = TIM1;                       /* 定时器1 */
    g_tim1_handler.Init.Prescaler = psc;                  /* 定时器分频 */
    g_tim1_handler.Init.CounterMode = TIM_COUNTERMODE_UP; /* 向上计数模式 */
    g_tim1_handler.Init.Period = arr;                     /* 自动重装载值 */
    g_tim1_handler.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_PWM_Init(&g_tim1_handler);                    /* 初始化PWM */

    g_tim1_ch1handler.OCMode = TIM_OCMODE_PWM1;                                      /* CH1/2 PWM模式1 */
    g_tim1_ch1handler.Pulse = arr / 2;                                               //设置比较值,此值用来确定占空比，默认比较值为自动重装载值的一半,即占空比为50%
    g_tim1_ch1handler.OCPolarity = TIM_OCPOLARITY_HIGH;                              //输出比较极性为高
    HAL_TIM_PWM_ConfigChannel(&g_tim1_handler, &g_tim1_ch1handler, PWMDAC_TIMX_CHY); //配置TIM1通道1

    HAL_TIM_PWM_Start(&g_tim1_handler, TIM_CHANNEL_1);    /* 开启PWM通道1 */
}

/**
 * @brief       定时器底层驱动，时钟使能，引脚配置
 * @note
 *              此函数会被HAL_TIM_PWM_Init()调用
 * @param       htim:定时器句柄
 * @retval      无
 */
void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim)
{
    GPIO_InitTypeDef GPIO_Initure;

    if (htim->Instance == TIM1)
    {
        __HAL_RCC_TIM1_CLK_ENABLE();     /* 使能定时器1 */
        __HAL_AFIO_REMAP_TIM1_PARTIAL(); /* TIM1通道引脚部分重映射使能 */
        PWMDAC_GPIO_CLK_ENABLE();        /* PWM DAC GPIO 时钟使能 */

        GPIO_Initure.Pin = PWMDAC_GPIO_PIN;
        GPIO_Initure.Mode = GPIO_MODE_AF_PP;
        GPIO_Initure.Pull = GPIO_PULLUP;
        GPIO_Initure.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(PWMDAC_GPIO_PORT, &GPIO_Initure); /* TIMX PWM CHY 引脚模式设置 */
    }
}

/**
 * @brief       设置PWM DAC输出电压
 * @param       vol : 0~3300,代表0~3.3V
 * @retval      无
 */
void pwmdac_set_voltage(uint16_t vol)
{
    float temp = vol;
    temp /= 100;             /* 缩小100倍, 得到实际电压值 */
    temp = temp * 256 / 3.3; /* 将电压转换成PWM占空比 */
    __HAL_TIM_SET_COMPARE(&g_tim1_handler, PWMDAC_TIMX_CHY, temp);  /* 设置新的占空比 */
}



