/**
  ******************************************************************************
  * @file    test_udp.c
  * @author  zwx
  * @version V1.8.0
  * @date    2019-01-29
  * @brief   
  ******************************************************************************
  */  

#include "test_udp.h"
#include "lwip/sockets.h"
#include "srv_netconf.h"
#include <string.h>

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


/**
  * @brief  
  * @param  None
  * @retval None
  */
void test_udp_server(void)
{
	int socket_fd;
	struct sockaddr_in sa,ra;
	int recv_len, snd_len; 
	char data_buffer[80]; 
	int buf_len = 0;
	socklen_t sock_len = sizeof(struct sockaddr_in);
	
	/* Creates an UDP socket (SOCK_DGRAM) with Internet Protocol Family (PF_INET).
	 * Protocol family and Address family related. For example PF_INET Protocol Family and AF_INET family are coupled.
	*/
	buf_len = sizeof(data_buffer);
	
	socket_fd = socket(PF_INET, SOCK_DGRAM, 0);

	if ( socket_fd < 0 )
	{
		printf("socket call failed");
		return;
	}

	memset(&sa, 0, sizeof(struct sockaddr_in));
	ra.sin_family = AF_INET;
	ra.sin_addr.s_addr = inet_addr("192.168.1.45");
	//ra.sin_addr.s_addr = htonl(INADDR_ANY);
	ra.sin_port = htons(6001);


	/* Bind the UDP socket to the port RECEIVER_PORT_NUM and to the current
	* machines IP address (Its defined by RECEIVER_PORT_NUM).
	* Once bind is successful for UDP sockets application can operate
	* on the socket descriptor for sending or receiving data.
	*/
	if (bind(socket_fd, (struct sockaddr *)&ra, sizeof(struct sockaddr_in)) == -1)
	{
		printf("Bind to Port Number %d ,IP address %s failed\n",6001,"192.168.1.45");
		close(socket_fd);
		return;
	}
	/* RECEIVER_PORT_NUM is port on which Server waits for data to
	* come in. It copies the received data into receive buffer and
	* prints the received data as string. If no data is available it 
	* blocks.recv calls typically return any availbale data on the socket instead of waiting for the entire data to come.
	*/
	while(1)
	{
		//recv_data = recv(socket_fd,data_buffer,sizeof(data_buffer),0);
		recv_len = recvfrom(socket_fd, data_buffer, buf_len, 0, (struct sockaddr*)&sa, &sock_len);
		if(recv_len > 0)
		{
			data_buffer[recv_len] = '\0';
			printf("%s\n", data_buffer);
		} 
		// send back
		snd_len = sendto(socket_fd, (char *)data_buffer, recv_len, 0, (struct sockaddr*)&sa, sock_len);
		if(snd_len != recv_len)
		{
			printf("sendto error\n");
		}
	}
	printf("close udp server\r\n");
	close(socket_fd);	
}

void test_udp_clinet(void)
{
	int socket_fd;
	struct sockaddr_in sa;
	struct sockaddr_in ra;
	int sent_data; 
	char data_buffer[20] = {"hello world\r\n"}; 
	int buf_len = 0;
	int recv_len, snd_len; 
	socklen_t sock_len = sizeof(struct sockaddr_in);
	
	buf_len = sizeof(data_buffer);
	/* Creates an UDP socket (SOCK_DGRAM) with Internet Protocol Family (PF_INET).
	 * Protocol family and Address family related. For example PF_INET Protocol Family and AF_INET family are coupled.
	*/
	socket_fd = socket(PF_INET, SOCK_DGRAM, 0);

	if ( socket_fd < 0 )
	{
		printf("socket call failed");
		return;
	}

	memset(&sa, 0, sizeof(struct sockaddr_in));
	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = inet_addr("192.168.1.45");
//  sa.sin_addr.s_addr = htonl(INADDR_ANY);
	sa.sin_port = htons(6001);


	/* Bind the TCP socket to the port SENDER_PORT_NUM and to the current
	* machines IP address (Its defined by SENDER_IP_ADDR). 
	* Once bind is successful for UDP sockets application can operate
	* on the socket descriptor for sending or receiving data.
	*/
	if (bind(socket_fd, (struct sockaddr *)&sa, sizeof(struct sockaddr_in)) == -1)
	{
		printf("Bind to Port Number %d ,IP address %s failed\n",6001,"192.168.1.45");
		close(socket_fd);
		return;
	}

	memset(&ra, 0, sizeof(struct sockaddr_in));
	ra.sin_family = AF_INET;
	ra.sin_addr.s_addr = inet_addr("192.168.1.133");
	ra.sin_port = htons(6002);

	while(1)
	{
		snd_len = sendto(socket_fd, data_buffer, buf_len, 0, (struct sockaddr*)&ra, sock_len);
		if(snd_len != buf_len)
		{
			printf("send failed\n");
			close(socket_fd);
			return;
		} 	
		
		recv_len = recvfrom(socket_fd, data_buffer, buf_len, 0, (struct sockaddr*)&sa, &sock_len);
		if(recv_len > 0)
		{
			data_buffer[recv_len] = '\0';
			printf("%s\n", data_buffer);
		} 		
	}

	close(socket_fd);
}
  
/**
  * @brief  
  * @param  None
  * @retval None
  */
void vTaskTestUdpServer(void *pvParameters)
{
	test_udp_server();
	printf("error return TestUdpServer\r\n");
	
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
void vTaskTestUdpClent(void *pvParameters)
{
	test_udp_clinet();
	printf("error return TestUdpClient\r\n");
	
    while(1)
    {
        vTaskDelay(1000);
    }
}

/****************** 北京龙鼎源科技 http://www.bdrl.com.cn/ ********************/

