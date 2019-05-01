/**
  ******************************************************************************
  * @file    lwipopts.h
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    31-July-2013
  * @brief   lwIP Options Configuration.
  *          This file is based on Utilities\lwip_v1.4.1\src\include\lwip\opt.h
  *          and contains the lwIP configuration for the STM32F4x7 demonstration.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

#ifndef __LWIPOPTS_H__
#define __LWIPOPTS_H__

/**
 * SYS_LIGHTWEIGHT_PROT==1: if you want inter-task protection for certain
 * critical regions during buffer allocation, deallocation and memory
 * allocation and deallocation.
 */
#define SYS_LIGHTWEIGHT_PROT    1

#define ETHARP_TRUST_IP_MAC     0
#define IP_REASSEMBLY           0   /*  UDP IP包重组 */ /* UDP 最大发送65507 (~64k)一次 */
#define IP_FRAG                 0   /*  UDP IP 分包 */
#define IP_REASS_MAXAGE         3   /* 最大的分包数 */
#define ARP_QUEUEING            0   /* 1：会创建ARP链表 */
#define TCP_LISTEN_BACKLOG      1

/**
 * NO_SYS==1: Provides VERY minimal functionality. Otherwise,
 * use lwIP facilities.
 */
#define NO_SYS                  0

	/* ---------- Memory options 设置发送数据缓存的大小 ---------- */
	/* MEM_ALIGNMENT: should be set to the alignment of the CPU for which
	   lwIP is compiled. 4 byte alignment -> define MEM_ALIGNMENT to 4, 2
	   byte alignment -> define MEM_ALIGNMENT to 2. */
	#define MEM_ALIGNMENT           4

	/**************************  Heap Memory ********************************/
	/* MEM_SIZE: the size of the heap memory. If the application will send
	a lot of data that needs to be copied, this should be set high.
	* heap内存的大小,如果在应用中有大量数据发送的话这个值最好设置大一点 */
	#define MEM_SIZE              (30*1024)    /*PBUF_RAM*/
	/**************************  Heap Memory End ****************************/

	/*************************** Pool Memory *********************************/
	/* MEMP_NUM_PBUF: the number of memp struct pbufs. If the application
	   sends a lot of data out of ROM (or other static memory), this
	   should be set high. */
	#define MEMP_NUM_PBUF           100   /* PBUF_ROM */
	/* raw pcb */
	#define MEMP_NUM_RAW_PCB        4
	/* MEMP_NUM_UDP_PCB: the number of UDP protocol control blocks. One
	   per active UDP "connection". */
	#define MEMP_NUM_UDP_PCB        6  /* 同时创建的UDP的连接个数 */
	/* MEMP_NUM_TCP_PCB: the number of simulatenously active TCP
	   connections. */
	#define MEMP_NUM_TCP_PCB        24 /* 同时建立激活的TCP数量 */
	/* MEMP_NUM_TCP_PCB_LISTEN: the number of listening TCP
	   connections. */
	#define MEMP_NUM_TCP_PCB_LISTEN 5  /* 能够监听的TCP连接数量 */
	/* MEMP_NUM_TCP_SEG: the number of simultaneously queued TCP
	   segments. */
	#define MEMP_NUM_TCP_SEG        20 /* 最多同时在队列中的TCP段数量 */
	/* MEMP_NUM_SYS_TIMEOUT: the number of simulateously active
	   timeouts. */
	#define MEMP_NUM_SYS_TIMEOUT    10 /* 能够同时激活的timeout个数 */
	/**
	 * MEMP_NUM_NETCONN: the number of struct netconns.
	 * (only needed if you use the sequential API, like api_lib.c)
	 * 设置LWIP最大的连接socket个数
	 */
	#define MEMP_NUM_NETCONN        30
	/**
	 * MEMP_NUM_NETBUF: the number of struct netbufs.
	 * (only needed if you use the sequential API, like api_lib.c)
	 */
	/* 使用socket时，该值设置的太小，可能导致接收数据时，分配内存失败，从而不能同时为几个连接的数据收发服务 */
	/* 收数据的时候会分配 netbuf 例如在  */
	#define MEMP_NUM_NETBUF         10
	/**
	 * MEMP_NUM_TCPIP_MSG_INPKT: the number of struct tcpip_msg, which are used
	 * for incoming packets. 
	 * (only needed if you use tcpip.c)
	 */  /* 如果数据包来的特别多的话，增大这个值 */
	#define MEMP_NUM_TCPIP_MSG_INPKT        100 /* tcpip_input得到这个地址，通过邮箱发送给tcpip_thread， 消息处理完成后释放，   */  

	/* ---------- Pbuf options  设置接受数据缓存的大小  ---------- */
	/* PBUF_POOL_SIZE: the number of buffers in the pbuf pool. */
	#define PBUF_POOL_SIZE          100

	/* PBUF_POOL_BUFSIZE: the size of each pbuf in the pbuf pool. */
	#define PBUF_POOL_BUFSIZE       512  /* 接收的每个数据包分配的pbuf的大小 */

	/**************************  Pool Memory End **********************/


	/* ---------- TCP options ---------- */
	#define LWIP_TCP                1
	#define TCP_TTL                 255

	/* Controls if TCP should queue segments that arrive out of
	   order. Define to 0 if your device is low on memory. */
	#define TCP_QUEUE_OOSEQ         0  /*当TCP的数据段超出队列时的控制位,当设备的内存过小的时候此项应为0*/

	/* TCP Maximum segment size. */   /* 最大TCP分段 */
	#define TCP_MSS                 (1500 - 40)   /* TCP_MSS = (Ethernet MTU - IP header size - TCP header size) */

	/* TCP sender buffer space (bytes). */  /* TCP发送缓冲区大小(bytes) */
	#define TCP_SND_BUF             (5*TCP_MSS)

	/*  TCP_SND_QUEUELEN: TCP sender buffer space (pbufs). This must be at least
	  as much as (2 * TCP_SND_BUF/TCP_MSS) for things to work. */ /* TCP发送缓冲区大小(pbuf) */
	#define TCP_SND_QUEUELEN        (4* TCP_SND_BUF/TCP_MSS)

	/* TCP receive window. */  /* TCP接收窗口大小 */
	#define TCP_WND                 (10*TCP_MSS)

