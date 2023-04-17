/**
 ****************************************************************************************************
 * @file        24cxx.h
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-24
 * @brief       24CXX ��������
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
 
#ifndef __24CXX_H
#define __24CXX_H

#include "./SYSTEM/sys/sys.h"


#define AT24C01     127
#define AT24C02     255
#define AT24C04     511
#define AT24C08     1023
#define AT24C16     2047
#define AT24C32     4095
#define AT24C64     8191
#define AT24C128    16383
#define AT24C256    32767

/* ������ʹ�õ���24c02�����Զ���EE_TYPEΪAT24C02 */

#define EE_TYPE     AT24C02

void at24cxx_init(void);        /* ��ʼ��IIC */
uint8_t at24cxx_check(void);    /* ������� */
uint8_t at24cxx_read_one_byte(uint16_t addr);                       /* ָ����ַ��ȡһ���ֽ� */
void at24cxx_write_one_byte(uint16_t addr,uint8_t data);            /* ָ����ַд��һ���ֽ� */
void at24cxx_write(uint16_t addr, uint8_t *pbuf, uint16_t datalen); /* ��ָ����ַ��ʼд��ָ�����ȵ����� */
void at24cxx_read(uint16_t addr, uint8_t *pbuf, uint16_t datalen);  /* ��ָ����ַ��ʼ����ָ�����ȵ����� */

#endif













