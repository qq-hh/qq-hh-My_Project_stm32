#include "delay.h"
#include "sys.h"
#include "lcd.h"
#include "touch.h"
#include "gui.h"
#include "test.h"
#include "usart.h"
#include "key.h"

extern u8   g_KeyActionFlag;



int main(void)
{	
	SystemInit();//初始化RCC 设置系统主频为72MHZ
    uart_init(115200);
	delay_init();
	LCD_Init();	   //液晶屏初始化
    KEY_Init();
	while(1)
	{
		
//	Pic_test();	
    Key_Scan();		
    if(g_KeyActionFlag==LONG_KEY)
    {
        printf("长按");
        
    }
    else if(g_KeyActionFlag==SHORT_KEY)
    {
        printf("短按");
        
    }
		
	}
}

