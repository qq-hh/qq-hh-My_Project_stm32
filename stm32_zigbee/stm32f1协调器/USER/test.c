#include "lcd.h"
#include "delay.h"
#include "gui.h"
#include "test.h"
#include "touch.h"
#include "key.h" 
#include "led.h"
#include "pic.h"
#include "usart.h"
#include "rgb.h"
//#include "FONT.h"   //文中用到的中文字体
#include "dht11.h"


	
u8 *Direction[4]={"Rotation:0","Rotation:90","Rotation:180","Rotation:270"};

u16 ColorTab[5]={RED,GREEN,BLUE,YELLOW,BRED};//定义颜色数组


void DrawTestPage(u8 *str)
{
//绘制固定栏up
LCD_Clear(BLACK);
LCD_Fill(0,0,lcddev.width,30,BLUE);
//绘制固定栏down
LCD_Fill(0,lcddev.height-20,lcddev.width,lcddev.height,BLUE);
POINT_COLOR=WHITE;
Gui_StrCenter(0,1,WHITE,BLUE,"好好学习",24,1);//居中显示
Gui_StrCenter(0,lcddev.height-18,WHITE,BLUE,"本设备使用触摸屏",16,1);//居中显示
//绘制测试区域
//LCD_Fill(0,20,lcddev.width,lcddev.height-20,WHITE);
}

void cuntest(void){

	LCD_direction(3);
	//绘制固定栏up
LCD_Clear(BLACK);
LCD_Fill(0,0,lcddev.width,30,BLUE);
//绘制固定栏down
LCD_Fill(0,lcddev.height-20,lcddev.width,lcddev.height,BLUE);
POINT_COLOR=WHITE;
Gui_StrCenter(0,1,WHITE,BLUE,"存货",24,1);//居中显示
Gui_StrCenter(0,lcddev.height-18,WHITE,BLUE,"请严格按照规范操作",16,1);//居中显示
//绘制测试区
	Show_Str(400,270,WHITE,RED,"返回",24,0);
	LCD_direction(USE_HORIZONTAL);
	
}

void  quhuo(void){
	LCD_direction(3);
	LCD_Clear(WHITE);
	Show_Str(50,200,BLUE,RED,"待取货",24,0);
	LCD_direction(USE_HORIZONTAL);
	
}

void Rotate_Test(void)
{
	
	
	LCD_direction(3);
	DrawTestPage("");
	LCD_Fill(150,130,220,220,BLUE);
	Show_Str(155,150,WHITE,YELLOW,"存货",24,1);
	LCD_Fill(250,130,320,220,RED);
	Show_Str(255,150,WHITE,YELLOW,"取货",24,1);
	delay_ms(1000);delay_ms(1000);
	LCD_direction(USE_HORIZONTAL);

	
}

void Button_Test(void)
{
	u8 key;
 	  
	TP_Init();
	KEY_Init();
  uart1_init(9600);
   Rotate_Test();

while(1)
	{
	 	key=KEY_Scan();
		tp_dev.scan(0); 		 
		if(tp_dev.sta&TP_PRES_DOWN)			//触摸屏被按下
		{	
			printf("x= %d\r\n",tp_dev.x);
			printf("y= %d\r\n",tp_dev.y);
			
		 	if(tp_dev.x<240&&tp_dev.x>120&&tp_dev.y>270&&tp_dev.y<330)
			{	
			 	printf("1 \r\n");
				cuntest();
			
				
			}
				if(tp_dev.x<295&&tp_dev.x>185&&tp_dev.y>43&&tp_dev.y<75)//存货返回
			{	
			 
				 Rotate_Test();
			}
				if(tp_dev.x<240&&tp_dev.x>120&&tp_dev.y>160&&tp_dev.y<220)
			{	
			 	quhuo();
			  
			}
			delay_ms(30);
		}

}
}


