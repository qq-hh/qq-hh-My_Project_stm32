#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "key.h"
#include "led.h"

typedef  unsigned long	 uint32; 

	//����RX����PA10��TX����PA9				
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
	SystemInit();//��ʼ��RCC ����ϵͳ��ƵΪ72MHZ
	delay_init();	     //��ʱ��ʼ��
	uart_init(115200);	 //���ڳ�ʼ��Ϊ115200
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
	case WKUP_PRES:	//���Ʒ�����
				UartSend("JUMP(1);\r\n");//��ҳ
		    CheckBusy();
	    	delay_ms(100);
					break; 
		case  KEY1_PRES:
    UartSend("SET_BTN(3,1);SET_POINT(2,90);SET_PROG(1,50);\r\n");//���ư�����ָ�롢������
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


