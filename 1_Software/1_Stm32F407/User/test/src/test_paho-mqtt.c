/**
  ******************************************************************************
  * @file    test_paho-mqtt.c
  * @author  zwx
  * @version V1.8.0
  * @date    2019-01-29
  * @brief   
  ******************************************************************************
  */  

#include "test_paho-mqtt.h"
#include "paho_mqtt_pipe.h"
#include "paho_mqtt.h"
#include "MQTTPacket.h"

#include "FreeRTOS.h"
#include "task.h"

#include "heap_6.h"

#include "elog.h"
#include <stdio.h>
#include <string.h>

/* Exported variables --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private struct  -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static void mq_publish(const char *send_str);

/**
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
 */
#define MQTT_URI                "tcp://iot.eclipse.org:1883"
#define MQTT_USERNAME           "admin"
#define MQTT_PASSWORD           "admin"
#define MQTT_SUBTOPIC           "/mqtt/test"
#define MQTT_PUBTOPIC           "/mqtt/test"
#define MQTT_WILLMSG            "Goodbye!"

/* define MQTT client context */
static MQTTClient client;
static char sup_pub_topic[48] = { 0 };


static void mqtt_sub_callback(MQTTClient *c, MessageData *msg_data)
{
    *((char *)msg_data->message->payload + msg_data->message->payloadlen) = '\0';
    log_d("Topic: %.*s receive a message: %.*s\n",
               msg_data->topicName->lenstring.len,
               msg_data->topicName->lenstring.data,
               msg_data->message->payloadlen,
               (char *)msg_data->message->payload);

    return;
}

static void mqtt_sub_default_callback(MQTTClient *c, MessageData *msg_data)
{
    *((char *)msg_data->message->payload + msg_data->message->payloadlen) = '\0';
    log_d("mqtt sub default callback: %.*s %.*s\n",
               msg_data->topicName->lenstring.len,
               msg_data->topicName->lenstring.data,
               msg_data->message->payloadlen,
               (char *)msg_data->message->payload);
    return;
}

static void mqtt_connect_callback(MQTTClient *c)
{
    log_d("Start to connect mqtt server\n");
}

static void mqtt_online_callback(MQTTClient *c)
{
    log_d("Connect mqtt server success\n");
    log_d("Publish message: Hello,RT-Thread! to topic: %s\n", sup_pub_topic);
    mq_publish("Hello,RT-Thread!\n");
}

static void mqtt_offline_callback(MQTTClient *c)
{
    log_d("Disconnect from mqtt server\n");
}


/**
  * @brief  
  * @param  None
  * @retval None
  */
void PahoMqtt_Init(void)
{
    /* init condata param by using MQTTPacket_connectData_initializer */
    MQTTPacket_connectData condata = MQTTPacket_connectData_initializer;
    static char cid[20] = { 0 };
	

    static int is_started = 0;
    if (is_started)
    {
        return;
    }
	
    /* config MQTT context param */
    {
        client.isconnected = 0;
        client.uri = MQTT_URI;

        /* generate the random client ID */
        sprintf(cid, "-rtthread%d", xTaskGetTickCount());
        sprintf(sup_pub_topic, "%s%s", MQTT_PUBTOPIC, cid);
        /* config connect param */
        memcpy(&client.condata, &condata, sizeof(condata));
        client.condata.clientID.cstring = cid;
        client.condata.keepAliveInterval = 60;
        client.condata.cleansession = 1;
        client.condata.username.cstring = MQTT_USERNAME;
        client.condata.password.cstring = MQTT_PASSWORD;

        /* config MQTT will param. */
        client.condata.willFlag = 0;
        client.condata.will.qos = 1;
        client.condata.will.retained = 0;
        client.condata.will.topicName.cstring = sup_pub_topic;

        /* malloc buffer. */
        client.buf_size = client.readbuf_size = 1024;
        client.buf = pvMyPortMalloc(client.buf_size, SRAM_IN);
        client.readbuf = pvMyPortMalloc(client.readbuf_size, SRAM_IN);
        if (!(client.buf && client.readbuf))
        {
            log_e("no memory for MQTT client buffer!\n");
            goto _exit;
        }

        /* set event callback function */
        client.connect_callback = mqtt_connect_callback;
        client.online_callback = mqtt_online_callback;
        client.offline_callback = mqtt_offline_callback;

        /* set subscribe table and event callback */
        client.messageHandlers[0].topicFilter = sup_pub_topic;
        client.messageHandlers[0].callback = mqtt_sub_callback;
        client.messageHandlers[0].qos = QOS1;

        /* set default subscribe event callback */
        client.defaultMessageHandler = mqtt_sub_default_callback;
    }	
	
    /* run mqtt client */
    paho_mqtt_start(&client);
    is_started = 1;	
	
_exit:
    return;	
}

/**
 * This function publish message to specific mqtt topic.
 *
 * @param send_str publish message
 *
 * @return none
 */
static void mq_publish(const char *send_str)
{
    MQTTMessage message;
    const char *msg_str = send_str;
    const char *topic = MQTT_PUBTOPIC;
    message.qos = QOS1;
    message.retained = 0;
    message.payload = (void *)msg_str;
    message.payloadlen = strlen(message.payload);

    MQTTPublish(&client, topic, &message);

    return;
}





/**
  * @brief  
  * @param  None
  * @retval None
  */
void Test_PahoMqtt(void *pvParameters)	
{
	PahoMqtt_Init();
	
	while(1)
	{
		vTaskDelay(1000);
	}
}






/****************** 北京龙鼎源科技 http://www.bdrl.com.cn/ ********************/


