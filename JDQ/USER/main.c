#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "jdq.h"
#include "key.h"

//////////////////////////////////////////////////////////////////////////////////	 

//������ʹ��stm32f103zet6��Ƭ��������ԭ�Ӿ�Ӣ�棬������·�̵���				
//�̵����������ſ���ʼ���ļ�
//ʵ�������ĸ����ذ������ƶ�Ӧ�ļ̵�������ҿ����ڼ̵�������ӵ�������ݣ�����ʵ��Ч��

////////////////////////////////////////////////////////////////////////////////// 	   

 int main(void)
 {	vu8 key =0;
	delay_init();	    
	LED_Init();		
  relay_init();	
  KEY_Init();	 
	while(1)
	{

	   key=KEY_Scan(0);	//�õ���ֵ
	   	if(key)
		{						   
			switch(key)
			{				 
				case WKUP_PRES:	//���Ʒ�����
					JDQ1=!JDQ1;
					break; 
				case KEY1_PRES:	//����LED1��ת	 
				JDQ2=!JDQ2;
				 break; 
				case KEY0_PRES:	//ͬʱ����LED0,LED1��ת 
		          JDQ3=!JDQ3;
					break;
				case KEY2_PRES:	//����LED1��ת	
					JDQ4=!JDQ4;
					break;
			}
		}else delay_ms(10); 
		
		
	}
 }


 
 
 