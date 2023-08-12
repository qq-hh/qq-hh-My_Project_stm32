
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
	SystemInit();//��ʼ��RCC ����ϵͳ��ƵΪ72MHZ
  uart_init(115200);
	delay_init();
	LCD_Init();	   //Һ������ʼ��
  //ѭ������
	DHT11_Init();
	while(1)
	{

		
		Button_Test();
	}
}

