#include "enc28j60.h"
#include "spi.h"
#include "delay.h"
#include "usart.h"
#include "lwip_comm.h"
#include "led.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK ENC28J60模块
//ENC28J60驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2015/4/27
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////

dev_strucrt enc28j60_dev;

//初始化ENC28J60
//macaddr:MAC地址
//返回值:0,初始化成功;
//       1,初始化失败;
u8 ENC28J60_Init(void)
{
	u8 version;
	u16 retry=0;
	u32 temp;
	
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef  SPI_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOG|RCC_APB2Periph_AFIO, ENABLE);	 //使能PB,G端口时钟
    	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;				 //PB12上拉 防止W25X的干扰
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);					//初始化指定IO
 	GPIO_SetBits(GPIOB,GPIO_Pin_12);						//上拉				
 	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7|GPIO_Pin_8;	//PG8 7 推挽 	  
 	GPIO_Init(GPIOG, &GPIO_InitStructure);
  
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_6;   			//中断引脚PG6上拉输入
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 
	GPIO_Init(GPIOG, &GPIO_InitStructure);
	
	//PG6外部中断，中断线6
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOG,GPIO_PinSource6);
	
	EXTI_InitStructure.EXTI_Line = EXTI_Line6;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	
	EXTI_ClearITPendingBit(EXTI_Line6); //清除中断线6挂起标志位
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;			//外部中断线6
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	//抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;			//子优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	GPIO_SetBits(GPIOG,GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8);	//PG6/7/8上拉						  
	SPI2_Init();    		//初始化SPI	 
	SPI_Cmd(SPI2, DISABLE); // SPI外设不使能

	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //SPI设置为双线双向全双工
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		//SPI主机
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;	//发送接收8位帧结构
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;			//时钟悬空低
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;		//数据捕获于第1个时钟沿
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;			//NSS信号由软件控制
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;		//定义波特率预分频的值:波特率预分频值为256
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	//数据传输从MSB位开始
	SPI_InitStructure.SPI_CRCPolynomial = 7;			//CRC值计算的多项式
	SPI_Init(SPI2, &SPI_InitStructure);  				//根据SPI_InitStruct中指定的参数初始化外设SPIx寄存器
	SPI_Cmd(SPI2, ENABLE); //使能SPI外设
	
	SPI2_SetSpeed(SPI_BaudRatePrescaler_4);	//SPI2 SCK频率为36M/4=9Mhz
	//初始化MAC地址
	temp=*(vu32*)(0x1FFFF7E8);	//获取STM32的唯一ID的前24位作为MAC地址后三字节
	enc28j60_dev.macaddr[0]=2;
	enc28j60_dev.macaddr[1]=0;
	enc28j60_dev.macaddr[2]=0;
	enc28j60_dev.macaddr[3]=(temp>>16)&0XFF;	//低三字节用STM32的唯一ID
	enc28j60_dev.macaddr[4]=(temp>>8)&0XFFF;
	enc28j60_dev.macaddr[5]=temp&0XFF;

	ENC28J60_RST=0;			//复位ENC28J60
	delay_ms(10);	 
	ENC28J60_RST=1;			//复位结束				    
	delay_ms(10);	
	ENC28J60_Write_Op(ENC28J60_SOFT_RESET,0,ENC28J60_SOFT_RESET);	//软件复位
	while(!(ENC28J60_Read(ESTAT)&ESTAT_CLKRDY)&&retry<250)	//等待时钟稳定
	{
		retry++;
		delay_ms(1);
	}	
	if(retry>=250)return 1; //ENC28J60初始化失败
	version=ENC28J60_Get_EREVID();			//获取ENC28J60的版本号
	printf("ENC28J60 Version:%d\r\n",version);	
	
	enc28j60_dev.NextPacketPtr=RXSTART_INIT;
	//接收缓冲器由一个硬件管理的循环FIFO 缓冲器构成。
	//寄存器对ERXSTH:ERXSTL 和ERXNDH:ERXNDL 作
	//为指针，定义缓冲器的容量和其在存储器中的位置。
	//ERXST和ERXND指向的字节均包含在FIFO缓冲器内。
	//当从以太网接口接收数据字节时，这些字节被顺序写入
	//接收缓冲器。 但是当写入由ERXND 指向的存储单元
	//后，硬件会自动将接收的下一字节写入由ERXST 指向
	//的存储单元。 因此接收硬件将不会写入FIFO 以外的单
	//元。
	//设置接收起始字节	
	ENC28J60_Write(ERXSTL,RXSTART_INIT&0XFF);	//设置接收缓冲区起始地址低8位
	ENC28J60_Write(ERXSTH,RXSTART_INIT>>8);		//设置接收缓冲区起始地址高8位
	//设置接收接收字节
	ENC28J60_Write(ERXNDL,RXSTOP_INIT&0XFF);	
	ENC28J60_Write(ERXNDH,RXSTOP_INIT>>8);
	//设置发送起始字节
	ENC28J60_Write(ETXSTL,TXSTART_INIT&0XFF);
	ENC28J60_Write(ETXSTH,TXSTART_INIT>>8);
	//设置发送结束字节
	ENC28J60_Write(ETXNDL,TXSTOP_INIT&0XFF);
	ENC28J60_Write(ETXNDH,TXSTOP_INIT>>8);
	//ERXWRPTH:ERXWRPTL 寄存器定义硬件向FIFO 中
	//的哪个位置写入其接收到的字节。 指针是只读的，在成
	//功接收到一个数据包后，硬件会自动更新指针。 指针可
	//用于判断FIFO 内剩余空间的大小  8K-1500。 
	//设置接收读指针字节
	ENC28J60_Write(ERXRDPTL,RXSTART_INIT&0XFF);
	ENC28J60_Write(ERXRDPTH,RXSTART_INIT>>8);
	//接收过滤器
	//UCEN：单播过滤器使能位
	//当ANDOR = 1 时：
	//1 = 目标地址与本地MAC 地址不匹配的数据包将被丢弃
	//0 = 禁止过滤器
	//当ANDOR = 0 时：
	//1 = 目标地址与本地MAC 地址匹配的数据包会被接受
	//0 = 禁止过滤器
	//CRCEN：后过滤器CRC 校验使能位
	//1 = 所有CRC 无效的数据包都将被丢弃
	//0 = 不考虑CRC 是否有效
	//PMEN：格式匹配过滤器使能位
	//当ANDOR = 1 时：
	//1 = 数据包必须符合格式匹配条件，否则将被丢弃
	//0 = 禁止过滤器
	//当ANDOR = 0 时：
	//1 = 符合格式匹配条件的数据包将被接受
	//0 = 禁止过滤器
	//ENC28J60_Write(ERXFCON,ERXFCON_UCEN|ERXFCON_CRCEN|ERXFCON_PMEN);
	ENC28J60_Write(ERXFCON,0);
	ENC28J60_Write(EPMM0,0X3F);
	ENC28J60_Write(EPMM1,0X30);
	ENC28J60_Write(EPMCSL,0Xf9);
	ENC28J60_Write(EPMCSH,0Xf7);
	//bit 0 MARXEN：MAC 接收使能位
	//1 = 允许MAC 接收数据包
	//0 = 禁止数据包接收
	//bit 3 TXPAUS：暂停控制帧发送使能位
	//1 = 允许MAC 发送暂停控制帧（用于全双工模式下的流量控制）
	//0 = 禁止暂停帧发送
	//bit 2 RXPAUS：暂停控制帧接收使能位
	//1 = 当接收到暂停控制帧时，禁止发送（正常操作）
	//0 = 忽略接收到的暂停控制帧
	ENC28J60_Write(MACON1,MACON1_MARXEN|MACON1_TXPAUS|MACON1_RXPAUS);
	//将MACON2 中的MARST 位清零，使MAC 退出复位状态。
	ENC28J60_Write(MACON2,0x00);
		//bit 7-5 PADCFG2:PACDFG0：自动填充和CRC 配置位
	//111 = 用0 填充所有短帧至64 字节长，并追加一个有效的CRC
	//110 = 不自动填充短帧
	//101 = MAC 自动检测具有8100h 类型字段的VLAN 协议帧，并自动填充到64 字节长。如果不
	//是VLAN 帧，则填充至60 字节长。填充后还要追加一个有效的CRC
	//100 = 不自动填充短帧
	//011 = 用0 填充所有短帧至64 字节长，并追加一个有效的CRC
	//010 = 不自动填充短帧
	//001 = 用0 填充所有短帧至60 字节长，并追加一个有效的CRC
	//000 = 不自动填充短帧
	//bit 4 TXCRCEN：发送CRC 使能位
	//1 = 不管PADCFG如何，MAC都会在发送帧的末尾追加一个有效的CRC。 如果PADCFG规定要
	//追加有效的CRC，则必须将TXCRCEN 置1。
	//0 = MAC不会追加CRC。 检查最后4 个字节，如果不是有效的CRC 则报告给发送状态向量。
	//bit 0 FULDPX：MAC 全双工使能位
	//1 = MAC工作在全双工模式下。 PHCON1.PDPXMD 位必须置1。
	//0 = MAC工作在半双工模式下。 PHCON1.PDPXMD 位必须清零。
	ENC28J60_Write(MACON3,MACON3_PADCFG0|MACON3_TXCRCEN|MACON3_FRMLNEN|MACON3_FULDPX);
	// 最大帧长度 1518
	ENC28J60_Write(MAMXFLL,MAX_FRAMELEN&0XFF);
	ENC28J60_Write(MAMXFLH,MAX_FRAMELEN>>8);
	//配置背对背包间间隔寄存器MABBIPG。当使用
	//全双工模式时，大多数应用使用15h 编程该寄存
	//器，而使用半双工模式时则使用12h 进行编程。
	ENC28J60_Write(MABBIPG,0x15);
	//配置非背对背包间间隔寄存器的低字节
	//MAIPGL。 大多数应用使用12h 编程该寄存器。
	//如果使用半双工模式，应编程非背对背包间间隔
	//寄存器的高字节MAIPGH。 大多数应用使用0Ch
	//编程该寄存器。
	ENC28J60_Write(MAIPGL,0x12);
	ENC28J60_Write(MAIPGH,0x0C);
	//设置MAC地址
	ENC28J60_Write(MAADR5,enc28j60_dev.macaddr[0]);
	ENC28J60_Write(MAADR4,enc28j60_dev.macaddr[1]);
	ENC28J60_Write(MAADR3,enc28j60_dev.macaddr[2]);
	ENC28J60_Write(MAADR2,enc28j60_dev.macaddr[3]);
	ENC28J60_Write(MAADR1,enc28j60_dev.macaddr[4]);
	ENC28J60_Write(MAADR0,enc28j60_dev.macaddr[5]);
	//配置PHY为全双工  LEDB为拉电流
	ENC28J60_PHY_Write(PHCON1,PHCON1_PDPXMD);	
	//HDLDIS：PHY 半双工环回禁止位
	//当PHCON1.PDPXMD = 1 或PHCON1.PLOOPBK = 1 时：
	//此位可被忽略。
	//当PHCON1.PDPXMD = 0 且PHCON1.PLOOPBK = 0 时：
	//1 = 要发送的数据仅通过双绞线接口发出
	//0 = 要发送的数据会环回到MAC 并通过双绞线接口发出
	ENC28J60_PHY_Write(PHCON2,PHCON2_HDLDIS);
	//ECON1 寄存器
	//寄存器3-1 所示为ECON1 寄存器，它用于控制
	//ENC28J60 的主要功能。 ECON1 中包含接收使能、发
	//送请求、DMA 控制和存储区选择位。	   
	ENC28J60_Set_Bank(ECON1);
	//EIE： 以太网中断允许寄存器
	//bit 7 INTIE： 全局INT 中断允许位
	//1 = 允许中断事件驱动INT 引脚
	//0 = 禁止所有INT 引脚的活动（引脚始终被驱动为高电平）
	//bit 6 PKTIE： 接收数据包待处理中断允许位
	//1 = 允许接收数据包待处理中断
	//0 = 禁止接收数据包待处理中断
	ENC28J60_Write_Op(ENC28J60_BIT_FIELD_SET,EIE,EIE_INTIE|EIE_PKTIE|EIE_TXIE|EIE_TXERIE|EIE_RXERIE);
	// enable packet reception
	//bit 2 RXEN：接收使能位
	//1 = 通过当前过滤器的数据包将被写入接收缓冲器
	//0 = 忽略所有接收的数据包
	ENC28J60_Write_Op(ENC28J60_BIT_FIELD_SET,ECON1,ECON1_RXEN);
	printf("ENC28J60 Duplex:%s\r\n",ENC28J60_Get_Duplex()?"Full Duplex":"Half Duplex");	//获取双工方式
	return 0;
}

