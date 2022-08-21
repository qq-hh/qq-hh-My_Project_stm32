#include "delay.h"
#include "sys.h"
#include "lcd.h"
#include "touch.h"
#include "gui.h"
#include "test.h"
#include "usart.h"
#include "dht11.h"


int main(void)
{	
	SystemInit();//初始化RCC 设置系统主频为72MHZ
  uart_init(115200);
	delay_init();
	LCD_Init();	   //液晶屏初始化
  //循环测试
	DHT11_Init();
	while(1)
	{

		
		Button_Test();
	}
}

