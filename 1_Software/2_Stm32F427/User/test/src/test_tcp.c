/**
  ******************************************************************************
  * @file    test_tcp.c
  * @author  zwx
  * @version V1.8.0
  * @date    2019-01-29
  * @brief   
  ******************************************************************************
  */  

#include "test_tcp.h"
#include "lwip/sockets.h"
#include "srv_netconf.h"

#include "FreeRTOS.h"
#include "task.h"

/* Exported variables --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private struct  -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
#define TEST_DATA_LEN				128U
static u8 SocTCPRecv[TEST_DATA_LEN] = {0};
static u8 SocTCPClientSend[] = "Hello, This is a Socke Tcp Client Test ";
static u8 SocTCPServerSend[] = "Hello, This is a Socke Tcp Client Test ";



/**
  * @brief  
  * @param  None
  * @retval None
  */
void test_tcp_client(void)
{
	int i = 0;
	int sockfd=0;
	struct sockaddr_in servaddr;
	struct sockaddr_in clientaddr;
	socklen_t len = sizeof(clientaddr);
	int recvn = 0;
	int status = 0;
	uint32_t StartTime, EndTime, Elapsed;

	/* 创建socket */
	sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(sockfd < 0)
	{
		printf("Tcp Client : socket fail\r\n");
	}	
	else
	{
		printf("Tcp Client : socket ok\r\n");
	}

	/* 设置要连接的服务器端的ip和port信息 */
	servaddr.sin_addr.s_addr = inet_addr("192.168.1.139");
	servaddr.sin_family = AF_INET;
	servaddr.sin_len = sizeof(servaddr);
	servaddr.sin_port = htons(8000);		
Srv_SetNetNonBlock(sockfd);
	while(1)
	{	
struct timeval timeo;
timeo.tv_sec = 5;
timeo.tv_usec = 0;
setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeo, sizeof(timeo));		
		
		
//		SetPSocketBlock(sockfd);
		/* 连接到服务器 */
		StartTime = xTaskGetTickCount();
		status = connect(sockfd, (struct sockaddr *)&servaddr, sizeof(struct sockaddr));  /* 阻塞情况 超时时间为18s */
		if (status == -1)
		{
			EndTime = xTaskGetTickCount();
			Elapsed = (EndTime - StartTime) * portTICK_RATE_MS;	
			printf("Tcp connect timeout = %dms\r\n", Elapsed);
			printf("Tcp Client : connetc failed\r\n");
	//		close(sockfd);
			continue;
		}
		else
		{
			printf("Tcp Client : connetc ok\r\n");
		}
			
		
		while(1)
		{
			recvn = recv(sockfd, SocTCPRecv, TEST_DATA_LEN, 0);
			if(recvn == -1)
			{
				printf("TCP Client : recv Error State = %d\r\n", recvn);
				break;					
			}
			else if(recvn == 0)
			{
				printf("TCP Client : recv remote close Error\r\n");
				close(sockfd);
				break;
			}
			else
			{
				printf("TCP Client Recv : ");
				for(i=0; i<recvn; i++)
				{
					printf("%d", SocTCPRecv[i]);
				}
				printf("\r\n");
				
				send(sockfd, SocTCPClientSend, sizeof(SocTCPClientSend), 0);
			}		
		}	
	}

}

/**
  * @brief  
  * @param  None
  * @retval None
  */
void test_tcp_server(void)
{
	int i = 0;
	int sockfd=0, confd=0;
	struct sockaddr_in servaddr;
	struct sockaddr_in clientaddr;
	socklen_t len = sizeof(clientaddr);
	int recvn = 0;
	
	/* 创建socket */
	sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(sockfd < 0)
	{
		printf("Tcp Server : socket fail\r\n");
	}	

	/* 设置服务器端的ip和port信息 */
	servaddr.sin_addr.s_addr = inet_addr("192.168.1.61");
	servaddr.sin_family = AF_INET;
	servaddr.sin_len = sizeof(servaddr);
	servaddr.sin_port = htons(8000);	
	
	/* 绑定 */
	if(bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
	{
		printf("TCP Server : bind fail\r\n");
		close(sockfd);	
	}	
	
	/* 监听 */
	if(listen(sockfd, 1) == -1)
	{
		printf("TCP Server : listen fail\r\n");
		close(sockfd);		
	}	
	
	while(1)
	{
		/* accept 阻塞直到有客户端连接 */
		confd = accept(sockfd, (struct sockaddr *)&clientaddr, &len);
		if(confd == -1)
		{
			printf("TCP Server : accept fail\r\n");
		}	
		else
		{
			printf("Cliet Addr: %s, port 0x%x\r\n", inet_ntoa(clientaddr.sin_addr), clientaddr.sin_port);
		}
		
		while(1)
		{
			recvn = recv(confd, SocTCPRecv, TEST_DATA_LEN, 0);
			if(recvn == -1)
			{
				printf("TCP Server : recv Error State = %d\r\n", recvn);
				close(confd);
				break;						
			}
			else if(recvn == 0)
			{
				printf("TCP Server : recv remote close Error\r\n");
				close(confd);
			}
			else
			{
				printf("TCP Server Recv : ");
				for(i=0; i<recvn; i++)
				{
					printf("%d", SocTCPRecv[i]);
				}
				printf("\r\n");
				
				send(sockfd, SocTCPServerSend, sizeof(SocTCPServerSend), 0);
			}
		}
	}
}

/****************** 北京龙鼎源科技 http://www.bdrl.com.cn/ ********************/