//读取ENC28J60控制寄存器(带操作码)
//op：操作码
//addr:寄存器地址/参数
//返回值:读到的数据
u8 ENC28J60_Read_Op(u8 op,u8 addr)
{
	u8 dat=0;
	ENC28J60_CS=0;
	dat=op|(addr&ADDR_MASK);
	SPI2_ReadWriteByte(dat);
	dat=SPI2_ReadWriteByte(0XFF);
	//如果是读取MAC/MII寄存器,则第二次读到的数据才是正确的,见手册29页
	if(addr&0x80)dat=SPI2_ReadWriteByte(0XFF);	
	ENC28J60_CS=1;
	return dat;	
}

//向ENC28J60寄存器写入数据(带操作码)
//op：操作码
//addr:寄存器地址
//data:参数
void ENC28J60_Write_Op(u8 op,u8 addr,u8 data)
{
	u8 dat=0;
	ENC28J60_CS=0;
	dat=op|(addr&ADDR_MASK);
	SPI2_ReadWriteByte(dat);
	SPI2_ReadWriteByte(data);
	ENC28J60_CS=1;
}

//读取ENC28J60接收缓存数据
//len:要读取的数据长度
//data:输出数据缓存区(末尾自动添加结束符)
void ENC28J60_Read_Buf(u32 len,u8* data)
{
	ENC28J60_CS=0;
	SPI2_ReadWriteByte(ENC28J60_READ_BUF_MEM);
	while(len)
	{
		len--;
		*data=(u8)SPI2_ReadWriteByte(0);
		data++; 
	}
	*data='\0';
	ENC28J60_CS=1;
}

