#ifndef __MAX6675_H
#define __MAX6675_H
 
#include "stm32f10x.h"
#include "sys.h" 

//#define MAX6675_CS_H   GPIO_SetBits(GPIOB,GPIO_Pin_9)
//#define MAX6675_CS_L   GPIO_ResetBits(GPIOB,GPIO_Pin_9)
//#define MAX6675_CK_H   GPIO_SetBits(GPIOB,GPIO_Pin_11)
//#define MAX6675_CK_L   GPIO_ResetBits(GPIOB,GPIO_Pin_11)
//#define MAX6675_SO  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_7)
//#define MAX6675_CS_H1   GPIO_SetBits(GPIOB,GPIO_Pin_12)
//#define MAX6675_CS_L1   GPIO_ResetBits(GPIOB,GPIO_Pin_12)
//#define MAX6675_CK_H1   GPIO_SetBits(GPIOB,GPIO_Pin_15)
//#define MAX6675_CK_L1   GPIO_ResetBits(GPIOB,GPIO_Pin_15)
//#define MAX6675_SO1  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_14)  



#define MAX6675_CS_H   GPIO_SetBits(GPIOE,GPIO_Pin_7)
#define MAX6675_CS_L   GPIO_ResetBits(GPIOE,GPIO_Pin_7)
#define MAX6675_CK_H   GPIO_SetBits(GPIOE,GPIO_Pin_9)
#define MAX6675_CK_L   GPIO_ResetBits(GPIOE,GPIO_Pin_9)
#define MAX6675_SO  GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_11)

#define MAX6675_CS_H1   GPIO_SetBits(GPIOE,GPIO_Pin_12)
#define MAX6675_CS_L1   GPIO_ResetBits(GPIOE,GPIO_Pin_12)
#define MAX6675_CK_H1   GPIO_SetBits(GPIOE,GPIO_Pin_15)
#define MAX6675_CK_L1   GPIO_ResetBits(GPIOE,GPIO_Pin_15)
#define MAX6675_SO1  GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_14)  
void max6675_init1(void);			 //初始化max6675模块 
u16 max6675_readRawValue1(void);/*按位读取电平的原始数据函数*/
float max6675_readTemperature1(void);/*读取温度函数*/
 
 
void max6675_init2(void);			
u16 max6675_readRawValue2(void);
float max6675_readTemperature2(void);
                                                                                                                                     
#endif

