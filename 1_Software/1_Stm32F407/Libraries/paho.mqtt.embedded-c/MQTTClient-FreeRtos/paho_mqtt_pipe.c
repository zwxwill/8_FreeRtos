/**
  ******************************************************************************
  * @file    paho_mqtt_pipe.c
  * @author  zwx
  * @version V1.8.0
  * @date    2019-01-29
  * @brief   
			* MQTT URI farmat:
			* domain mode
			* tcp://iot.eclipse.org:1883
			*
			* ipv4 mode
			* tcp://192.168.10.1:1883
			* ssl://192.168.10.1:1884
			*
			* ipv6 mode
			* tcp://[fe80::20c:29ff:fe9a:a07e]:1883
			* ssl://[fe80::20c:29ff:fe9a:a07e]:1884
  ******************************************************************************
  */  
#include "paho_mqtt_pipe.h"
#include "paho_mqtt.h"
#include "MQTTPacket.h"
#include "elog.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "lwip/sockets.h"
#include "srv_netconf.h"
#include "heap_6.h"
#include "lwip/netdb.h"

#include <string.h>

#include "stm32f4xx.h"

/* Exported variables --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private struct  -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static TaskHandle_t xHandleTaskMqtt = NULL;
/* Private functions ---------------------------------------------------------*/
static int sendPacket(MQTTClient *c, int length);
static int MQTTPacket_readPacket(MQTTClient *c);
/*
 * resolve server address
 * @param server the server sockaddress
 * @param url the input URL address.
 * @param host_addr the buffer pointer to save server host address
 * @param request the pointer to point the request url, for example, /index.html
 *
 * @return 0 on resolve server address OK, others failed
 *
 * URL example:
 * tcp://192.168.10.151:1883
 * tls://192.168.10.151:61614
 * tcp://[fe80::20c:29ff:fe9a:a07e]:1883
 * tls://[fe80::20c:29ff:fe9a:a07e]:61614
 */
int mqtt_resolve_uri(MQTTClient *c, struct addrinfo **res)
{
    int rc = 0;
    int uri_len = 0, host_addr_len = 0, port_len = 0;
    char *ptr;
    char port_str[6] = {0};      /* default port of mqtt(http) */
	
    const char *host_addr = 0;
    char *host_addr_new = NULL;
    const char *uri = c->uri;
    uri_len = strlen(uri);

	/* strip protocol(tcp or ssl) */
    if (strncmp(uri, "tcp://", 6) == 0)
    {
        host_addr = uri + 6;
    }
    else if (strncmp(uri, "ssl://", 6) == 0)
    {
        host_addr = uri + 6;

#ifndef MQTT_USING_TLS
        log_w("Warning: tls uri, please enable mqtt tls support!\n");
        rc = -1;
        goto _exit;
#endif
    }
    else
    {
        rc = -1;
        goto _exit;
    }
	
    /* ipv6 address */
    if (host_addr[0] == '[')
    {
        host_addr += 1;
        ptr = strstr(host_addr, "]");
        if (!ptr)
        {
            rc = -1;
            goto _exit;
        }
        host_addr_len = ptr - host_addr;
        if ((host_addr_len < 1) || (host_addr_len > uri_len))
        {
            rc = -1;
            goto _exit;
        }

        port_len = uri_len - 6 - host_addr_len - 3;
        if (port_len >= 6 || port_len < 1)
        {
            rc = -1;
            goto _exit;
        }

        strncpy(port_str, host_addr + host_addr_len + 2, port_len);
        port_str[port_len] = '\0';
        log_d("ipv6 address port: %s\n", port_str);
    }
    else /* ipv4 or domain. */
    {
        ptr = strstr(host_addr, ":");
        if (!ptr)
        {
            rc = -1;
            goto _exit;
        }
        host_addr_len = ptr - host_addr;
        if ((host_addr_len < 1) || (host_addr_len > uri_len))
        {
            rc = -1;
            goto _exit;
        }

        port_len = uri_len - 6 - host_addr_len - 1;
        if (port_len >= 6 || port_len < 1)
        {
            rc = -1;
            goto _exit;
        }

        strncpy(port_str, host_addr + host_addr_len + 1, port_len);
        port_str[port_len] = '\0';
        log_d("ipv4 address port: %s\n", port_str);
    }	
	
    /* get host addr ok. */
    {
        /* resolve the host name. */
        struct addrinfo hint;
        int ret;
		struct addrinfo *curr;
		char ipstr[16];

        host_addr_new = pvMyPortMalloc(host_addr_len + 1, SRAM_IN);

        if (!host_addr_new)
        {
            rc = -1;
            goto _exit;
        }

        memcpy(host_addr_new, host_addr, host_addr_len);
        host_addr_new[host_addr_len] = '\0';
        log_d("HOST = '%s'\n", host_addr_new);

#ifdef MQTT_USING_TLS
        if (c->tls_session)
        {
            c->tls_session->host = rt_strdup(host_addr_new);
            c->tls_session->port = rt_strdup(port_str);
        }
#endif

        memset(&hint, 0, sizeof(hint));

        ret = lwip_getaddrinfo(host_addr_new, port_str, &hint, res);
        if (ret != 0)
        {
            log_e("getaddrinfo err: %d '%s'\n", ret, host_addr_new);
            rc = -1;
            goto _exit;
        }
		
		for (curr = *res; curr != NULL; curr = curr->ai_next) 
		{
			ipaddr_ntoa_r((const ip_addr_t *)&(((struct sockaddr_in *)(curr->ai_addr))->sin_addr), ipstr, 16);
			log_d("IP:%s\n", ipstr);
		}		
    }	
	
_exit:
    if (host_addr_new != NULL)
    {
        vMyPortFree(host_addr_new, SRAM_IN);
        host_addr_new = NULL;
    }
    return rc;	
}