//向ENC28J60写发送缓存数据
//len:要写入的数据长度
//data:数据缓存区 
void ENC28J60_Write_Buf(u32 len,u8* data)
{
	ENC28J60_CS=0;
	SPI2_ReadWriteByte(ENC28J60_WRITE_BUF_MEM);
	while(len)
	{
		len--;
		SPI2_ReadWriteByte(*data);
		data++;
	}
	ENC28J60_CS=1;
}

//设置ENC28J60寄存器Bank
//ban:要设置的bank
void ENC28J60_Set_Bank(u8 bank)
{
	if((bank&BANK_MASK)!=enc28j60_dev.enc28j60bank)	//和当前bank不一致的时候才设置
	{
		ENC28J60_Write_Op(ENC28J60_BIT_FIELD_CLR,ECON1,(ECON1_BSEL1|ECON1_BSEL0));
		ENC28J60_Write_Op(ENC28J60_BIT_FIELD_SET,ECON1,(bank&BANK_MASK)>>5);
		enc28j60_dev.enc28j60bank=(bank&BANK_MASK);
	}
}

//读取ENC28J60指定寄存器 
//addr:寄存器地址
//返回值:读到的数据
u8 ENC28J60_Read(u8 addr)
{
	ENC28J60_Set_Bank(addr);	//设置BANK
	return ENC28J60_Read_Op(ENC28J60_READ_CTRL_REG,addr);
}

