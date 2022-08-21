#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "jdq.h"
#include "key.h"

//////////////////////////////////////////////////////////////////////////////////	 

//本程序使用stm32f103zet6单片机，正点原子精英版，连接四路继电器				
//继电器开关引脚看初始化文件
//实验现象：四个开关按键控制对应的继电器，大家可以在继电器出添加电机、灯泡，看下实验效果

////////////////////////////////////////////////////////////////////////////////// 	   

 int main(void)
 {	vu8 key =0;
	delay_init();	    
	LED_Init();		
  relay_init();	
  KEY_Init();	 
	while(1)
	{

	key=KEY_Scan(0);	//得到键值
	   	if(key)
		{						   
			switch(key)
			{				 
				case WKUP_PRES:	//控制蜂鸣器
					JDQ1=!JDQ1;
					break; 
				case KEY1_PRES:	//控制LED1翻转	 
				JDQ2=!JDQ2;
				 break; 
				case KEY0_PRES:	//同时控制LED0,LED1翻转 
		     JDQ3=!JDQ3;
					break;
				case KEY2_PRES:	//控制LED1翻转	
					JDQ4=!JDQ4;
					break;
			}
		}else delay_ms(10); 
		
		
	}
 }


 
 
 