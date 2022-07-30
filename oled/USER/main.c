#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "usart.h"
#include "stm32f10x.h"
#include "OLED_I2C.h"
#include "delay.h" 
//////////////////////////////////////////////////////////////////////////////////	 
//使用stm32f102zet6，oled-4pin显示屏
//字体取模在codetab.h 文件里
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////  

int main(void)
{
	unsigned char i;
	extern const unsigned char BMP1[];
	
	delay_init();
	I2C_Configuration();
	OLED_Init();
	
	while(1)
	{
		OLED_Fill(0xFF);//全屏点亮
	  delay_ms(5000);
		OLED_Fill(0x00);//全屏灭
		delay_ms(5000);
		for(i=0;i<5;i++)
		{
			OLED_ShowCN(22+i*16,0,i);//测试显示中文
		}
		delay_ms(5000);
		OLED_ShowStr(0,3,"Hello world",1);//测试6*8字符
		OLED_ShowStr(0,4,"example",2);				//测试8*16字符
		delay_ms(5000);
		OLED_CLS();//清屏
		OLED_OFF();//测试OLED休眠
    delay_ms(5000);
		OLED_ON();//测试OLED休眠后唤醒
		OLED_DrawBMP(0,0,128,8,(unsigned char *)BMP1);//测试BMP位图显示
		delay_ms(5000);
	}
}