//向ENC28J60指定寄存器写数据
//addr:寄存器地址
//data:要写入的数据	
void ENC28J60_Write(u8 addr,u8 data)
{
	ENC28J60_Set_Bank(addr);
	ENC28J60_Write_Op(ENC28J60_WRITE_CTRL_REG,addr,data);
}

//向ENC28J60的PHY寄存器写入数据
//addr:寄存器地址
//data:要写入的数据	
void ENC28J60_PHY_Write(u8 addr,u16 data)
{
	u16 retry=0;
	ENC28J60_Write(MIREGADR,addr);		//向MIREGADR写入PHY寄存器地址
	ENC28J60_Write(MIWRL,data);			//写入数据的低8字节
	ENC28J60_Write(MIWRH,data>>8);		//写入数据的高8字节
	while((ENC28J60_Read(MISTAT)&MISTAT_BUSY)&&retry<0XFFFF)retry++;	//等待写PHY结束
}

//读取ENC28J60的PHY寄存器
//addr:要读取的寄存器地址
//返回值:读取到的PHY的值
u16 ENC28J60_PHY_Read(u8 addr)
{
	u8 temp;
	u16 phyvalue,retry=0;
	temp=ENC28J60_Read(MICMD);
	ENC28J60_Write(MIREGADR,addr);
	ENC28J60_Write(MICMD,temp|MICMD_MIIRD);		//开始读PHY寄存器
	while((ENC28J60_Read(MISTAT)&MISTAT_BUSY)&&retry<0XFFFF)retry++;	//等待读PHY完成
	ENC28J60_Write(MICMD,temp&(~MICMD_MIIRD));	//读PHY完成
	phyvalue=ENC28J60_Read(MIRDL);			//读取低8位
	phyvalue|=(ENC28J60_Read(MIRDH)<<8);	//读取高8位
	return phyvalue;
}

