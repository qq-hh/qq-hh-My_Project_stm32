#include "max6675.h"
#include "sys.h" 
#include "delay.h"
#include "usart.h"	

/*max6675初始化程序*/
void max6675_init1(void){
    GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE );//PORTB时钟使能
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE );//PORTB时钟使能
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_15;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //PB8推挽输出  GPIO_Mode_AF_PP
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIOB9
//	
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7|GPIO_Pin_14;
//	GPIO_InitStructure.GPIO_Mode =GPIO_Mode_IPU;//PB7上拉输入 
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIOB7
	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_15|GPIO_Pin_7|GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //PB8推挽输出  GPIO_Mode_AF_PP
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOE, &GPIO_InitStructure);//初始化GPIOB9
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14|GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode =GPIO_Mode_IPU;//PB7上拉输入 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOE, &GPIO_InitStructure);//初始化GPIOB7

}

u16 max6675_readRawValue1(void)
{
  u16 tmp=0;
  u16 i;
  MAX6675_CS_L;//cs低
  /*按位读取电平*/
  for(i=0;i<16;i++)
          {  
			  /*创建一个沿，按沿读取PB7的端口电平*/
			MAX6675_CK_H;
			delay_us(10);
			if(MAX6675_SO)tmp|=0x0001;
		    //printf("tmp%d---:%d\n",i,tmp);/*这是为了检查错误时按位打印每个数*/
			tmp<<=1;//按位左移
		    MAX6675_CK_L;
			delay_us(10);
                  }
	      MAX6675_CS_H;//cs高		  
   if (tmp&0X04) 
 {
    tmp = 0; //未检测到热电偶
	// printf("未检测到热电偶\n");//测试用
  }
  else 
 {
	tmp<<=1;//去掉D15位
	tmp>>=4;//去掉D0、1、2位
  }
	return tmp;
}
 


float max6675_readTemperature1(void)
{
	u16 d;
	float i,S;
	S=2.142857;//实验数，用温水和热水 环境测得的一个实际差量
	delay_ms(300);
	d=max6675_readRawValue1();
	i=d*1023.75/4095/S;
    return i;
}

u16 max6675_readRawValue2(void)
{
  u16 tmp=0;
  u16 i;
  MAX6675_CS_L1;//cs低
  /*按位读取电平*/
  for(i=0;i<16;i++)
          {  
			  /*创建一个沿，按沿读取PB7的端口电平*/
			MAX6675_CK_H1;
			delay_us(10);
			if(MAX6675_SO1)tmp|=0x0001;
		    //printf("tmp%d---:%d\n",i,tmp);/*这是为了检查错误时按位打印每个数*/
			tmp<<=1;//按位左移
		    MAX6675_CK_L1;
			delay_us(10);
                  }
	      MAX6675_CS_H1;//cs高		  
   if (tmp&0X04) 
 {
    tmp = 0; //未检测到热电偶
	// printf("未检测到热电偶\n");//测试用
  }
  else 
 {
	tmp<<=1;//去掉D15位
	tmp>>=4;//去掉D0、1、2位
  }
	return tmp;
}
 
float max6675_readTemperature2(void)
{
	u16 d;
	float i,S;
	S=2.142857;//实验数，用温水和热水 环境测得的一个实际差量
	delay_ms(300);
	d=max6675_readRawValue2();
	i=d*1023.75/4095/S;
    return i;
}

