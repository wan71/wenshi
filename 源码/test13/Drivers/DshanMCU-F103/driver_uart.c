/*  Copyright (s) 2019 深圳百问网科技有限公司
 *  All rights reserved
 * 
 * 文件名称：driver_uart.c
 * 摘要：
 *  
 * 修改历史     版本号        Author       修改内容
 *--------------------------------------------------
 * 2023.8.05      v01         百问科技      创建文件
 *--------------------------------------------------
*/
#include "FreeRTOS.h"
#include "driver_uart.h"
#include "driver_lcd.h"
#include "freertos_user.h"
#include "driver_timer.h"
#include "usart.h"
#include "semphr.h"
#include <stdio.h>
#include <stdarg.h>
#include "event_groups.h"


extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;
static UART_HandleTypeDef * g_HDebugUART = &huart1;


//struct UART_Device g_stm32_uart1;
static struct UART_Device *g_cur_uart1_dev;
static struct UART_Device *g_cur_uart2_dev;
static struct UART_Device *g_cur_uart3_dev;
/***************************************************************************************************************/

struct UART_Data {
    UART_HandleTypeDef *handle;
    SemaphoreHandle_t xTxSem;
    QueueHandle_t xRxQueue;
    uint8_t rxdatas[UART_RX_QUEUE_LEN];
};

static struct UART_Data g_stm32_dma_uart1_data = {
    &huart1,
	  
};
static struct UART_Data g_stm32_dma_uart2_data = {
    &huart2,
	  
};
static struct UART_Data g_stm32_dma_uart3_data = {
    &huart3,
	  
};


void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
  /* Prevent unused argument(s) compilation warning */
	 struct UART_Data *data;
	if(huart->Instance == USART1)
	{
  huart->gState = HAL_UART_STATE_READY;//huart1传输完成 变为准备
	 data = g_cur_uart1_dev->priv_data;
		/* 释放信号量 */
     xSemaphoreGiveFromISR(data->xTxSem, NULL);
	}
	if(huart->Instance == USART2)
	{
		huart->gState = HAL_UART_STATE_READY;//huart1传输完成 变为准备
		data = g_cur_uart2_dev->priv_data;
		/* 释放信号量 */
     xSemaphoreGiveFromISR(data->xTxSem, NULL);
	}
	if(huart->Instance == USART3)
	{
  huart->gState = HAL_UART_STATE_READY;//huart1传输完成 变为准备C
		data = g_cur_uart3_dev->priv_data;
		/* 释放信号量 */
     xSemaphoreGiveFromISR(data->xTxSem, NULL);
		
		
	}
  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_UART_TxCpltCallback can be implemented in the user file.
   */

}


void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
	struct UART_Data *data;
   int len = Size;
  
	if(huart->Instance == USART1)
	{
		extern DMA_HandleTypeDef hdma_usart1_rx;
		 struct g_rots_RxQueue uart1;
		data = g_cur_uart1_dev->priv_data;
	 /* 写队列 */
//		for (int i = 0; i < len; i++)
//		{
//				xQueueSendFromISR(data->xRxQueue, &data->rxdatas[i], NULL);
//		}
		  strcpy(uart1.res,data->rxdatas);
			xQueueSendFromISR(data->xRxQueue, &uart1,0);
		  memset(data->rxdatas,0x00,sizeof(data->rxdatas));
	
	/* 再次启动数据的接收 */
     HAL_UARTEx_ReceiveToIdle_DMA(data->handle, data->rxdatas, huart->RxXferSize);
		 __HAL_DMA_DISABLE_IT(&hdma_usart1_rx,DMA_IT_HT);
	}
	else if(huart->Instance == USART2)
	{
		extern DMA_HandleTypeDef hdma_usart2_rx;
		 struct g_rots_RxQueue uart2;
		 data = g_cur_uart2_dev->priv_data;
		 strcpy(uart2.res,data->rxdatas);
			xQueueSendFromISR(data->xRxQueue, &uart2,0);
		  memset(data->rxdatas,0x00,sizeof(data->rxdatas));
		
			/* 再次启动数据的接收 */
     HAL_UARTEx_ReceiveToIdle_DMA(data->handle, data->rxdatas, huart->RxXferSize);
		 __HAL_DMA_DISABLE_IT(&hdma_usart2_rx,DMA_IT_HT);
		
	}
	else if(huart->Instance == USART3)
	{
		static int i;
		extern DMA_HandleTypeDef hdma_usart3_rx;
		 struct g_rots_RxQueue uart3;
		 data = g_cur_uart3_dev->priv_data;
		 strcpy(uart3.res,data->rxdatas);
			xQueueSendFromISR(data->xRxQueue, &uart3,0);
		  memset(data->rxdatas,0x00,sizeof(data->rxdatas));
		i++;
			/* 再次启动数据的接收 */
     HAL_UARTEx_ReceiveToIdle_DMA(data->handle, data->rxdatas, huart->RxXferSize);
		 __HAL_DMA_DISABLE_IT(&hdma_usart3_rx,DMA_IT_HT);
		
	}

	
	
}