//读取ENC28J60的版本号
//返回值:ENC28J60版本号
u8 ENC28J60_Get_EREVID(void)
{
	//在EREVID 内也存储了版本信息。 EREVID 是一个只读控
	//制寄存器，包含一个5 位标识符，用来标识器件特定硅片
	//的版本号
	return ENC28J60_Read(EREVID)&0X1F;
}

//获取ENC28J60双工状态
//返回值:1，全双工；0，半双工
u16 ENC28J60_Get_Duplex(void)
{
	u16 temp;
	temp=ENC28J60_PHY_Read(PHCON1)&0X0100;
	return (temp>>8);
}


//通过ENC28J60发送数据包到网络
//len:数据包大小
//packet:数据包
void ENC28J60_Packet_Send(u32 len,u8* packet)
{
	INTX_DISABLE();
	while(ENC28J60_Read(ECON1)&0X08);		//正在发送数据，等待发送完成
	if(len>MAX_FRAMELEN)
	{
		ENC28J60_Write_Op(ENC28J60_BIT_FIELD_SET,MACON3,MACON3_HFRMEN);
		ENC28J60_Write_Op(ENC28J60_BIT_FIELD_SET,0X0E,PKTCTRL_POVERRIDE|PKTCTRL_PHUGEEN);
	}
	//设置发送缓冲区地址写指针入口
	ENC28J60_Write(EWRPTL,TXSTART_INIT&0XFF);
	ENC28J60_Write(EWRPTH,TXSTART_INIT>>8);
	//设置TXND指针，以对应给定的数据包大小	
	ENC28J60_Write(ETXNDL,(TXSTART_INIT+len)&0XFF);
	ENC28J60_Write(ETXNDH,(TXSTART_INIT+len)>>8);
	//写每包控制字节（0x00表示使用macon3的设置） 
	ENC28J60_Write_Op(ENC28J60_WRITE_BUF_MEM,0,0x00);
	//复制数据包到发送缓冲区
 	ENC28J60_Write_Buf(len,packet);
	//发送数据到网络
	ENC28J60_Write_Op(ENC28J60_BIT_FIELD_SET,ECON1,ECON1_TXRTS);
	//复位发送逻辑的问题。参见Rev. B4 Silicon Errata point 12.
	if((ENC28J60_Read(EIR)&EIR_TXERIF))ENC28J60_Write_Op(ENC28J60_BIT_FIELD_CLR,ECON1,ECON1_TXRTS);
	INTX_ENABLE();
}