/* ---------- ICMP options ---------- */
#define LWIP_ICMP                       1


/* ---------- DHCP options ---------- */
/* Define LWIP_DHCP to 1 if you want DHCP configuration of
   interfaces. DHCP is not implemented in lwIP 0.5.1, however, so
   turning this on does currently not work. */
#define LWIP_DHCP               0

/* ---------- UDP options ---------- */
#define LWIP_UDP                1
#define UDP_TTL                 255


/* ---------- Statistics options ---------- */
#define LWIP_PROVIDE_ERRNO    1


/*
   --------------------------------------
   ---------- Checksum options ----------
   --------------------------------------
*/

/*
The STM32F4x7 allows computing and verifying the IP, UDP, TCP and ICMP checksums by hardware:
 - To use this feature let the following define uncommented.
 - To disable it and process by CPU comment the  the checksum.
*/
#define CHECKSUM_BY_HARDWARE

#ifdef CHECKSUM_BY_HARDWARE
/* CHECKSUM_GEN_IP==0: Generate checksums by hardware for outgoing IP packets.*/
#define CHECKSUM_GEN_IP                 0
/* CHECKSUM_GEN_UDP==0: Generate checksums by hardware for outgoing UDP packets.*/
#define CHECKSUM_GEN_UDP                0
/* CHECKSUM_GEN_TCP==0: Generate checksums by hardware for outgoing TCP packets.*/
#define CHECKSUM_GEN_TCP                0
/* CHECKSUM_CHECK_IP==0: Check checksums by hardware for incoming IP packets.*/
#define CHECKSUM_CHECK_IP               0
/* CHECKSUM_CHECK_UDP==0: Check checksums by hardware for incoming UDP packets.*/
#define CHECKSUM_CHECK_UDP              0
/* CHECKSUM_CHECK_TCP==0: Check checksums by hardware for incoming TCP packets.*/
#define CHECKSUM_CHECK_TCP              0
/* CHECKSUM_CHECK_ICMP==0: Check checksums by hardware for incoming ICMP packets.*/
#define CHECKSUM_GEN_ICMP               0
#else
/* CHECKSUM_GEN_IP==1: Generate checksums in software for outgoing IP packets.*/
#define CHECKSUM_GEN_IP                 1
/* CHECKSUM_GEN_UDP==1: Generate checksums in software for outgoing UDP packets.*/
#define CHECKSUM_GEN_UDP                1
/* CHECKSUM_GEN_TCP==1: Generate checksums in software for outgoing TCP packets.*/
#define CHECKSUM_GEN_TCP                1
/* CHECKSUM_CHECK_IP==1: Check checksums in software for incoming IP packets.*/
#define CHECKSUM_CHECK_IP               1
/* CHECKSUM_CHECK_UDP==1: Check checksums in software for incoming UDP packets.*/
#define CHECKSUM_CHECK_UDP              1
/* CHECKSUM_CHECK_TCP==1: Check checksums in software for incoming TCP packets.*/
#define CHECKSUM_CHECK_TCP              1
/* CHECKSUM_CHECK_ICMP==1: Check checksums by hardware for incoming ICMP packets.*/
#define CHECKSUM_GEN_ICMP               1
#endif