/* 使用DMA, FreeRTOS队列,信号量 */
static int stm32_dma_uart_init(struct UART_Device *pDev, int baud, int datas, char parity, int stop)
{
    struct UART_Data *data = pDev->priv_data;

	  if(data->handle->Instance==USART1)
		{
    g_cur_uart1_dev = pDev;
		}
		else if(data->handle->Instance==USART2)
		{
			 g_cur_uart2_dev = pDev;
			
		}
		else if(data->handle->Instance==USART3)
		{
			 g_cur_uart3_dev = pDev;
			
		}
    data->xTxSem = xSemaphoreCreateBinary();
    data->xRxQueue = xQueueCreate(1,sizeof(struct g_rots_RxQueue));

    /* 启动第1次数据的接收 */
    //HAL_UART_Receive_DMA(data->handle, &data->rxdatas, 100);
   // HAL_UARTEx_ReceiveToIdle_DMA(data->handle, data->rxdatas, UART_RX_QUEUE_LEN);
	HAL_UARTEx_ReceiveToIdle_DMA(data->handle, (uint8_t *)data->rxdatas, UART_RX_QUEUE_LEN); 
	
    
    return 0;
}


static int stm32_dma_uart_send(struct UART_Device *pDev, uint8_t *datas, int len, int timeout_ms)
{
    struct UART_Data *data = pDev->priv_data;
    
    /* 仅仅是触发中断而已 */
    HAL_UART_Transmit_DMA(data->handle, datas, len);

    /* 等待发送完毕:等待信号量 */
    if (pdTRUE == xSemaphoreTake(data->xTxSem, timeout_ms))
        return 0;
    else
        return -1;
}


static int stm32_dma_uart_recv(struct UART_Device *pDev, void *para, int timeout_ms)
{
    struct UART_Data *uart_data = pDev->priv_data;
    struct g_rots_RxQueue *p=(struct g_rots_RxQueue *)para;
    /* 读取队列得到数据, 问题:谁写队列?中断:写队列 */
    if (pdPASS == xQueueReceive(uart_data->xRxQueue,p,timeout_ms))
		{
		   // print_usart1("this is %s\r\n",p.res);
        return 0;
		}
		
    else
        return -1;
}



static struct UART_Device g_stm32_dma_uart1 = {
    "stm32_dma_uart1",
    stm32_dma_uart_init,
	  stm32_dma_uart_send,
	  stm32_dma_uart_recv,
    &g_stm32_dma_uart1_data,
};

static struct UART_Device g_stm32_dma_uart2 = {
    "stm32_dma_uart2",
    stm32_dma_uart_init,
	  stm32_dma_uart_send,
	  stm32_dma_uart_recv,
    &g_stm32_dma_uart2_data,
};

static struct UART_Device g_stm32_dma_uart3 = {
    "stm32_dma_uart3",
    stm32_dma_uart_init,
	  stm32_dma_uart_send,
	  stm32_dma_uart_recv,
    &g_stm32_dma_uart3_data,
};



/******************************************************/
struct UART_Device *g_uart_devs[] = {&g_stm32_dma_uart1,&g_stm32_dma_uart2,&g_stm32_dma_uart3};

struct UART_Device *GetUARTDevice(char *name)
{
    int i = 0;
    for (i = 0; i < sizeof(g_uart_devs)/sizeof(g_uart_devs[0]); i++)
    {
        if (0 == strcmp(name, g_uart_devs[i]->name))
            return g_uart_devs[i];
    }

    return NULL;
}




/****************************************************************自己封装的****************************************************/

void print_usart1(char *format, ...) 
{
    //char buf[UART_RX_QUEUE_LEN];
	 char buf[300]={0};
		va_list ap;
		va_start(ap, format);
		vsnprintf((char*)buf,sizeof(buf)+1,(char*)format,ap);
	 HAL_UART_Transmit_DMA(&huart1, (uint8_t *)buf, strlen(buf));
		xSemaphoreTake(g_stm32_dma_uart1_data.xTxSem,1000);
		va_end(ap);
}

void print_usart2(char *format, ...) 
{
    //char buf[UART_RX_QUEUE_LEN];

	 char buf[300];
    va_list ap;
    va_start(ap, format);
    vsnprintf(buf, sizeof(buf), format, ap);
    va_end(ap);
	 HAL_UART_Transmit_DMA(&huart2, (uint8_t *)buf, strlen(buf));
//	HAL_UART_Transmit(&huart2, (uint8_t *)buf, strlen(buf),HAL_MAX_DELAY);
		xSemaphoreTake(g_stm32_dma_uart2_data.xTxSem,1000);

}

void print_usart3(char *format, ...) 
{
    char buf[UART_RX_QUEUE_LEN];
		va_list ap;
		va_start(ap, format);
		vsnprintf((char*)buf,sizeof(buf)+1,(char*)format,ap);
	 HAL_UART_Transmit_DMA(&huart3, (uint8_t *)buf, strlen(buf));
		xSemaphoreTake(g_stm32_dma_uart3_data.xTxSem,1000);
		va_end(ap);
}


int rece_usart1(void *para,int aTime) 
{
    struct g_rots_RxQueue *p=(struct g_rots_RxQueue *)para;
	 if (pdPASS == xQueueReceive(g_stm32_dma_uart1_data.xRxQueue,p,portMAX_DELAY))
		{
		   
        return 0;
		}
		
    else
        return -1;
}



int rece_usart2(void *para,int aTime) 
{
    struct g_rots_RxQueue *p=(struct g_rots_RxQueue *)para;
	 if (pdPASS == xQueueReceive(g_stm32_dma_uart2_data.xRxQueue,p,aTime))
		{
		   
        return 0;
		}
		
    else
        return -1;
}

int rece_usart3(void *para,int aTime) 
{
    struct g_rots_RxQueue *p=(struct g_rots_RxQueue *)para;
	 if (pdPASS == xQueueReceive(g_stm32_dma_uart3_data.xRxQueue,p,aTime))
		{
		   
        return 0;
		}
		
    else
        return -1;
}














