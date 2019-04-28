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
#include <string.h>
#include "heap_6.h"

#include "FreeRTOS.h"
#include "task.h"
#include "bsp.h"

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
static u8 *pSocTcpClientBuf; /* 测试客户端发送大量数据的情况 */

#define SOCKET_DISCONNECT				0
#define SOCKET_CONNECTED				1
#define SOCKET_CONNECTING				2

static int g_ConState = 0;

/**
  * @brief  
  * @param  None
  * @retval None
  */
void vTaskTestTcpClient(void *pvParameters)
{
	test_tcp_client();
	//test_tcp_client_nob();
	
    while(1)
    {
        vTaskDelay(1000);
    }
}

/**
  * @brief  
  * @param  None
  * @retval None
  */
void vTaskTestTcpServer(void *pvParameters)
{
	test_tcp_server(8100);
	
    while(1)
    {
        vTaskDelay(1000);
    }
}

/**
  * @brief  
  * @param  None
  * @retval None
  */
static void test_socketNonBlock(int sock)
{
    int flags;

    flags = fcntl(sock, F_GETFL, 0);
	flags |= O_NONBLOCK;
    fcntl(sock, F_SETFL, flags);
}

/**
  * @brief  
  * @param  None
  * @retval None
  */
static void test_socketBlock(int sock)
{
    int flags;

    flags = fcntl(sock, F_GETFL, 0);
	flags &= (~O_NONBLOCK);
    fcntl(sock, F_SETFL, flags);	
}

int select_nonb_again(int sockfd, const struct sockaddr *addr, socklen_t addrlen, int blockMs)
{
	static int conTimeoutCnt = 0;
    int flags, n, error;  
    socklen_t len;  
    fd_set rset, wset;  
    struct timeval tval;  
	int status = 0, status1 = 0;
	int ret = 0;

	/* 设置为非阻塞状态 */
    flags = fcntl(sockfd, F_GETFL, 0);  
    fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);

	FD_ZERO(&rset);
	FD_SET(sockfd, &rset);
	wset = rset;
	
	tval.tv_sec = blockMs/1000;
	tval.tv_usec = (blockMs%1000)*1000;		

	status1 = select(sockfd+1, &rset, &wset, NULL, &tval);

	if(status1 == 0) 
	{
		printf("socket connect select timeout %d \n", conTimeoutCnt++);
		g_ConState = SOCKET_CONNECTING;
		ret = FALSE;
	}
	else 
	{
		conTimeoutCnt = 0;
		if (FD_ISSET(sockfd, &rset) || FD_ISSET(sockfd, &wset)) 
		{
			len = sizeof(error);
			if (getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error, &len) < 0)
			{
				printf("socket getsockopt error \n");	
				g_ConState = SOCKET_DISCONNECT;	
				close(sockfd); 
				ret = FALSE;						
			}	
			else
			{
				if (error) 
				{
					printf("socket getsockopt have error %d\n", error);	
					g_ConState = SOCKET_DISCONNECT;	
					close(sockfd); 
					ret = FALSE;					
				}
				else
				{
					printf("socket getsockopt connected ok\n");	
					g_ConState = SOCKET_CONNECTED;						
					ret = TRUE;										
				}
			}
		}
		else
		{
			printf("socket FD_ISSET error \n");	
			g_ConState = SOCKET_DISCONNECT;	
			close(sockfd); 
			ret = FALSE;							
		}							
	}
	
	fcntl(sockfd, F_SETFL, flags);
		
	return ret;
}



/**
  * @brief  
  * @param  None
  * @retval None
  */
int connect_nonb(int sockfd, const struct sockaddr *addr, socklen_t addrlen, int blockMs)  
{  
    int flags, n, error;  
    socklen_t len;  
    fd_set rset, wset;  
    struct timeval tval;  
	int status = 0, status1 = 0;
	int ret = 0;

	/* 设置为非阻塞状态 */
    flags = fcntl(sockfd, F_GETFL, 0);  
    fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);

	FD_ZERO(&rset);
	FD_SET(sockfd, &rset);
	wset = rset;
	
	tval.tv_sec = blockMs/1000;
	tval.tv_usec = (blockMs%1000)*1000;		
	
	error = 0;  
	status = connect(sockfd, (struct sockaddr *) addr, addrlen);
	if(status == 0) /* connect ok */
	{	
		printf("socket connect success. \n");
		g_ConState = SOCKET_CONNECTED;
		ret = TRUE;
	}	
	else
	{
		if(errno == EINPROGRESS)
		{			
			ret = select_nonb_again(sockfd, addr, addrlen, blockMs);	
		}
		else if(errno == EISCONN)
		{
			printf("socket already connected %d \n", errno);	
			g_ConState = SOCKET_CONNECTED;				
			ret = TRUE;		
		}
		else
		{
			printf("socket connect error %d \n", errno);
			g_ConState = SOCKET_DISCONNECT;	
			close(sockfd); 
			ret = FALSE;			
		}
	}	
	
	fcntl(sockfd, F_SETFL, flags);
	
	return ret;
}  

void test_getsockname(int sock)
{
    struct sockaddr_in local_addr;
    int local_addr_len = sizeof(struct sockaddr_in);

    getsockname(sock, (struct sockaddr*) &local_addr, (socklen_t *)&local_addr_len);

	printf("getsockname : addr: %s, port: %d\r\n", inet_ntoa(local_addr.sin_addr), (int)ntohs(local_addr.sin_port));
}


