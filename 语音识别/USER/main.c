#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "usart.h"
 

 int main(void)
 {		
  u8 a;
	delay_init();	    	 //延时函数初始化	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	uart_init(115200);	 //串口初始化为115200
 	LED_Init();			     //LED端口初始化
	KEY_Init();          //初始化与按键连接的硬件接口
	 
 	while(1)
	{
		if(USART_GetFlagStatus(USART1,USART_FLAG_RXNE) != RESET){  //查询串口待处理标志位
			a =USART_ReceiveData(USART1);//读取接收到的数据
//			switch (a){
//				case '0':
//				printf("hello ");
//					break;
//				case '1':
//					printf("hello111");
//					break;
//				case '2':		
//					printf("BUZZER "); //把收到的数据发送回电脑
//					break;
//				default:
//					break;
//			}		  
			printf("message: %d\r\n",a);
		
		//while 循环，demo模板
	}	 
 }

}
 