/*
   ----------------------------------------------
   ---------- Sequential layer options ----------
   ----------------------------------------------
*/
/**
 * LWIP_NETCONN==1: Enable Netconn API (require to use api_lib.c)
 */
#define LWIP_NETCONN                    1

/*
   ------------------------------------
   ---------- Socket options ----------
   ------------------------------------
*/
/**
 * LWIP_SOCKET==1: Enable Socket API (require to use sockets.c)
 */
#define LWIP_SOCKET                     1



/**
 * LWIP_SO_RCVTIMEO==1: Enable receive timeout for sockets/netconns and
 * SO_RCVTIMEO processing.
 */
/* 通过定义LWIP_SO_RCVTIMEO使能netconn结构体中recv_timeout,使用recv_timeout可以避免阻塞线程 */
#define LWIP_SO_RCVTIMEO                1

/*
   -----------------------------------
   ---------- DEBUG options ----------
   -----------------------------------
*/

#define LWIP_DEBUG                      0
#define ERRNO                         /* 错误码使能 */

/*
   ---------------------------------
   ---------- OS options ----------
   ---------------------------------
*/

#define TCPIP_THREAD_NAME              "TCP/IP"
#define TCPIP_THREAD_STACKSIZE          1000

#define TCPIP_MBOX_SIZE                 128  /* mbox: 所有的socket bind listen input 都会通过这个mbox, 传给tcpip_thread 待处理 ， 一个邮箱4个byte, 256byte */
#define DEFAULT_UDP_RECVMBOX_SIZE       24   /*  */
#define DEFAULT_TCP_RECVMBOX_SIZE       23
#define DEFAULT_ACCEPTMBOX_SIZE         22

#define DEFAULT_THREAD_STACKSIZE        500
#define TCPIP_THREAD_PRIO               (configMAX_PRIORITIES - 4)
#define LWIP_COMPAT_MUTEX               1


/*********************    LWIP 调试相关   ******************************/


/* 用于统计MEMP的使用状况 */
#define MEMP_STATS                      1
#define LWIP_STATS_DISPLAY              1 // 使用MEMP_STATS_DISPLAY(i)来打印  MEM_STATS_DISPLAY() 打印MEM使用情况
#define SYS_STATS_COM                   1 // ZWX 添加的用于统计邮箱使用情况  STATS_DISP_COM()  用于打印

/**
 * MEMP_DEBUG: Enable debugging in memp.c.
 */
#define MEM_DEBUG                       LWIP_DBG_ON  /* 可以把MEM debug 打开，调试 */
#define MEMP_DEBUG                      LWIP_DBG_ON  /* 可以把MEMP debug 打开，调试 */
#define LWIP_DBG_MIN_LEVEL              LWIP_DBG_LEVEL_ALL  /* 这个调试等级打开到 serious  */

#define TCP_QLEN_DEBUG				    LWIP_DBG_OFF   /*  调试lwip queue len */


#endif /* __LWIPOPTS_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
