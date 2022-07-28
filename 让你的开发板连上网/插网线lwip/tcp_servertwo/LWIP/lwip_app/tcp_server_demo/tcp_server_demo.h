#ifndef __TCP_SERVER_DEMO_H
#define __TCP_SERVER_DEMO_H
#include "sys.h"
#include "lwip/debug.h"
#include "lwip/stats.h"
#include "lwip/tcp.h"
#include "lwip/memp.h"
#include "lwip/mem.h"
#include "lwip_comm.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK ENC28J60模块
//TCP Server 测试代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2015/4/30
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//*******************************************************************************
//修改信息
//无
////////////////////////////////////////////////////////////////////////////////// 	   
 
#define TCP_SERVER_RX_BUFSIZE	2000	//定义tcp server最大接收数据长度
#define TCP_SERVER_PORT			8088	//定义tcp server的端口
 
//tcp服务器连接状态
enum tcp_server_states
{
	ES_TCPSERVER_NONE = 0,		//没有连接
	ES_TCPSERVER_ACCEPTED,		//有客户端连接上了 
	ES_TCPSERVER_CLOSING,		//即将关闭连接
}; 
//LWIP回调函数使用的结构体
struct tcp_server_struct
{
	u8 state;               //当前连接状
	struct tcp_pcb *pcb;    //指向当前的pcb
	struct pbuf *p;         //指向接收/或传输的pbuf
}; 

void tcp_server_test(void);//TCP Server测试函数
err_t tcp_server_accept(void *arg,struct tcp_pcb *newpcb,err_t err);
err_t tcp_server_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
void tcp_server_error(void *arg,err_t err);
err_t tcp_server_poll(void *arg, struct tcp_pcb *tpcb);
err_t tcp_server_sent(void *arg, struct tcp_pcb *tpcb, u16_t len);
void tcp_server_senddata(struct tcp_pcb *tpcb, struct tcp_server_struct *es);
void tcp_server_connection_close(struct tcp_pcb *tpcb, struct tcp_server_struct *es);
void tcp_server_remove_timewait(void);
#endif 
