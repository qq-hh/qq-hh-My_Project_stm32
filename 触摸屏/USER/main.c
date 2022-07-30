#include "delay.h"
#include "sys.h"
#include "lcd.h"
#include "touch.h"
#include "gui.h"
#include "test.h"
#include "usart.h"
int main(void)
{	
	SystemInit();//初始化RCC 设置系统主频为72MHZ
  uart_init(115200);
	delay_init();
	LCD_Init();	   //液晶屏初始化

	while(1)
	{
		
	Pic_test();			

		
	}
}