static int getNextPacketId(MQTTClient *c)
{
    return c->next_packetid = (c->next_packetid == MAX_PACKET_ID) ? 1 : c->next_packetid + 1;
}


/**
 * This function subscribe specified mqtt topic.
 *
 * @param c the pointer of MQTT context structure
 * @param topicFilter topic filter name
 * @param qos requested QoS
 *
 * @return the error code, 0 on subscribe successfully.
 */
static int MQTTSubscribe(MQTTClient *c, const char *topicFilter, enum QoS qos)
{
    int rc = PAHO_FAILURE;
    int len = 0;
    MQTTString topic = MQTTString_initializer;
    topic.cstring = (char *)topicFilter;

    if (!c->isconnected)
        goto _exit;

    len = MQTTSerialize_subscribe(c->buf, c->buf_size, 0, getNextPacketId(c), 1, &topic, (int *)&qos);
    if (len <= 0)
        goto _exit;
    if ((rc = sendPacket(c, len)) != PAHO_SUCCESS) // send the subscribe packet
        goto _exit;             // there was a problem

    {
        int res;
        fd_set readset;
        struct timeval timeout;

        timeout.tv_sec = 5;
        timeout.tv_usec = 0;

        FD_ZERO(&readset);
        FD_SET(c->sock, &readset);

        res = select(c->sock + 1, &readset, NULL, NULL, &timeout);

        if (res <= 0)
        {
            LOG_E("%s wait resp fail, res:%d errno:%d\n", __FUNCTION__, res, errno);
            rc = -1;
            goto _exit;
        }
    }

    rc = MQTTPacket_readPacket(c);
    if (rc < 0)
    {
        LOG_E("MQTTPacket_readPacket MQTTConnect fail");
        goto _exit;
    }

    if (rc == SUBACK)      // wait for suback
    {
        int count = 0, grantedQoS = -1;
        unsigned short mypacketid;

        if (MQTTDeserialize_suback(&mypacketid, 1, &count, &grantedQoS, c->readbuf, c->readbuf_size) == 1)
            rc = grantedQoS; // 0, 1, 2 or 0x80

        if (rc != 0x80)
        {
            rc = 0;
        }
    }
    else
        rc = PAHO_FAILURE;

_exit:
    return rc;
}


/**
  * @brief  
  * @param  None
  * @retval None
  */