/**
  * @brief  
  * @param  None
  * @retval None
  */
void test_tcp_client_nob(void)
{
	int i = 0;
	int sockfd=0;
	struct sockaddr_in servaddr;
	int recvn = 0;
	int status = 0;
	int sendn = 0;
	int flags = 0;
	int retry = 0;
	
	while(1)
	{
		/* 创建socket */
		sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
		if(sockfd < 0)
		{
			printf("Tcp Client : socket fail\r\n");
			continue;
		}	
		else
		{
			printf("Tcp Client : socket ok\r\n");
		}

		/* 设置要连接的服务器端的ip和port信息 */
		servaddr.sin_addr.s_addr = inet_addr("192.168.1.133");
		servaddr.sin_family = AF_INET;
		servaddr.sin_len = sizeof(servaddr);
		servaddr.sin_port = htons(8200);	
		

		status = connect_nonb(sockfd, (struct sockaddr *)&servaddr, sizeof(struct sockaddr), 0);  /* 最后一个参数 超时时间 ms */
		if(status == FALSE)
		{
			for(retry=0; retry<3; retry++)
			{
				if(g_ConState == SOCKET_CONNECTING)
				{
					vTaskDelay(500); /* 延时500ms再次重试 */	
					status =  select_nonb_again(sockfd, (struct sockaddr *)&servaddr, sizeof(struct sockaddr), 0);  /* 最后一个参数 超时时间 ms */	
				}	
				else
				{
					break;
				}
			}
		}

		if(status == FALSE)
		{
			printf("retry----------------------------\r\n");
			g_ConState = SOCKET_DISCONNECT;	
			shutdown(sockfd, SHUT_RDWR); 
			continue;
		}

		while(1)
		{
			recvn = recv(sockfd, SocTCPRecv, TEST_DATA_LEN, 0);
			if(recvn == -1)
			{
				printf("TCP Client : recv Error State = %d\r\n", recvn);
				close(sockfd);
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
void test_tcp_client(void)
{
	int i = 0;
	int sockfd=0;
	struct sockaddr_in servaddr;
	int recvn = 0;
	int status = 0;
	int sendn = 0;
	int flags = 0;
	int bufLen = 20*1024;
	
	pSocTcpClientBuf = pvMyPortMalloc(bufLen, SRAM_EX);
	
	for(i=0; i<bufLen; i++)
	{
		pSocTcpClientBuf[i] = i%128;
	}
	
	while(1)
	{
		/* 创建socket */
		sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
		if(sockfd < 0)
		{
			printf("Tcp Client : socket fail\r\n");
			continue;
		}	
		else
		{
			printf("Tcp Client : socket ok\r\n");
		}

		/* 设置要连接的服务器端的ip和port信息 */
		servaddr.sin_addr.s_addr = inet_addr("192.168.1.133");
		servaddr.sin_family = AF_INET;
		servaddr.sin_len = sizeof(servaddr);
		servaddr.sin_port = htons(8200);	
			
		/* 连接到服务器 */
		status = connect(sockfd, (struct sockaddr *)&servaddr, sizeof(struct sockaddr));  /* 阻塞情况 超时时间为18s */
		if (status != 0)
		{
			printf("Tcp Client : connetc failed %d\r\n", status);
			close(sockfd);
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
				close(sockfd);
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
				
				//send(sockfd, SocTCPClientSend, sizeof(SocTCPClientSend), 0);
				send(sockfd, pSocTcpClientBuf, bufLen, 0);
			}		
		}				
	}
}

/**
  * @brief  
  * @param  None
  * @retval None
  */
void test_tcp_server(int port)
{
	int i = 0;
	int sockfd=0, confd=0;
	struct sockaddr_in servaddr;
	struct sockaddr_in clientaddr;
	socklen_t len = sizeof(clientaddr);
	int recvn = 0;
	int sendn = 0;
	
	while(1)
	{
		/* 创建socket */
		sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if(sockfd < 0)
		{
			printf("Tcp Server : socket fail\r\n");
			continue;
		}	
		else
		{
			printf("Tcp Server : socket ok\r\n");
		}

		/* 设置服务器端的ip和port信息 */
		//servaddr.sin_addr.s_addr = inet_addr("192.168.1.51");
		servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
		servaddr.sin_family = AF_INET;
		servaddr.sin_len = sizeof(servaddr);
		servaddr.sin_port = htons(port);	
		
		/* 绑定 */
		if(bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
		{
			printf("TCP Server : bind fail\r\n");
			close(sockfd);	
			continue;
		}	
		else
		{
			test_getsockname(sockfd);
			printf("TCP Server : bind ok\r\n");
		}		
		
		/* 监听 */
		if(listen(sockfd, 1) == -1)
		{
			printf("TCP Server : listen fail\r\n");
			close(sockfd);		
			continue;
		}	
		else
		{
			printf("TCP Server : listen ok\r\n");
		}
		
		while(1)
		{
			/* accept 阻塞直到有客户端连接 */
			confd = accept(sockfd, (struct sockaddr *)&clientaddr, &len);
			if(confd == -1)
			{
				printf("TCP Server : accept fail\r\n");
				close(sockfd);
				break;
			}	
			else
			{
				printf("Cliet Addr: %s, port %d\r\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
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
					printf("TCP Server : recv remote close\r\n");
					close(confd);
					break;
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
}




/****************** 北京龙鼎源科技 http://www.bdrl.com.cn/ ********************/