//从网络获取一个数据包内容
//maxlen:数据包最大允许接收长度
//packet:数据包缓存区
//返回值:收到的数据包长度(字节)									  
u32 ENC28J60_Packet_Receive(u32 maxlen,u8* packet)
{
	u32 rxstat;
	u32 len;  
	INTX_DISABLE();
	if(ENC28J60_Read(EPKTCNT)==0)return 0;  //是否收到数据包?	
	while((ENC28J60_Read(ESTAT)&0X04));		//接收忙，等待接收完成	
	//设置接收缓冲器读指针
	ENC28J60_Write(ERDPTL,(enc28j60_dev.NextPacketPtr));
	ENC28J60_Write(ERDPTH,(enc28j60_dev.NextPacketPtr)>>8);	   
	// 读下一个包的指针
	enc28j60_dev.NextPacketPtr=ENC28J60_Read_Op(ENC28J60_READ_BUF_MEM,0);
	enc28j60_dev.NextPacketPtr|=ENC28J60_Read_Op(ENC28J60_READ_BUF_MEM,0)<<8;
	//读包的长度
	len=ENC28J60_Read_Op(ENC28J60_READ_BUF_MEM,0);
	len|=ENC28J60_Read_Op(ENC28J60_READ_BUF_MEM,0)<<8;
 	len-=4; //去掉CRC计数
	//读取接收状态
	rxstat=ENC28J60_Read_Op(ENC28J60_READ_BUF_MEM,0);
	rxstat|=ENC28J60_Read_Op(ENC28J60_READ_BUF_MEM,0)<<8;
	//限制接收长度	
	if (len>maxlen-1)len=maxlen-1;	
	//检查CRC和符号错误
	// ERXFCON.CRCEN为默认设置,一般我们不需要检查.
	if((rxstat&0x80)==0)len=0;//无效
	else 
	{
		ENC28J60_Read_Buf(len,packet);//从接收缓冲器中复制数据包
	}
	//RX读指针移动到下一个接收到的数据包的开始位置 
	//并释放我们刚才读出过的内存
	ENC28J60_Write(ERXRDPTL,(enc28j60_dev.NextPacketPtr));
	ENC28J60_Write(ERXRDPTH,(enc28j60_dev.NextPacketPtr)>>8);
	//递减数据包计数器标志我们已经得到了这个包 
 	ENC28J60_Write_Op(ENC28J60_BIT_FIELD_SET,ECON2,ECON2_PKTDEC);
	INTX_ENABLE();
	return(len);
}

//中断处理函数
void ENC28J60_ISRHandler(void)
{
	u8 status;
	u8 packetnum;
	u16 temp;
	ENC28J60_Write_Op(ENC28J60_BIT_FIELD_CLR,EIE,EIE_INTIE);	//关闭ENC28J60的全局中断
	status=ENC28J60_Read(EIR);	//读取以太网中断标志寄存器
	if(status&EIR_PKTIF)		//接收到数据,处理数据
	{
		ENC28J60_Write_Op(ENC28J60_BIT_FIELD_CLR,EIR,EIR_PKTIF);	//清除ENC28J60的接收中断标志位
		lwip_pkt_handle();
	}
	if(status&EIR_TXIF)			//以太网发送中断
	{
		ENC28J60_Write_Op(ENC28J60_BIT_FIELD_CLR,EIR,EIR_TXIF);	//清除ENC28J60的接收中断标志位
		
	}
	if(status&EIR_RXERIF)		//接收错误中断标志位
	{
		ENC28J60_Write_Op(ENC28J60_BIT_FIELD_CLR,EIR,EIR_RXERIF);	
		packetnum=ENC28J60_Read(EPKTCNT);
		temp=ENC28J60_Read(ERXRDPTH)<<8;		//读取高字节
		temp|=ENC28J60_Read(ERXRDPTL);			//读取低字节
		temp++;
		ENC28J60_Write(ERXRDPTL,temp&0XFF);		//先写入低字节
		ENC28J60_Write(ERXRDPTH,temp>>8);		//先写入低字节
		ENC28J60_Write_Op(ENC28J60_BIT_FIELD_SET,ECON2,ECON2_PKTDEC);
		printf("接收错误！接收到数据包个数:%d\r\n",packetnum);
	}
	if(status&EIR_TXERIF)		//发送错误中断标志位
	{
		ENC28J60_Write_Op(ENC28J60_BIT_FIELD_CLR,EIR,EIR_TXERIF);
		ENC28J60_Write_Op(ENC28J60_BIT_FIELD_CLR,ESTAT,ESTAT_LATECOL|ESTAT_TXABRT);
		printf("发送错误!\r\n");
	}
	ENC28J60_Write_Op(ENC28J60_BIT_FIELD_SET,EIE,EIE_INTIE);	//打开ENC28J60的全局中断
}

//外部中断线6的中断服务函数
void EXTI9_5_IRQHandler(void)
{
#if SYSTEM_SUPPORT_OS 		//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntEnter();    
#endif
	EXTI_ClearITPendingBit(EXTI_Line6); //清除中断线6挂起标志位
	while(ENC28J60_INT == 0)
	{
		ENC28J60_ISRHandler();
	}
#if SYSTEM_SUPPORT_OS 	//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntExit();  											 
#endif
}