static int MQTTConnect(MQTTClient *c)
{
    int rc = -1, len;
    MQTTPacket_connectData *options = &c->condata;

    if (c->isconnected) /* don't send connect packet again if we are already connected */
        goto _exit;

    c->keepAliveInterval = options->keepAliveInterval;

    if ((len = MQTTSerialize_connect(c->buf, c->buf_size, options)) <= 0)
        goto _exit;

    if ((rc = sendPacket(c, len)) != 0)  // send the connect packet
        goto _exit; // there was a problem

    {
        int res;
        fd_set readset;
        struct timeval timeout;

        timeout.tv_sec = 5;
        timeout.tv_usec = 0;

        FD_ZERO(&readset);
        FD_SET(c->sock, &readset);

        res = select(c->sock + 1, &readset, NULL, NULL, &timeout);

        if (res <= 0)
        {
            LOG_E("%s wait resp fail, res:%d errno:%d", __FUNCTION__, res, errno);
            rc = -1;
            goto _exit;
        }
    }

    rc = MQTTPacket_readPacket(c);
    if (rc < 0)
    {
        LOG_E("%s MQTTPacket_readPacket fail", __FUNCTION__);
        goto _exit;
    }

    if (rc == CONNACK)
    {
        unsigned char sessionPresent, connack_rc;

        if (MQTTDeserialize_connack(&sessionPresent, &connack_rc, c->readbuf, c->readbuf_size) == 1)
        {
            rc = connack_rc;
        }
        else
        {
            rc = -1;
        }
    }
    else
        rc = -1;

_exit:
    if (rc == 0)
        c->isconnected = 1;

    return rc;
}

/**
  * @brief  
  * @param  None
  * @retval None
  */
static int MQTTDisconnect(MQTTClient *c)
{
    int rc = PAHO_FAILURE;
    int len = 0;

    len = MQTTSerialize_disconnect(c->buf, c->buf_size);
    if (len > 0)
        rc = sendPacket(c, len);            // send the disconnect packet

    c->isconnected = 0;

    return rc;
}

/**
  * @brief  
  * @param  None
  * @retval None
  */
static int net_connect(MQTTClient *c)
{
    int rc = -1;
    struct addrinfo *addr_res = NULL;

    int timeout = MQTT_SOCKET_TIMEO;	
	c->sock = -1;
    c->next_packetid = 0;
	
#ifdef MQTT_USING_TLS
    if (strncmp(c->uri, "ssl://", 6) == 0)
    {
        if (mqtt_open_tls(c) < 0)
        {
            LOG_E("mqtt_open_tls err!\n");
            return -RT_ERROR;
        }
    }
    else
    {
        c->tls_session = RT_NULL;
    }
#endif	

    rc = mqtt_resolve_uri(c, &addr_res);
    if (rc < 0 || addr_res == NULL)
    {
        log_e("resolve uri err\n");
        goto _exit;
    }
	
#ifdef MQTT_USING_TLS
    if (c->tls_session)
    {
        int tls_ret = 0;

        if ((tls_ret = mbedtls_client_context(c->tls_session)) < 0)
        {
            LOG_E("mbedtls_client_context err return : -0x%x\n", -tls_ret);
            return -RT_ERROR;
        }

        if ((tls_ret = mbedtls_client_connect(c->tls_session)) < 0)
        {
            LOG_E("mbedtls_client_connect err return : -0x%x\n", -tls_ret);
            rc = -RT_ERROR;
            goto _exit;
        }
        LOG_D("tls connect success...\n");

        c->sock = c->tls_session->server_fd.fd;

        /* set recv timeout option */
        setsockopt(c->sock, SOL_SOCKET, SO_RCVTIMEO, (void *) &timeout,
                   sizeof(timeout));
        setsockopt(c->sock, SOL_SOCKET, SO_SNDTIMEO, (void *) &timeout,
                   sizeof(timeout));

        rc = 0;
        goto _exit;
    }
#endif	
	
    if ((c->sock = socket(addr_res->ai_family, SOCK_STREAM, 0)) == -1)
    {
        LOG_E("create socket error!\n");
        goto _exit;
    }

    if ((rc = connect(c->sock, addr_res->ai_addr, addr_res->ai_addrlen)) == -1)
    {
        LOG_E("connect err!\n");
        rc = -2;
        goto _exit;
    }	

_exit:
    if (addr_res)
    {
        freeaddrinfo(addr_res);
        addr_res = NULL;
    }
    return rc;
}

