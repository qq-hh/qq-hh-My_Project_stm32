#include "netif/ethernetif.h" 
#include "enc28j60.h"  
#include "lwip_comm.h" 
#include "malloc.h"
#include "netif/etharp.h"  
#include "string.h" 

//由ethernetif_init()调用用于初始化硬件
//netif:网卡结构体指针 
//返回值:ERR_OK,正常
//       其他,失败
static err_t low_level_init(struct netif *netif)
{
	netif->hwaddr_len = ETHARP_HWADDR_LEN; //设置MAC地址长度,为6个字节
	//初始化MAC地址,设置什么地址由用户自己设置,但是不能与网络中其他设备MAC地址重复
	netif->hwaddr[0]=lwipdev.mac[0]; 
	netif->hwaddr[1]=lwipdev.mac[1]; 
	netif->hwaddr[2]=lwipdev.mac[2];
	netif->hwaddr[3]=lwipdev.mac[3];
	netif->hwaddr[4]=lwipdev.mac[4];
	netif->hwaddr[5]=lwipdev.mac[5];
	netif->mtu=1500; //最大允许传输单元,允许该网卡广播和ARP功能
	netif->flags = NETIF_FLAG_BROADCAST|NETIF_FLAG_ETHARP|NETIF_FLAG_LINK_UP;		
	return ERR_OK;
} 
//用于发送数据包的最底层函数(lwip通过netif->linkoutput指向该函数)
//netif:网卡结构体指针
//p:pbuf数据结构体指针
//返回值:ERR_OK,发送正常
//       ERR_MEM,发送失败
static err_t low_level_output(struct netif *netif,struct pbuf *p)
{
	struct pbuf *q;
	int l=0;
	u8 *buffer;
	buffer=mymalloc(SRAMIN,p->tot_len);	//申请内存
	if(buffer==NULL)printf("发送数据缓冲区内存申请失败\r\n");
	for(q=p;q!=NULL;q=q->next) 
	{
		memcpy((u8_t*)&buffer[l], q->payload, q->len);
		l=l+q->len;
	} 
	ENC28J60_Packet_Send(p->tot_len,buffer);
	myfree(SRAMIN,buffer);				//释放内存
	return ERR_OK;
}  
///用于接收数据包的最底层函数
//neitif:网卡结构体指针
//返回值:pbuf数据结构体指针
static struct pbuf * low_level_input(struct netif *netif)
{  
	struct pbuf *p,*q;
	u32 len;
	u8 *buffer;
	int l=0;
	
	p=NULL;
	buffer=mymalloc(SRAMIN,1600);			//申请内存
	if(buffer!=NULL)len=ENC28J60_Packet_Receive(MAX_FRAMELEN,buffer);	//接收数据
	else 
	{
		printf("接收数据缓冲区内存申请失败\r\n");
		return p;
	}
	p=pbuf_alloc(PBUF_RAW,len,PBUF_POOL);	//pbufs内存池分配pbuf
	if(p!=NULL)
	{
		for(q=p;q!=NULL;q=q->next)
		{
			memcpy((u8_t*)q->payload,(u8_t*)&buffer[l], q->len);
			l=l+q->len;
		}    
	}
	myfree(SRAMIN,buffer);					//释放内存
	return p;
}
//网卡接收数据(lwip直接调用)
//netif:网卡结构体指针
//返回值:ERR_OK,发送正常
//       ERR_MEM,发送失败
err_t ethernetif_input(struct netif *netif)
{
	err_t err;
	struct pbuf *p;
	p=low_level_input(netif);   //调用low_level_input函数接收数据
	if(p==NULL) return ERR_MEM;
	err=netif->input(p, netif); //调用netif结构体中的input字段(一个函数)来处理数据包
	if(err!=ERR_OK)
	{
		LWIP_DEBUGF(NETIF_DEBUG,("ethernetif_input: IP input error\n"));
		pbuf_free(p);
		p = NULL;
	} 
	return err;
} 
//使用low_level_init()函数来初始化网络
//netif:网卡结构体指针
//返回值:ERR_OK,正常
//       其他,失败
err_t ethernetif_init(struct netif *netif)
{
	LWIP_ASSERT("netif!=NULL",(netif!=NULL));
#if LWIP_NETIF_HOSTNAME			//LWIP_NETIF_HOSTNAME 
	netif->hostname="lwip";  	//初始化名称
#endif 
	netif->name[0]=IFNAME0; 	//初始化变量netif的name字段
	netif->name[1]=IFNAME1; 	//在文件外定义这里不用关心具体值
	netif->output=etharp_output;//IP层发送数据包函数
	netif->linkoutput=low_level_output;//ARP模块发送数据包函数
	low_level_init(netif); 		//底层硬件初始化函数
	return ERR_OK;
}














