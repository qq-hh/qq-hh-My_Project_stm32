#include "delay.h"
#include "sys.h"
#include "lcd.h"
#include "touch.h"
#include "gui.h"
#include "test.h"
#include "usart.h"
#include "key.h" 


int main(void)
{	
	u8 t;
	u8 len;
	u8  a;
	SystemInit();//初始化RCC 设置系统主频为72MHZ
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
<<<<<<< HEAD
  uart1_init(9600);
	uart2_init(9600);
=======
  uart1_init(38400);
	uart2_init(38400);
>>>>>>> e8b95b0088448286ec768b82a60064d0bf377976
	delay_init();
	LCD_Init();	   //液晶屏初始化
  DrawTestPage("");
		
  //循环测试
	while(1)
	{


if(USART2_RX_STA&0x8000)
		{					   
			len=USART2_RX_STA&0x3fff;
			printf("message:\r\n");
			for(t=0;t<len;t++)
			{
				USART1->DR=USART2_RX_BUF[t];
				while((USART1->SR&0X40)==0);
			}
			printf("\r\n\r\n");
			
			Show_Str(100,200,WHITE,RED,USART2_RX_BUF,32,0);
			USART1_RX_STA=0;
			USART2_RX_STA=0;
		}		
	//Button_Test();
	
}

}



