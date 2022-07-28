#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "usart.h"	 
#include "usmart.h"
#include "sram.h"
#include "malloc.h"
#include "enc28j60.h" 	 
#include "lwip/netif.h"
#include "lwip_comm.h"
#include "lwipopts.h"
#include "timer.h"
#include "tcp_server_demo.h"
#include "cjson.h"

extern u8 tcp_server_flag;	 //TCP Server 测试全局状态标记变量
//加载UI
//mode:
//bit0:0,不加载;1,加载前半部分UI
//bit1:0,不加载;1,加载后半部分UI
void lwip_test_ui(u8 mode)
{
	u8 buf[30]; 

	if(mode&1<<0)
	{
		
	}
	if(mode&1<<1)
	{

		if(lwipdev.dhcpstatus==2)sprintf((char*)buf,"DHCP IP:%d.%d.%d.%d",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);//打印动态IP地址
		else sprintf((char*)buf,"Static IP:%d.%d.%d.%d",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);//打印静态IP地址
	
	}
}

int main(void)
{	 
	u8 key;
	delay_init();	    	//延时函数初始化	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);		//设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	uart_init(115200);	 	//串口初始化为115200
 	LED_Init();			    //LED端口初始化

	KEY_Init();	 			//初始化按键
	TIM3_Int_Init(1000,719);//定时器3频率为100hz
 	usmart_dev.init(72);	//初始化USMART		
 	FSMC_SRAM_Init();		//初始化外部SRAM
	my_mem_init(SRAMIN);	//初始化内部内存池
	my_mem_init(SRAMEX);	//初始化外部内存池
		
	lwip_test_ui(1);		//加载前半部分UI		    
	while(lwip_comm_init()) //lwip初始化
	{

		delay_ms(1200);
	 
	}
	
#if LWIP_DHCP   //使用DHCP
	while((lwipdev.dhcpstatus!=2)&&(lwipdev.dhcpstatus!=0XFF))//等待DHCP获取成功/超时溢出
	{
		lwip_periodic_handle();	//LWIP内核需要定时处理的函数
	}
#endif
	lwip_test_ui(2);		//加载后半部分UI 
	delay_ms(500);			//延时1s
	delay_ms(500);
	tcp_server_test();  	//TCP Server模式
  	while(1)
	{	
		key = KEY_Scan(0);
		if(key == KEY1_PRES)		//按KEY1键建立连接
		{
			if((tcp_server_flag & 1<<6)) printf("TCP连接已经建立,不能重复连接\r\n");	//如果连接成功,不做任何处理
			else tcp_server_test();		//当断开连接后,调用tcp_server_test()函数
		}
		delay_ms(10);
	}
}

