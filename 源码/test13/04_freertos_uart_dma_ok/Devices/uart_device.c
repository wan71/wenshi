#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_usart.h"
#include "uart_device.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "queue.h"
#include "string.h"

#define UART_RX_QUEUE_LEN 100

extern UART_HandleTypeDef huart1;

static struct UART_Device *g_cur_uart1_dev;


/******************************************************/
/* 使用中断+FreeRTOS队列,信号量 */


struct UART_Device g_stm32_uart1;

struct UART_Data {
    UART_HandleTypeDef *handle;
    SemaphoreHandle_t xTxSem;
    QueueHandle_t xRxQueue;
    uint8_t rxdatas[100];
};


void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    struct UART_Data *data;
    
    if (huart == &huart1)
    {
        data = g_cur_uart1_dev->priv_data;
        
        /* 释放信号量 */
        xSemaphoreGiveFromISR(data->xTxSem, NULL);
    }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    struct UART_Data *data;
    int len = huart->RxXferSize - huart->RxXferCount;
    
    if (huart == &huart1)
    {
        data = g_cur_uart1_dev->priv_data;
        
        /* 写队列 */
        for (int i = 0; i < len; i++)
        {
            xQueueSendFromISR(data->xRxQueue, &data->rxdatas[i], NULL);
        }

        /* 再次启动数据的接收 */
        if (g_cur_uart1_dev == &g_stm32_uart1)
            HAL_UART_Receive_IT(data->handle, data->rxdatas, huart->RxXferSize);
        else
            HAL_UARTEx_ReceiveToIdle_DMA(data->handle, data->rxdatas, huart->RxXferSize);
    }
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    struct UART_Data *data;
    int len = Size;
    
    if (huart == &huart1)
    {
        data = g_cur_uart1_dev->priv_data;
        
        /* 写队列 */
        for (int i = 0; i < len; i++)
        {
            xQueueSendFromISR(data->xRxQueue, &data->rxdatas[i], NULL);
        }

        /* 再次启动数据的接收 */
        HAL_UARTEx_ReceiveToIdle_DMA(data->handle, data->rxdatas, huart->RxXferSize);
    }
}



/******************************************************/
/* 使用DMA, FreeRTOS队列,信号量 */

static int stm32_dma_uart_init(struct UART_Device *pDev, int baud, int datas, char parity, int stop)
{
    struct UART_Data *data = pDev->priv_data;

    g_cur_uart1_dev = pDev;

    data->xTxSem = xSemaphoreCreateBinary();
    data->xRxQueue = xQueueCreate(UART_RX_QUEUE_LEN, 1);

    /* 启动第1次数据的接收 */
    //HAL_UART_Receive_DMA(data->handle, &data->rxdatas, 100);
    HAL_UARTEx_ReceiveToIdle_DMA(data->handle, data->rxdatas, 100);
    
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

static int stm32_dma_uart_recv(struct UART_Device *pDev, uint8_t *data, int timeout_ms)
{
    struct UART_Data *uart_data = pDev->priv_data;
    
    /* 读取队列得到数据, 问题:谁写队列?中断:写队列 */
    if (pdPASS == xQueueReceive(uart_data->xRxQueue, data,timeout_ms))
        return 0;
    else
        return -1;
}

static struct UART_Data g_stm32_dma_uart1_data = {
    &huart1,
};


static struct UART_Device g_stm32_dma_uart1 = {
    "stm32_dma_uart1",
    stm32_dma_uart_init,
    stm32_dma_uart_send,
    stm32_dma_uart_recv,
    &g_stm32_dma_uart1_data,
};


/******************************************************/


struct UART_Device *g_uart_devs[] = {&g_stm32_dma_uart1};

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


