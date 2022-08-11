#include "sys.h"
#include "usart.h"
#include "delay.h"

typedef  unsigned long	 uint32; 



//加入以下代码,支持printf函数,而不需要选择use MicroLIB
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 

/*使用microLib的方法*/
 /* 
int fputc(int ch, FILE *f)
{
	USART_SendData(USART1, (uint8_t) ch);

	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET) {}	
   
    return ch;
}
int GetKey (void)  { 

    while (!(USART1->SR & USART_FLAG_RXNE));

    return ((int)(USART1->DR & 0x1FF));
}
*/
int UartGet (void)  { 

    while (!(USART1->SR & USART_FLAG_RXNE));

    return ((int)(USART1->DR & 0x1FF));
} 

extern u8 rx_flag_finished;
extern u8 RX_BUF[];
extern u8 rx_count;
extern u8 ok;
void CheckBusy(void)  
{
	while(1)
	{
   if(ok==0x0f)
		 break;
	}		
	
	ok=0;
}


    void get_var(unsigned char *val)
		{	 
			for(;;)
			{ 
				if((RX_BUF[0]=='V')&&(RX_BUF[1]=='A')&&(RX_BUF[2]=='R'))
				{
					RX_BUF[0]=0;
					RX_BUF[1]=0;
					RX_BUF[2]=0;
					val[0]=RX_BUF[3];
					val[1]=RX_BUF[4];
					val[2]=RX_BUF[5];
					val[3]=RX_BUF[6];
					rx_flag_finished=0;	
					rx_count=0;
					break;				
				}	
			}

		}

		
char rch[5][3];     //xxx:xxx,xxx,xxx,xxx
u8 i=0;	
u8 p=0;//
u8 cmd=0;		//==1
u8 cmdok=0;	//==1 
u8 ok=0x00;
void getch(u8 c)
{	
	 if (c=='{')
	{	i=0;
		cmd=1;
		p=0;
	}
	else if(c=='}')
	{	cmd=0;
		cmdok=1;
	}
	else if((c==':')||(c==','))
	{
			p++;
			i=0;
	}		
	else if (cmd==1)
	{	
		if (i<3) rch[p][i]=c;
		i++;
	}
	else	if(c=='O')
	{
		ok=(ok&0x00)|(0x01);
	}		
	else if(c=='K')
	{
		ok=(ok&0x0d)|(0x02);
	}	
  else if(c=='\r')
	{
		ok=(ok&0x0b)|(0x04);
	}
  else if(c=='\n')
	{
	 ok=(ok&0x07)|(0x08);
	}
	
}
unsigned char val[4];
unsigned char GetValue(void)  
{	
	unsigned char m,n;
	while(1)
	{			
		if(cmdok==1)
		{			
			if ((rch[0][0]=='V')&&(rch[0][1]=='A')&&(rch[0][2]=='R'))
			{	
				/*第一个字节*/
				if ((rch[1][2]>=0x30) && (rch[1][2]<=0x39))//100以上的数  
				{		
					val[0]=(rch[1][0]-0x30)*100+(rch[1][1]-0x30)*10+(rch[1][2]-0x30);
					
				}
				else if((rch[1][1]>=0x30) && (rch[1][1]<=0x39)) //10以上的数
				{
					val[0]=(rch[1][0]-0x30)*10+(rch[1][1]-0x30);
				}		
				else if((rch[1][0]>=0x30) && (rch[1][0]<=0x39))
				{
					val[0]=(rch[1][0]-0x30);						
				}
				/*第二个字节*/
					if ((rch[2][2]>=0x30) && (rch[2][2]<=0x39))//100以上的数
				{		
					val[1]=(rch[2][0]-0x30)*100+(rch[2][1]-0x30)*10+(rch[2][2]-0x30);				
				}
				else if((rch[2][1]>=0x30) && (rch[2][1]<=0x39)) //10以上的数
				{
					val[1]=(rch[2][0]-0x30)*10+(rch[2][1]-0x30);
				}		
				else if((rch[2][0]>=0x30) && (rch[2][0]<=0x39))
				{
					val[1]=(rch[2][0]-0x30);						
				}
				
			/*第三个字节*/
					if ((rch[3][2]>=0x30) && (rch[3][2]<=0x39))//100以上的数
				{		
					val[2]=(rch[3][0]-0x30)*100+(rch[3][1]-0x30)*10+(rch[3][2]-0x30);
					
				}
			
				else if((rch[3][1]>=0x30) && (rch[3][1]<=0x39)) //10以上的数
				{
					val[2]=(rch[3][0]-0x30)*10+(rch[3][1]-0x30);
				}		
				else if((rch[3][0]>=0x30) && (rch[3][0]<=0x39))
				{
					val[2]=(rch[3][0]-0x30);						
				}
				/*第四个字节*/
					if ((rch[4][2]>=0x30) && (rch[4][2]<=0x39))//100以上的数
				{		
					val[3]=(rch[4][0]-0x30)*100+(rch[4][1]-0x30)*10+(rch[4][2]-0x30);
					
				}
			
				else if((rch[4][1]>=0x30) && (rch[4][1]<=0x39)) //10以上的数
				{
					val[3]=(rch[4][0]-0x30)*10+(rch[4][1]-0x30);
				}		
				else if((rch[4][0]>=0x30) && (rch[4][0]<=0x39))
				{
					val[3]=(rch[4][0]-0x30);						
				}		
				
			for(n=0;n<5;n++)
					for(m=0;m<3;m++)
				rch[n][m]=0;
				 cmdok=0;
					break;
				
				
		}
			//	cmdok=0;
			
		
		
	}	
		
}
}

