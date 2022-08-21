#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "dht11.h"

  
int main(void)
{ 
	u8 t=0;			    
	u8 temperature;  	    
	u8 humidity;  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	delay_init(168);  //初始化延时函数
	uart_init(38400);		//初始化串口波特率为115200
	
	LED_Init();					//初始化LED 
 	while(DHT11_Init())	//DHT11初始化	
	{
		
 		delay_ms(200);
	}								   

	while(1)
	{	    	    
 		if(t%10==0)//每100ms读取一次
		{									  
			DHT11_Read_Data(&temperature,&humidity);		//读取温湿度值				
      printf("temperature= %d\r\n",temperature)	;
			
		  printf("humidity=%d\r\n",humidity); 	   
			delay_ms(3000);
		}				   
	 	delay_ms(10);
		t++;
		if(t==20)
		{
			t=0;
			LED0=!LED0;
		}
	}
}