/**
  * @brief  
  * @param  None
  * @retval None
  */
static int net_disconnect(MQTTClient *c)
{
    if (c->sock >= 0)
    {
        closesocket(c->sock);
        c->sock = -1;
    }

    return 0;
}

/**
  * @brief  
  * @param  None
  * @retval None
  */
static int sendPacket(MQTTClient *c, int length)
{
    int rc;
    struct timeval tv;

    tv.tv_sec = 2000;
    tv.tv_usec = 0;

    setsockopt(c->sock, SOL_SOCKET, SO_SNDTIMEO, (char *)&tv, sizeof(struct timeval));
    rc = send(c->sock, c->buf, length, 0);
    if (rc == length)
    {
        rc = 0;
    }
    else
    {
        rc = -1;
    }

    return rc;
}

static int net_read(MQTTClient *c, unsigned char *buf,  int len, int timeout)
{
    int bytes = 0;
    int rc;

    while (bytes < len)
    {
#ifdef MQTT_USING_TLS
        if (c->tls_session)
        {
            rc = mbedtls_client_read(c->tls_session, &buf[bytes], (size_t)(len - bytes));
            if (rc <= 0)
            {
                bytes = -1;
                break;
            }
            else
            {
                bytes += rc;
            }
            goto _continue;
        }
#endif

        rc = recv(c->sock, &buf[bytes], (size_t)(len - bytes), MSG_DONTWAIT);

        if (rc == -1)
        {
            if (errno != ENOTCONN && errno != ECONNRESET)
            {
                bytes = -1;
                break;
            }
        }
        else
            bytes += rc;

_continue:
        if (bytes >= len)
        {
            break;
        }

        if (timeout > 0)
        {
            fd_set readset;
            struct timeval interval;

            LOG_D("net_read %d:%d, timeout:%d\n", bytes, len, timeout);
            timeout  = 0;

            interval.tv_sec = 1;
            interval.tv_usec = 0;

            FD_ZERO(&readset);
            FD_SET(c->sock, &readset);

            select(c->sock + 1, &readset, NULL, NULL, &interval);
        }
        else
        {
            LOG_D("net_read %d:%d, break!", bytes, len);
            break;
        }
    }

    return bytes;
}

static int decodePacket(MQTTClient *c, int *value, int timeout)
{
    unsigned char i;
    int multiplier = 1;
    int len = 0;
    const int MAX_NO_OF_REMAINING_LENGTH_BYTES = 4;

    *value = 0;
    do
    {
        int rc = MQTTPACKET_READ_ERROR;

        if (++len > MAX_NO_OF_REMAINING_LENGTH_BYTES)
        {
            rc = MQTTPACKET_READ_ERROR; /* bad data */
            goto exit;
        }
        rc = net_read(c, &i, 1, timeout);
        if (rc != 1)
            goto exit;
        *value += (i & 127) * multiplier;
        multiplier *= 128;
    }
    while ((i & 128) != 0);
exit:
    return len;
}

static int MQTTPacket_readPacket(MQTTClient *c)
{
    int rc = PAHO_FAILURE;
    MQTTHeader header = {0};
    int len = 0;
    int rem_len = 0;

    /* 1. read the header byte.  This has the packet type in it */
    if (net_read(c, c->readbuf, 1, 0) != 1)
        goto exit;

    len = 1;
    /* 2. read the remaining length.  This is variable in itself */
    decodePacket(c, &rem_len, 50);
    len += MQTTPacket_encode(c->readbuf + 1, rem_len); /* put the original remaining length back into the buffer */

    /* 3. read the rest of the buffer using a callback to supply the rest of the data */
    if (rem_len > 0 && (net_read(c, c->readbuf + len, rem_len, 300) != rem_len))
        goto exit;

    header.byte = c->readbuf[0];
    rc = header.bits.type;

exit:
    return rc;
}

