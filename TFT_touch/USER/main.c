/****************************************************************************************************
//=========================================电源接线================================================//
//     LCD模块                STM32单片机
//      VCC          接        DC5V/3.3V      //电源
//      GND          接          GND          //电源地
//=======================================液晶屏数据线接线==========================================//
//本模块默认数据总线类型为SPI总线
//     LCD模块                STM32单片机    
//    SDI(MOSI)      接          PB15         //液晶屏SPI总线数据写信号
//    SDO(MISO)      接          PB14         //液晶屏SPI总线数据读信号，如果不需要读，可以不接线
//=======================================液晶屏控制线接线==========================================//
//     LCD模块 					      STM32单片机 
//       LED         接          PB9          //液晶屏背光控制信号，如果不需要控制，接5V或3.3V
//       SCK         接          PB13         //液晶屏SPI总线时钟信号
//      DC/RS        接          PB10         //液晶屏数据/命令控制信号
//       RST         接          PB12         //液晶屏复位控制信号
//       CS          接          PB11         //液晶屏片选控制信号
//=========================================触摸屏触接线=========================================//
//如果模块不带触摸功能或者带有触摸功能，但是不需要触摸功能，则不需要进行触摸屏接线
//	   LCD模块                STM32单片机 
//      T_IRQ        接          PC10         //触摸屏触摸中断信号
//      T_DO         接          PC2          //触摸屏SPI总线读信号
//      T_DIN        接          PC3          //触摸屏SPI总线写信号
//      T_CS         接          PC13         //触摸屏片选控制信号
//      T_CLK        接          PC0          //触摸屏SPI总线时钟信号
程序主要侧重方面：1、中英文字体显示，设置字体颜色，背景色、字体大小（16位、24位、32位），如何取模
                  2、图片显示，如何取模。
									3、触摸功能的使用，点击某一按钮，有所反应。触摸功能单独写一个工程
**************************************************************************************************/	
#include "delay.h"
#include "sys.h"
#include "lcd.h"
#include "touch.h"
#include "gui.h"
#include "usart.h"
#include "touch.h"
#include "key.h" 
#include "led.h"
#include "pic.h"
#include "rgb.h"
//#include "FONT.H"
u16 ColorTab[5]={RED,GREEN,BLUE,YELLOW,BRED};//定义颜色数组
void DrawTestPage(u8 *str)
{
//绘制固定栏标题
LCD_Clear(WHITE);
LCD_Fill(0,0,lcddev.width,20,BLUE);
//绘制固定栏down
LCD_Fill(0,lcddev.height-20,lcddev.width,lcddev.height,BLUE);
POINT_COLOR=WHITE;
Gui_StrCenter(0,2,WHITE,BLUE,"触摸屏的使用",16,1);//居中显示
Gui_StrCenter(0,lcddev.height-18,WHITE,BLUE,"好好学习，天天向上!",16,1);//居中显示

}


//触摸屏功能可以看串口打印触摸点的坐标，我设计了几个按钮，加了一个RGB共阳极小灯泡，连接线详见rgb.c
int main(void)
{	
	SystemInit();//初始化RCC 设置系统主频为72MHZ
  uart_init(115200);
	delay_init();
	LCD_Init();	   //液晶屏初始化
  DrawTestPage("");
	u8 key;
	TP_Init();
	KEY_Init();
	RGB_Init();
	LCD_Fill(100,50,150,100,RED);
	Show_Str(108,65,BLUE,YELLOW,"LED1",16,1);
	LCD_Fill(100,150,150,200,BLUE);
	Show_Str(108,165,	RED,YELLOW,"LED2",16,1);
	LCD_Fill(100,250,150,300,GREEN);
	Show_Str(108,265,BLUE,YELLOW,"LED3",16,1);
	while(1)
	{
	key=KEY_Scan();
		tp_dev.scan(0); 		 
		if(tp_dev.sta&TP_PRES_DOWN)			//触摸屏被按下
		{	
			//printf("x= %d\r\n",tp_dev.x);
		 	if(tp_dev.x<150&&tp_dev.x>100&&tp_dev.y>50&&tp_dev.y<100)
			{	
				printf("hello \r\n");
				RED1=1;
				GREEN1=0;
				BLUE1=0;
				}
			 if(tp_dev.x<150&&tp_dev.x>100&&tp_dev.y>150&&tp_dev.y<200)
			 {	
				printf("world \r\n");
				RED1=1;
				GREEN1=1;
				BLUE1=0;
					
				}
			 if(tp_dev.x<150&&tp_dev.x>100&&tp_dev.y>250&&tp_dev.y<300)
			{	
				printf("lalala \r\n");
		
				RED1=1;
				GREEN1=0;
				BLUE1=1;
					
				}
				
				}
		  delay_us(200000);
		   
			}
		
		
		
	}

	
	
