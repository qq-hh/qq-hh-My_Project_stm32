#include "delay.h"
#include "sys.h"
#include "lcd.h"
#include "touch.h"
#include "gui.h"
#include "test.h"
#include "usart.h"
#include "max6675.h"

int main(void)
{	
	SystemInit();//初始化RCC 设置系统主频为72MHZ
  uart_init(115200);
	delay_init();
	LCD_Init();	 
	max6675_init1();
	u8 t=0;
	u8 temp1;
  u8 temp2;
	//float temper;
	while(1)
	{
	LCD_Fill(0,0,lcddev.width,20,BLUE);
  LCD_Fill(0,lcddev.height-20,lcddev.width,lcddev.height,BLUE);
  
  
		Show_Str(50,50,BLUE,YELLOW,"temp1:       C",16,1);
		Show_Str(50,90,BLUE,YELLOW,"temp2:       C",16,1);
		if(t%10==0)			//每100ms读取一次
		{									  
	  temp1 = max6675_readTemperature1();
		printf("temp1= %d\r\n",temp1);
    LCD_ShowNum(120,50,temp1,2,16);		
		temp2 = max6675_readTemperature2();
		printf("temp1= %d\r\n",temp2);
    LCD_ShowNum(120,90,temp2,2,16);		
		
		}				   
	 	delay_ms(10);
		t++;
		if(t==20)
		{
			t=0;
			//LED0=!LED0;
		}
		
	}
	
}