// assume topic filter and name is in correct format
// # can only be at end
// + and # can only be next to separator
static char isTopicMatched(char *topicFilter, MQTTString *topicName)
{
    char *curf = topicFilter;
    char *curn = topicName->lenstring.data;
    char *curn_end = curn + topicName->lenstring.len;

    while (*curf && curn < curn_end)
    {
        if (*curn == '/' && *curf != '/')
            break;
        if (*curf != '+' && *curf != '#' && *curf != *curn)
            break;
        if (*curf == '+')
        {
            // skip until we meet the next separator, or end of string
            char *nextpos = curn + 1;
            while (nextpos < curn_end && *nextpos != '/')
                nextpos = ++curn + 1;
        }
        else if (*curf == '#')
            curn = curn_end - 1;    // skip until end of string
        curf++;
        curn++;
    };

    return (curn == curn_end) && (*curf == '\0');
}

static void NewMessageData(MessageData *md, MQTTString *aTopicName, MQTTMessage *aMessage)
{
    md->topicName = aTopicName;
    md->message = aMessage;
}

/**
  * @brief  
  * @param  None
  * @retval None
  */
static int deliverMessage(MQTTClient *c, MQTTString *topicName, MQTTMessage *message)
{
    int i;
    int rc = PAHO_FAILURE;

    // we have to find the right message handler - indexed by topic
    for (i = 0; i < MAX_MESSAGE_HANDLERS; ++i)
    {
        if (c->messageHandlers[i].topicFilter != 0 && (MQTTPacket_equals(topicName, (char *)c->messageHandlers[i].topicFilter) ||
                isTopicMatched((char *)c->messageHandlers[i].topicFilter, topicName)))
        {
            if (c->messageHandlers[i].callback != NULL)
            {
                MessageData md;
                NewMessageData(&md, topicName, message);
                c->messageHandlers[i].callback(c, &md);
                rc = PAHO_SUCCESS;
            }
        }
    }

    if (rc == PAHO_FAILURE && c->defaultMessageHandler != NULL)
    {
        MessageData md;
        NewMessageData(&md, topicName, message);
        c->defaultMessageHandler(c, &md);
        rc = PAHO_SUCCESS;
    }

    return rc;
}


/**
  * @brief  
  * @param  None
  * @retval None
  */
static int MQTT_cycle(MQTTClient *c)
{
    // read the socket, see what work is due
    int packet_type = MQTTPacket_readPacket(c);

    int len = 0,
        rc = PAHO_SUCCESS;

    if (packet_type == -1)
    {
        rc = PAHO_FAILURE;
        goto exit;
    }

    switch (packet_type)
    {
    case CONNACK:
    case PUBACK:
    case SUBACK:
        break;
    case PUBLISH:
    {
        MQTTString topicName;
        MQTTMessage msg;
        int intQoS;
        if (MQTTDeserialize_publish(&msg.dup, &intQoS, &msg.retained, &msg.id, &topicName,
                                    (unsigned char **)&msg.payload, (int *)&msg.payloadlen, c->readbuf, c->readbuf_size) != 1)
            goto exit;
        msg.qos = (enum QoS)intQoS;
        deliverMessage(c, &topicName, &msg);
        if (msg.qos != QOS0)
        {
            if (msg.qos == QOS1)
                len = MQTTSerialize_ack(c->buf, c->buf_size, PUBACK, 0, msg.id);
            else if (msg.qos == QOS2)
                len = MQTTSerialize_ack(c->buf, c->buf_size, PUBREC, 0, msg.id);
            if (len <= 0)
                rc = PAHO_FAILURE;
            else
                rc = sendPacket(c, len);
            if (rc == PAHO_FAILURE)
                goto exit; // there was a problem
        }
        break;
    }
    case PUBREC:
    {
        unsigned short mypacketid;
        unsigned char dup, type;
        if (MQTTDeserialize_ack(&type, &dup, &mypacketid, c->readbuf, c->readbuf_size) != 1)
            rc = PAHO_FAILURE;
        else if ((len = MQTTSerialize_ack(c->buf, c->buf_size, PUBREL, 0, mypacketid)) <= 0)
            rc = PAHO_FAILURE;
        else if ((rc = sendPacket(c, len)) != PAHO_SUCCESS) // send the PUBREL packet
            rc = PAHO_FAILURE; // there was a problem
        if (rc == PAHO_FAILURE)
            goto exit; // there was a problem
        break;
    }
    case PUBCOMP:
        break;
    case PINGRESP:
        c->tick_ping = xTaskGetTickCount();
        break;
    }

exit:
    return rc;
}

