#ifndef __USART_H
#define __USART_H
#include "stm32f10x_usart.h"
#include "stdio.h"	
#include "sys.h" 
#define USART1_REC_LEN  		200  
#define USART2_REC_LEN      200   
#define EN_USART1_RX 			1		 
#define EN_USART2_RX      1    
extern u8 USART1_RX_BUF[USART1_REC_LEN];
extern u16 USART1_RX_STA;

extern u8 USART2_RX_BUF[USART1_REC_LEN];
extern u16 USART2_RX_STA;

void uart1_init(u32 bound);
void uart2_init(u32 bound);

#endif


