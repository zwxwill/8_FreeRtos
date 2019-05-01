/**
  ******************************************************************************
  * @file    test_rawtcpclient.c
  * @author  zwx
  * @version V1.8.0
  * @date    2019-01-29
  * @brief   
  ******************************************************************************
  */  

#include "test_rawtcpclient.h"

#include "FreeRTOS.h"
#include "task.h"

#include "lwip/tcp.h"
#include "lwip/sockets.h"

#include <stdio.h>

/* Exported variables --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private struct  -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

#define RAW_SEND_LEN	(500)
static u8_t RawSendBuf[RAW_SEND_LEN] = {0};

struct myTcp
{
	struct tcp_pcb *conn;
	u8_t flg;
};

/**
  * @brief  
  * @param  None
  * @retval None
  */
static void raw_tcp_close(struct myTcp *raw)
{
	err_t res;
	
    /* Bring down TCP connection if not already done */
    if(raw->conn != NULL)
    {     
        tcp_recv(raw->conn, NULL);
        tcp_err(raw->conn,  NULL);
        tcp_sent(raw->conn, NULL);
        res = tcp_close(raw->conn);
        if(res != ERR_OK)
        {
            tcp_abort(raw->conn);
            printf("mqtt_close: Close err=%s\n", lwip_strerr(res));
        }
        raw->conn = NULL;
    }
}

/**
  * @brief  
  * @param  None
  * @retval None
  */
static err_t raw_tcp_recv_cb(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err)
{
	struct myTcp *raw = (struct myTcp *)arg;
	printf("recv callback\r\n");
	
	if(p == NULL)
	{
		raw_tcp_close(arg);
	}
	else
	{
        if(err != ERR_OK)
        {
            printf("raw_tcp_recv_cb: Recv err=%d\n", err);
            pbuf_free(p);
            return err;
        }		
		/* Tell remote that data has been received */
		tcp_recved(pcb, p->tot_len);
		pbuf_free(p);
	}
	
	return 0;
}

static err_t raw_tcp_sent_cb(void *arg, struct tcp_pcb *tpcb, u16_t len)
{
	static u8_t i = 0;
	i++;
	printf("----send callback %d \r\n", i);
	return 0;
}

/**
  * @brief  
  * @param  None
  * @retval None
  */
static err_t raw_tcp_connect_cb(void *arg, struct tcp_pcb *tpcb, err_t err)
{
	struct myTcp *raw = (struct myTcp *)arg;
	
	if(err != ERR_OK)
	{
        printf("tcp_connect_cb: TCP connect error %d\n", err);
        return err;	
	}
	else
	{
		raw->flg = 1;
		tcp_recv(tpcb, raw_tcp_recv_cb);
		tcp_sent(tpcb, raw_tcp_sent_cb);	
		tcp_poll(tpcb, NULL, 0);
		printf("con server success\r\n");	
	}
	return 0;
}

static void raw_tcp_err_cb(void *arg, err_t err)
{
	struct myTcp *raw = (struct myTcp *)arg;
	
	printf("raw_tcp_err_cb: TCP error callback: error %d, arg: %p\n", err, arg);
    /* Set conn to null before calling close as pcb is already deallocated*/
    raw->conn = 0;
}

static void raw_output_send(struct tcp_pcb *tpcb)
{
	int lenToSend = 0;
	int lenInBuf = 0;
	err_t err;

	//vTaskSuspendAll(); /* 特别注意：tcp_write和tcp_receive不能同时调用，在不同的线程中要加保护 */
	vPortEnterCritical();
	lenInBuf = tcp_sndbuf(tpcb);
	if(lenInBuf == 0)
	{
		return;
	}
	if(lenInBuf < RAW_SEND_LEN)
	{
		lenToSend = lenInBuf;
	}
	else
	{
		lenToSend = RAW_SEND_LEN;
	}
	
	err = tcp_write(tpcb, RawSendBuf, lenToSend, TCP_WRITE_FLAG_COPY);
	if(err == ERR_OK)
	{
		tcp_output(tpcb);
	}
	else
	{
		printf("tcp_write: Send failed with err %d (\"%s\")\n", err, lwip_strerr(err));
	}		
	printf("buf:%d send:%d final:%d\r\n", lenInBuf, RAW_SEND_LEN, lenToSend);
	//xTaskResumeAll();  /* 调度锁打开 */
	vPortExitCritical();	
}

/**
  * @brief  
  * @param  None
  * @retval None
  */
void vTaskTestRawTcpClient(void *pvParameters)
{
	struct myTcp Raw;
	err_t err;
	u32_t s_addr;
	u8_t arg;
	int i = 0;

	Raw.flg = 0;
	Raw.conn = tcp_new();
    if(Raw.conn == NULL)
    {
        printf("tcp_new ERR_MEM\r\n");
		return;
    }
	
    /* Set arg pointer for callbacks */
	arg = 0;
    tcp_arg(Raw.conn, &Raw);	/* 设置回调函数的参数为arg */
	
    /* Any local address, pick random local port number */
    err = tcp_bind(Raw.conn, IP_ADDR_ANY, 0);	
    if(err != ERR_OK)
    {
        printf("mconnect: Error binding to local ip/port, %d\n", err);
		return;
    }
	
    /* Connect to server */
	s_addr = inet_addr("192.168.1.133");
    err = tcp_connect(Raw.conn, (ip_addr_t *)&s_addr, 36584, raw_tcp_connect_cb);
    if(err != ERR_OK)
    {
        printf("tcp_connect: Error connecting to remote ip/port, %d\n", err);
        return;
    }	
	
    /* Set error callback */
    tcp_err(Raw.conn, raw_tcp_err_cb);	
	
    while(Raw.flg == 0)
    {
        vTaskDelay(1000);
    }
	
	for(i=0; i<RAW_SEND_LEN; i++)
	{
		RawSendBuf[i] = i%128;
	}
	vTaskDelay(1000);
	
	while(1)
	{
		raw_output_send(Raw.conn);   /* 链路成环 会引起dup ack */
		
		vTaskDelay(1000);
	}
}

/****************** 北京龙鼎源科技 http://www.bdrl.com.cn/ ********************/


