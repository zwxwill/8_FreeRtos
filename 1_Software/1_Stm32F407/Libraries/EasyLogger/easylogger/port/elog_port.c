/*
 * This file is part of the EasyLogger Library.
 *
 * Copyright (c) 2015, Armink, <armink.ztl@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * 'Software'), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Function: Portable interface for each platform.
 * Created on: 2015-04-28
 */
 
#include <elog.h>
#include "bsp_uart.h"

#include "FreeRtos.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include <stdio.h>

static SemaphoreHandle_t xSem_LockLog = NULL;

static QueueHandle_t xQueue_LogNotice = NULL;
static u8 notice = 0;
static void vTaskAsyncOutLog(void *arg);

/**
 * EasyLogger port initialize
 *
 * @return result
 */
ElogErrCode elog_port_init(void) {
    ElogErrCode result = ELOG_NO_ERR;

	xSem_LockLog = xSemaphoreCreateBinary();
	xSemaphoreGive(xSem_LockLog); /* 当前有一个资源可用 */
	
    /* add your code here */
#ifdef ELOG_ASYNC_OUTPUT_ENABLE
    xQueue_LogNotice = xQueueCreate(1024, sizeof(uint8_t));
	if( xQueue_LogNotice == 0 )
	{
		printf("xQueue_Log create error\r\n");
	}	
	
    xTaskCreate(vTaskAsyncOutLog,      /* function  */
                "vTaskAsyncOutLog",    /* name    */
                1024,                  /* stack size, unit: 4 byte */
                NULL,                  /* task param */
                configMAX_PRIORITIES-5,/* priority */ /* 这个优先级不能设置太小，容易buffer溢出 */
                NULL);                 /* handle  */
#endif	
	
    return result;
}

/**
 * output log port interface
 *
 * @param log output of log
 * @param size log size
 */
void elog_port_output(const char *log, size_t size) {
    
    /* add your code here */
	Uart_SendBuf((u8 *)log, size);
}

/**
 * output lock
 */
void elog_port_output_lock(void) {
    
    /* add your code here */
    xSemaphoreTake(xSem_LockLog, (TickType_t)portMAX_DELAY);
}

/**
 * output unlock
 */
void elog_port_output_unlock(void) {
    
    /* add your code here */
    xSemaphoreGive(xSem_LockLog); 
}

/**
 * get current time interface
 *
 * @return current time
 */
const char *elog_port_get_time(void) {
    
    /* add your code here */
	static char buf[11] = { 0 };

	sprintf(buf, "%10d", xTaskGetTickCount());
	
    return buf;
}

/**
 * get current process name interface
 *
 * @return current process name
 */
const char *elog_port_get_p_info(void) {
    
    /* add your code here */
    return "";
}

/**
 * get current thread name interface
 *
 * @return current thread name
 */
const char *elog_port_get_t_info(void) {
    
    /* add your code here */
	return pcTaskGetName(NULL);
}


#ifdef ELOG_ASYNC_OUTPUT_ENABLE
void elog_async_output_notice(void) {
    xQueueSend(xQueue_LogNotice,(void *) &notice, (TickType_t)5000);
}

static void vTaskAsyncOutLog(void *arg) {
    size_t get_log_size = 0;
    static char poll_get_buf[ELOG_LINE_BUF_SIZE - 4];

    while(true) {
        /* waiting log */
        xQueueReceive(xQueue_LogNotice, (void *)&notice, portMAX_DELAY);
        /* polling gets and outputs the log */
        while(true) {

#ifdef ELOG_ASYNC_LINE_OUTPUT
            get_log_size = elog_async_get_line_log(poll_get_buf, sizeof(poll_get_buf));
#else
            get_log_size = elog_async_get_log(poll_get_buf, sizeof(poll_get_buf));
#endif

            if (get_log_size) {
                elog_port_output(poll_get_buf, get_log_size);
            } else {
                break;
            }
        }
    }
}
#endif