unsigned char GetKey(unsigned char *rval)  
{	
	unsigned char m,n;
	while(1)
	{			
		if(cmdok==1)
		{			
			if ((rch[0][0]=='U')&&(rch[0][1]=='P'))
			{		
				*rval=1;
						cmdok=0;
				for(n=0;n<5;n++)
					for(m=0;m<3;m++)
				rch[n][m]=0;
	
					break;	
				
			}
			else if ((rch[0][0]=='D')&&(rch[0][1]=='N'))
			{
						*rval=0;
				   cmdok=0;
				for(n=0;n<5;n++)
					for(m=0;m<3;m++)
				rch[n][m]=0;
					break;	
			}

		
		}
		
	}	
		
}


//extern u8 rx_flag_finished;
//extern u8 RX_BUF[];
//extern u8 rx_count;
//void CheckBusy(void)  
//{
//	for(;;)
//	{
//		if(rx_flag_finished==0xc0)
//		{
//			if((RX_BUF[0]=='O')&&(RX_BUF[1]=='K'))
//			{
//				RX_BUF[0]=0;
//				RX_BUF[1]=0;
//				rx_flag_finished=0;	
//				rx_count=0;
//        break;				
//			}	
//				
//		}	
//	}
//	//	busy_flag=1;
//}


//unsigned char CheckState(void)  
//{
//	unsigned char state=0xff;
//	
//	for(;;)
//	{
//		if(rx_flag_finished==0xc0)
//		{
//			if((RX_BUF[0]=='O')&&(RX_BUF[1]=='K')&&(RX_BUF[2]=='\0'))
//			{
//				RX_BUF[0]=0;
//				RX_BUF[1]=0;
//				rx_flag_finished=0;	
//				rx_count=0;
//				state=0;
//        break;				
//			}	
//			else
//				if((RX_BUF[0]=='D')&&(RX_BUF[1]=='N')&&(RX_BUF[2]=='\0'))
//			{
//				RX_BUF[0]=0;
//				RX_BUF[1]=0;
//				rx_flag_finished=0;	
//				rx_count=0;
//				state=1;
//        break;				
//			}	
//			else 
//				
//			if((RX_BUF[0]=='U')&&(RX_BUF[1]=='P')&&(RX_BUF[2]=='\0'))
//			{
//				RX_BUF[0]=0;
//				RX_BUF[1]=0;
//				rx_flag_finished=0;	
//				rx_count=0;
//				state=2;
//        break;				
//			}	
//				
//		}	
//	}
//	return state;
//	//	busy_flag=1;
//}

