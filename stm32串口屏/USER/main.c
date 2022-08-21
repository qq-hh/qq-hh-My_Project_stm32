#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "key.h"
#include "led.h"

typedef  unsigned long	 uint32; 

	//连线RX连接PA10，TX连接PA9				
extern unsigned char val[];
int main(void)
{	
		vu8 key1=0;

	unsigned char i,k;
	unsigned char ret;
	char buf[128];
	unsigned char key=0xff;
	unsigned char data_h,data_l;
	unsigned short data;
	SystemInit();//初始化RCC 设置系统主频为72MHZ
	delay_init();	     //延时初始化
	uart_init(115200);	 //串口初始化为115200
	SPI_Flash_Init(); 
	delay_ms(500);
	SPI_Flash_Read(&data_h,0,1);  
	SPI_Flash_Read(&data_l,1,1);  
	data=data_h*256+data_l;	
	KEY_Init();
	LED_Init();

    while(1)
  {			
    delay_ms(500); 

		key1=KEY_Scan(0);
		if(key1){
	switch(key1){
	case WKUP_PRES:	//控制蜂鸣器
				UartSend("JUMP(1);\r\n");//翻页
		    CheckBusy();
	    	delay_ms(100);
					break; 
		case  KEY1_PRES:
    UartSend("SET_BTN(3,1);SET_POINT(2,90);SET_PROG(1,50);\r\n");//控制按键、指针、进度条
		CheckBusy();
		LED1=!LED1;
		delay_ms(10); 
		break;
		case  KEY0_PRES:
    UartSend("SET_BTN(3,0);SET_POINT(2,150);SET_PROG(1,100);\r\n");
		CheckBusy();
		LED0=!LED0;
		delay_ms(100);
		break;
	
		
	}
		}
	}
}