typedef struct pub_msg
{
	int len;
	u8 buf[1024];
}PubMsg_t;
static PubMsg_t PubMsg[10];
static QueueHandle_t xQueuePub = NULL;
PubMsg_t PubMsgRcv;
static u8 WaitOkFlg = 0;
/**
  * @brief  
  * @param  None
  * @retval None
  */
static void paho_mqtt_thread(void *param)
{
	MQTTClient *c = (MQTTClient *)param;
	int i=0, rc=0, len=0;
	int rc_t = 0;
	
	if(xQueuePub == NULL)
	{
		xQueuePub = xQueueCreate(10, sizeof(PubMsg_t));
	}
	
_mqtt_start:	
    if (c->connect_callback)
    {
        c->connect_callback(c);
    }

	rc = net_connect(c);
	if (rc != 0)
	{
		goto _mqtt_restart;
	}
	rc = MQTTConnect(c);
	if (rc != 0)
	{
		goto _mqtt_restart;
	}

	log_d("MQTT server connect success\n");
	
	for (i = 0; i < MAX_MESSAGE_HANDLERS; i++)
	{
		const char *topic = c->messageHandlers[i].topicFilter;
		enum QoS qos = c->messageHandlers[i].qos;

		if (topic == NULL)
			continue;

		rc = MQTTSubscribe(c, topic, qos);
		log_d("Subscribe #%d %s %s!\n", i, topic, (rc < 0) || (rc == 0x80) ? ("fail") : ("OK"));

		if (rc != 0)
		{
			if (rc == 0x80)
			{
				log_e("QoS config err!\n");
			}
			goto _mqtt_disconnect;
		}
	}

	if (c->online_callback)
	{
		c->online_callback(c);
	}
		
	WaitOkFlg = 1;
	c->tick_ping = xTaskGetTickCount();
	while(1)
	{
		int res;
		u32 tick_now;
		fd_set readset;
		struct timeval timeout;		
		
		tick_now = xTaskGetTickCount();
		if (((tick_now - c->tick_ping) / configTICK_RATE_HZ) > (c->keepAliveInterval - 5))
		{
			timeout.tv_sec = 1;
			//log_d("tick close to ping.\n");
		}
		else
		{
			timeout.tv_sec = c->keepAliveInterval - 10 - (tick_now - c->tick_ping) / configTICK_RATE_HZ;
			//log_d("timeount for ping: %d\n", timeout.tv_sec);
		}		
		timeout.tv_usec = 0;			
		FD_ZERO(&readset);
		FD_SET(c->sock, &readset);
//		FD_SET(c->pub_pipe[0], &readset);		

		/* int select(maxfdp1, readset, writeset, exceptset, timeout); */
//		res = select(((c->pub_pipe[0] > c->sock) ? c->pub_pipe[0] : c->sock) + 1, &readset, NULL, NULL, &timeout);
		res = select( c->sock + 1, &readset, NULL, NULL, &timeout);
		
		if (res == 0) /* timeout */
		{
			len = MQTTSerialize_pingreq(c->buf, c->buf_size);
			rc = sendPacket(c, len);
			if (rc != 0)
			{
				log_e("[%d] send ping rc: %d\n", xTaskGetTickCount(), rc);
				goto _mqtt_disconnect;
			}

			/* wait Ping Response. */
			timeout.tv_sec = 5;
			timeout.tv_usec = 0;

			FD_ZERO(&readset);
			FD_SET(c->sock, &readset);

			res = select(c->sock + 1, &readset, NULL, NULL, &timeout);
			if (res <= 0)
			{
				log_e("[%d] wait Ping Response res: %d\n", xTaskGetTickCount(), res);
				goto _mqtt_disconnect;
			}
		} /* res == 0: timeount for ping. */
		
        if (res < 0)  /* err occur */
        {
            log_e("select res: %d", res);
            goto _mqtt_disconnect;
        }		
		
        if (FD_ISSET(c->sock, &readset))
        {
            //LOG_D("sock FD_ISSET");
            rc_t = MQTT_cycle(c);
            //LOG_D("sock FD_ISSET rc_t : %d", rc_t);
            if (rc_t < 0) 
			{
				goto _mqtt_disconnect;
			}				
            continue;
        }	

//		if (FD_ISSET(c->pub_pipe[0], &readset))
//		{
//            MQTTMessage *message;
//            MQTTString topic = MQTTString_initializer;

//            len = read(c->pub_pipe[0], c->readbuf, c->readbuf_size);

//            if (len < sizeof(MQTTMessage))
//            {
//                c->readbuf[len] = '\0';
//                log_d("pub_sock recv %d byte: %s\n", len, c->readbuf);

//                if (strcmp((const char *)c->readbuf, "DISCONNECT") == 0)
//                {
//                    log_d("DISCONNECT\n");
//                    goto _mqtt_disconnect_exit;
//                }

//                continue;
//            }

//            message = (MQTTMessage *)c->readbuf;
//            message->payload = c->readbuf + sizeof(MQTTMessage);
//            topic.cstring = (char *)c->readbuf + sizeof(MQTTMessage) + message->payloadlen;
//            log_d("pub_sock topic:%s, payloadlen:%d\n", topic.cstring, message->payloadlen);
//			
//            len = MQTTSerialize_publish(c->buf, c->buf_size, 0, message->qos, message->retained, message->id,
//                                        topic, (unsigned char *)message->payload, message->payloadlen);			
//            if (len <= 0)
//            {
//                log_d("MQTTSerialize_publish len: %d\n", len);
//                goto _mqtt_disconnect;
//            }

//            if ((rc = sendPacket(c, len)) != PAHO_SUCCESS) // send the subscribe packet
//            {
//                log_d("MQTTSerialize_publish sendPacket rc: %d\n", rc);
//                goto _mqtt_disconnect;
//            }			
//		}		
	}	
	
_mqtt_disconnect:
    MQTTDisconnect(c);
	
_mqtt_restart:
    if (c->offline_callback)
    {
        c->offline_callback(c);
    }

    net_disconnect(c);
	if(xQueuePub != NULL)
	{
		vQueueDelete(xQueuePub);
	}
	vTaskDelay(pdMS_TO_TICKS(5));
	log_d("restart!");	
    goto _mqtt_start;

_mqtt_disconnect_exit:
    MQTTDisconnect(c);
    net_disconnect(c);
	if(xQueuePub != NULL)
	{
		vQueueDelete(xQueuePub);
	}

_mqtt_exit:
    log_d("thread exit\n");
	
}