void UartSend(char * databuf) 
{ 
	u8 i=0;
	while (1)
	{ 
		if ((*databuf)!=0)//直到数据都发送完成 
	{ 
		USART_SendData(USART1, *databuf); //发送一个字节数据
		while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET){}; //
		databuf++;//i++;
	}
	else return;
	}
}


u8 USART_RX_BUF[64];     //接收缓冲,最大64个字节.
//接收状态
//bit7，接收完成标志
//bit6，接收到0x0d
//bit5~0，接收到的有效字节数目
u8 USART_RX_STA=0;       //接收状态标记

#ifdef AUTO_TEST
	#define AUTO_NEXT
#else
	#define MANUA_NEXT
#endif


#define AUTO_NEXT	    //自动
//#define MANUA_NEXT	  //手动

#define KEY_UP    GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_0)//定义“下一步”按键
#define KEY_DOWM  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_1)//定义上一步”按键
#define KEY_Stop  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_2)//定义"暂停"
  


void uart_init(u32 bound){
    //GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO|RCC_APB2Periph_GPIOD|RCC_APB2Periph_GPIOC, ENABLE);
	
	
	
	     //_KEY_STEP   PC0  UP
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//上拉
    GPIO_Init(GPIOC, &GPIO_InitStructure);
	
		     //_KEY_STEP   PC1  DOWN
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//上拉
    GPIO_Init(GPIOC, &GPIO_InitStructure);
	
		     //_KEY_Stop   PC2  暂停
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	     //_LED   PD2
		GPIO_SetBits(GPIOD,GPIO_Pin_2);	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;	   //
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure); //	
	GPIO_SetBits(GPIOD,GPIO_Pin_2);
	
     //USART1_TX   PA.9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
   
    //USART1_RX	  PA.10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);  

   //Usart1 NVIC 配置

    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//

	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器USART1
  
   //USART 初始化设置
   
	USART_InitStructure.USART_BaudRate = bound;//一般设置为9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    USART_Init(USART1, &USART_InitStructure);
   

    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启中断
   
    USART_Cmd(USART1, ENABLE);                    //使能串口 

}

//extern void SPI_Flash_Write(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite);
extern void SPI_Flash_Write_Page(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite);



//u8 busy_flag=1;

//void USART1_IRQHandler(void)                	//??1??????
//{
//	u8 Res=1;
//	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //????(?????????0x0d 0x0a??)
//	{
//		Res =USART_ReceiveData(USART1);//(USART1->DR);	//????????
//		if(Res=='O')
//		busy_flag=0;
//		else
//		busy_flag=1;
//	//	printf("1");
//  }
//} 

u8 RX_BUF[4];
u8 rx_flag_finished=0;
u8 rx_count=0;
void USART1_IRQHandler(void)                	//??1??????
{
	u8 Res=1;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //????(?????????0x0d 0x0a??)
	{
		Res =USART_ReceiveData(USART1);//(USART1->DR);	//????????
		getch(Res);
//		if(rx_flag_finished!=0xc0)//?????
//		{
//			if(rx_flag_finished&0x40)//????0x0d
//			{
//				if(Res==0x0a)
//				{
//					rx_flag_finished|=0x80;	//????? rx_flag_finished=0xc0
//					RX_BUF[rx_count]='\0'; //??????????????
//				}
//				else 
//				{
//					rx_flag_finished=0;//????,????					
//					rx_count=0;
//				}
//			}
//			else //????0X0D
//			{	
//				if(Res!=0x0d)
//				{
//					RX_BUF[rx_count++]=Res ;	
//				}
//				else
//				{
//					rx_flag_finished|=0x40;
//				}
//			}
//		}  
		
		

//		if(Res=='O')
//		busy_flag=0;
//		else
//		busy_flag=1;
  }
} 



