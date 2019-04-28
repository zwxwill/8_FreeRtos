/**
  ******************************************************************************
  * @file    test_mqtt.c
  * @author  zwx
  * @version V1.8.0
  * @date    2019-01-29
  * @brief
  ******************************************************************************
  */

#include "test_mqtt.h"
#include "mqtt/mqtt.h"
#include "lwip/ip_addr.h"

#include "srv_printf.h"

#include "FreeRtos.h"
#include "task.h"

/* Exported variables --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private struct  -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

#define LWIP_CONST_CAST(target_type, val)	((target_type)(val))


#if LWIP_TCP

/** Define this to a compile-time IP address initialization
 * to connect anything else than IPv4 loopback
 */

#ifndef LWIP_MQTT_EXAMPLE_IPADDR_INIT
#if LWIP_IPV4
#define LWIP_MQTT_EXAMPLE_IPADDR_INIT = IPADDR4_INIT(IPADDR_LOOPBACK)
#else
#define LWIP_MQTT_EXAMPLE_IPADDR_INIT
#endif
#endif

static ip_addr_t mqtt_ip LWIP_MQTT_EXAMPLE_IPADDR_INIT;
static mqtt_client_t* mqtt_client;

static const struct mqtt_connect_client_info_t mqtt_client_info =
{
    "test", /* Client ID */
    "admin", /* user */
    "admin", /* pass */
    60,   /* keep alive */
    NULL, /* will_topic */
    NULL, /* will_msg */
    0,    /* will_qos */
    0     /* will_retain */
#if LWIP_ALTCP && LWIP_ALTCP_TLS
    , NULL
#endif
};

static void mqtt_incoming_data_cb(void *arg, const u8_t *data, u16_t len, u8_t flags)
{
    const struct mqtt_connect_client_info_t* client_info = (const struct mqtt_connect_client_info_t*)arg;
    LWIP_UNUSED_ARG(data);

    printf("mqtt_incoming_data_cb MQTT client \"%s\" data cb: len %d, flags %d\n", client_info->client_id,
           (int)len, (int)flags);
}

static void mqtt_incoming_publish_cb(void *arg, const char *topic, u32_t tot_len)
{
    const struct mqtt_connect_client_info_t* client_info = (const struct mqtt_connect_client_info_t*)arg;

    printf("mqtt_incoming_publish_cb MQTT client \"%s\" publish cb: topic %s, len %d\n", client_info->client_id,
           topic, (int)tot_len);
}

static void mqtt_request_cb(void *arg, err_t err)
{
    const struct mqtt_connect_client_info_t* client_info = (const struct mqtt_connect_client_info_t*)arg;

    printf("mqtt_request_cb MQTT client \"%s\" request cb: err %d\n", client_info->client_id, (int)err);
}

static void mqtt_connection_cb(mqtt_client_t *client, void *arg, mqtt_connection_status_t status)
{
    const struct mqtt_connect_client_info_t* client_info = (const struct mqtt_connect_client_info_t*)arg;
    LWIP_UNUSED_ARG(client);

    printf("mqtt_connection_cb MQTT client \"%s\" connection cb: status %d\n", client_info->client_id, (int)status);

    if(status == MQTT_CONNECT_ACCEPTED)
    {
		SRV_DEBUGF(SRV_MQTT_DEBUG | SRV_DBG_LEVEL_DEBUG, ("mqtt_connection_cb: Successfully connected\n"));
        mqtt_sub_unsub(client,
                       "topic_qos1", 1,
                       mqtt_request_cb, LWIP_CONST_CAST(void*, client_info),
                       1);
        mqtt_sub_unsub(client,
                       "topic_qos0", 0,
                       mqtt_request_cb, LWIP_CONST_CAST(void*, client_info),
                       1);
    }
}
#endif /* LWIP_TCP */

void mqtt_example_init(void)
{
#if LWIP_TCP
	err_t err;
	
    mqtt_client = mqtt_client_new();

    mqtt_set_inpub_callback(mqtt_client,
                            mqtt_incoming_publish_cb,
                            mqtt_incoming_data_cb,
                            LWIP_CONST_CAST(void*, &mqtt_client_info));
	
	IP4_ADDR(&mqtt_ip, 192, 168, 1, 155);   /* m2m.eclipse.org 198.41.30.241   阿里云：139.196.135.135 */

    err = mqtt_client_connect(mqtt_client,
                        &mqtt_ip, MQTT_PORT,
                        mqtt_connection_cb, LWIP_CONST_CAST(void*, &mqtt_client_info),
                        &mqtt_client_info);
	if(err != ERR_OK)
	{
		SRV_DEBUGF(SRV_MQTT_DEBUG | SRV_DBG_LEVEL_SERIOUS, ("mqtt_client_connect err = %d\r\n", err));
	}
#endif /* LWIP_TCP */
}


void vTaskTestMqtt(void *pvParameters)
{
	mqtt_example_init();
	
    while(1)
    {
        vTaskDelay(1000);
    }	
}


/****************** 北京龙鼎源科技 http://www.bdrl.com.cn/ ********************/