static void paho_mqtt_pub(void *param)
{
	int i=0, rc=0, len=0;
	MQTTClient *c = (MQTTClient *)param;
	
	while(1)
	{
		if(WaitOkFlg == 0)
		{
			vTaskDelay(5);
		}
		else
		{
			break;
		}
	}

	while(1)
	{
//		if (FD_ISSET(c->pub_pipe[0], &readset))
		xQueueReceive(xQueuePub, &PubMsgRcv, portMAX_DELAY);
		{
            MQTTMessage *message;
            MQTTString topic = MQTTString_initializer;

            //len = read(c->pub_pipe[0], c->readbuf, c->readbuf_size);
			len = PubMsgRcv.len;
            if (len < sizeof(MQTTMessage))
            {
                //c->readbuf[len] = '\0';
				PubMsgRcv.buf[len] = '\0';
                //log_d("pub_sock recv %d byte: %s\n", len, c->readbuf);
				log_d("pub_sock recv %d byte: %s\n", len, PubMsgRcv.buf);

                if (strcmp((const char *)c->readbuf, "DISCONNECT") == 0)
                {
                    log_d("DISCONNECT\n");
					MQTTDisconnect(c);
					net_disconnect(c);
					if(xQueuePub != NULL)
					{
						vQueueDelete(xQueuePub);
					}
                }

                continue;
            }

            message = (MQTTMessage *)PubMsgRcv.buf;
            message->payload = PubMsgRcv.buf + sizeof(MQTTMessage);
            topic.cstring = (char *)PubMsgRcv.buf + sizeof(MQTTMessage) + message->payloadlen;
            log_d("pub_sock topic:%s, payloadlen:%d\n", topic.cstring, message->payloadlen);
			
            len = MQTTSerialize_publish(c->buf, c->buf_size, 0, message->qos, message->retained, message->id,
                                        topic, (unsigned char *)message->payload, message->payloadlen);			
            if (len <= 0)
            {
                log_d("MQTTSerialize_publish len: %d\n", len);
       //         goto _mqtt_disconnect;
            }

            if ((rc = sendPacket(c, len)) != PAHO_SUCCESS) // send the subscribe packet
            {
                log_d("MQTTSerialize_publish sendPacket rc: %d\n", rc);
     //           goto _mqtt_disconnect;
            }			
		}		
	}

}	

/**
  * @brief  
  * @param  None
  * @retval None
  */
int paho_mqtt_start(MQTTClient *client)
{
    xTaskCreate(paho_mqtt_thread,      /* function  */
                "paho_mqtt_thread",    /* name    */
                256,                   /* stack size, unit: 4 byte */
                client,                  /* task param */
                3,                     /* priority */
                &xHandleTaskMqtt);     /* handle  */
	
    xTaskCreate(paho_mqtt_pub,      /* function  */
                "paho_mqtt_pub",    /* name    */
                256,                   /* stack size, unit: 4 byte */
                client,                  /* task param */
                4,                     /* priority */
                NULL);     /* handle  */	
}

/**
  * @brief  
  * @param  None
  * @retval None
  */
static int MQTT_local_send(MQTTClient *c, const void *data, int len)
{
    int send_len = 0;
	int i = 0;
	static int index = 0;
	char *p = (char *)data;
	
	for(i=0; i<len; i++)
	{
		PubMsg[index].buf[i] = p[i];
	}
	PubMsg[index].len = len;
	
	if( xQueueSend(xQueuePub, (void *)&(PubMsg[index]), 0) == pdPASS )
	{
		send_len = len;
		index++;
		if(index == 10)
		{
			index = 0;
		}			
	}

//    send_len = write(c->pub_pipe[1], data, len);

    return send_len;
}


/**
 * This function publish message to specified mqtt topic.
 * [MQTTMessage] + [payload] + [topic] + '\0'
 *
 * @param c the pointer of MQTT context structure
 * @param topicFilter topic filter name
 * @param message the pointer of MQTTMessage structure
 *
 * @return the error code, 0 on subscribe successfully.
 */
int MQTTPublish(MQTTClient *c, const char *topicName, MQTTMessage *message)
{
    int rc = PAHO_FAILURE;
    int len, msg_len;
    char *data = 0;

    if (!c->isconnected)
        goto exit;

    msg_len = sizeof(MQTTMessage) + message->payloadlen + strlen(topicName) + 1;
    data = pvMyPortMalloc(msg_len, SRAM_IN);
    if (!data)
	{
        goto exit;
	}

    memcpy(data, message, sizeof(MQTTMessage));
    memcpy(data + sizeof(MQTTMessage), message->payload, message->payloadlen);
    strcpy(data + sizeof(MQTTMessage) + message->payloadlen, topicName);

    len = MQTT_local_send(c, data, msg_len);
    if (len == msg_len)
    {
        rc = 0;
    }
    log_d("MQTTPublish sendto %d\n", len);

exit:
    if (data)
	{
        vMyPortFree(data, SRAM_IN);
	}

    return rc;
}


/****************** 北京龙鼎源科技 http://www.bdrl.com.cn/ ********************/
