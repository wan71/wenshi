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
///* 二进制信号量句柄 */
//SemaphoreHandle_t uart1_tx;




driver_uart_1 uart_1;
driver_uart_2 uart_2;
driver_uart_3 uart_3;



extern QueueHandle_t uart2_to_wifi_Queue;  //创建一个可以于uart2接受中断通信的队列
/**********************************************************************
 * 函数名称： UART_Init
 * 功能描述： UART初始化函数
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 0 - 成功, 其他值 - 失败
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/05	     V1.0	  韦东山	      创建
 ***********************************************************************/
void UART_Init(void)
{
}

int stm32_uart_init(driver_uart_1 *pDev)
{
	pDev->uart1_tx= xSemaphoreCreateBinary( );
	
}


static int inHandlerMode (void) //若在中断中__get_IPSR()返回1，否则返回0
{
   return __get_IPSR();  
}

int fputc(int c, FILE *f) 
{
    (void)f;
    HAL_UART_Transmit(g_HDebugUART, (const uint8_t *)&c, 1, DEBUG_UART_TIMEOUT);
    return c;
}



void print_usart1(char *format, ...) 
{
	  
    char buf[receiveData_uart1];
	
		if(inHandlerMode() != 0)
		{
					taskDISABLE_INTERRUPTS();//若在中断中调用则关闭中断，防止中断嵌套造成线程不安全
		}
		else //若不在中断中则进入临界区关闭中断且禁止任务调度
		{
		while(HAL_UART_GetState(&huart1) == HAL_UART_STATE_BUSY_TX)//若串口忙则挂起此任务
		taskYIELD();
		}
			
		va_list ap;
		va_start(ap, format);
		vsnprintf((char*)buf,sizeof(buf)+1,(char*)format,ap);
		
	 HAL_UART_Transmit_DMA(&huart1, (uint8_t *)buf, strlen(buf));
		
		xSemaphoreTake(uart_1.uart1_tx,1000);
 	//HAL_Delay(10);//为防止发送覆盖
		va_end(ap);
		//vTaskDelay(10);  //为防止发送覆盖
		if(inHandlerMode() != 0)
			taskENABLE_INTERRUPTS();
}

void print_usart2(char *format, ...)
{
    char buf[receiveData_uart2];
    if(inHandlerMode() != 0)
	{
      taskDISABLE_INTERRUPTS();
	}
    else
    {
		while(HAL_UART_GetState(&huart2) == HAL_UART_STATE_BUSY_TX)//若串口忙则挂起此任务
		taskYIELD();
		}
	va_list ap;
	va_start(ap, format);
	vsprintf(buf,format, ap);
		
	//HAL_UART_Transmit(&huart2, (uint8_t *)buf, strlen(buf),0x100);
		HAL_UART_Transmit_DMA(&huart2, (uint8_t *)buf, strlen(buf));
	//	HAL_Delay(10);
	va_end(ap);
	if(inHandlerMode() != 0)
	taskENABLE_INTERRUPTS();
}




void print_usart3(char *format, ...)
{
    char buf[receiveData_uart3];
    if(inHandlerMode() != 0)
	{
      taskDISABLE_INTERRUPTS();
	}
    else
    {
		while(HAL_UART_GetState(&huart3) == HAL_UART_STATE_BUSY_TX)//若串口忙则挂起此任务
		taskYIELD();
		}
	va_list ap;
	va_start(ap, format);
	vsprintf(buf, format, ap);
	//HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf),0x100);
		HAL_UART_Transmit_DMA(&huart3, (uint8_t *)buf, strlen(buf));
	va_end(ap);
	if(inHandlerMode() != 0)
	taskENABLE_INTERRUPTS();
}


int fgetc(FILE *f)
{
    uint8_t ch = 0;
    (void)f;

    /* Clear the Overrun flag just before receiving the first character */
    __HAL_UART_CLEAR_OREFLAG(g_HDebugUART);

    /* Wait for reception of a character on the USART RX line and echo this
    * character on console */
    HAL_UART_Receive(g_HDebugUART, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
    HAL_UART_Transmit(g_HDebugUART, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
    return ch;    
}



/**********************************************************************
 * 函数名称： UART_Test
 * 功能描述： UART测试程序
 * 输入参数： 无
 * 输出参数： 无
 *            无
 * 返 回 值： 0 - 成功, 其他值 - 失败
 * 修改日期        版本号     修改人        修改内容
 * -----------------------------------------------
 * 2023/08/05        V1.0     韦东山       创建
 ***********************************************************************/
void UART_Test(void *argument)
{    
    char c;
	  BaseType_t err;
    UART_Init();
    while (1)
    {
		
		  printf_1("uaet 1 test!\r\n");
			vTaskDelay(100);
			printf_2("uaet 2 test!\r\n");
			vTaskDelay(100);
			printf_3("uaet 3 test!\r\n");
			vTaskDelay(100);

    }
}





void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
  /* Prevent unused argument(s) compilation warning */
	
	if(huart->Instance == USART1)
	{
  huart->gState = HAL_UART_STATE_READY;//huart1传输完成 变为准备
	xSemaphoreGiveFromISR(uart_1.uart1_tx,NULL);
	}
	if(huart->Instance == USART2)
  huart->gState = HAL_UART_STATE_READY;//huart1传输完成 变为准备
	if(huart->Instance == USART3)
  huart->gState = HAL_UART_STATE_READY;//huart1传输完成 变为准备C
  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_UART_TxCpltCallback can be implemented in the user file.
   */

}



void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
	g_rots_uart2 uart2_to_esp;
  
	BaseType_t xReturn;/* 定义一个创建信息返回值，默认为pdPASS */
	
	
	if(huart->Instance == USART1)
	{
		extern DMA_HandleTypeDef hdma_usart1_rx;
		
		printf("%s",uart_1.aRxBuffer);
		
		
		memset(uart_1.aRxBuffer,0x00,sizeof(uart_1.aRxBuffer));
	 HAL_UARTEx_ReceiveToIdle_DMA(&huart1, (uint8_t *)&uart_1.aRxBuffer, receiveData_uart1); 
	 __HAL_DMA_DISABLE_IT(&hdma_usart1_rx,DMA_IT_HT);
	}
	else if(huart->Instance == USART2)
	{
		extern DMA_HandleTypeDef hdma_usart2_rx;
//		static int i;
//		printf_1("this is %d,%s\r\n",i++,uart_2.aRxBuffer);
		
		
		strcpy(uart2_to_esp.res,uart_2.aRxBuffer);
		xReturn =xQueueSendToBackFromISR(uart2_to_wifi_Queue,&uart2_to_esp,0);
		
		

		
		
		
		memset(uart_2.aRxBuffer,0x00,sizeof(uart_2.aRxBuffer));
		HAL_UARTEx_ReceiveToIdle_DMA(&huart2, (uint8_t *)&uart_2.aRxBuffer, receiveData_uart2); 
		__HAL_DMA_DISABLE_IT(&hdma_usart2_rx,DMA_IT_HT);
		
	}
		else if(huart->Instance == USART3)
	{
		extern DMA_HandleTypeDef hdma_usart3_rx;

		printf_3("%s\r\n",uart_3.aRxBuffer);
		HAL_UARTEx_ReceiveToIdle_DMA(&huart3, (uint8_t *)&uart_3.aRxBuffer, receiveData_uart3); 
	__HAL_DMA_DISABLE_IT(&hdma_usart3_rx,DMA_IT_HT); 
		
	}
	
	
	
}





























///*这是串口的接受中断，接受中断不负责处理数据，数据处理在任务函数中，所以它负者将数据写入队列*/
//void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
//{


//  /* Prevent unused argument(s) compilation warning */
//  UNUSED(huart);
//  /* NOTE: This function Should not be modified, when the callback is needed,
//           the HAL_UART_TxCpltCallback could be implemented in the user file
//   */

//	if(huart->Instance == USART1)
//	{
//		while(HAL_UART_Receive_DMA(&huart1,(uint8_t *)&uart_1.RxBuffer,1) != HAL_OK )   
//		{
//			 huart1.RxState = HAL_UART_STATE_READY;
//        __HAL_UNLOCK(&huart1);
//		}
//				if(uart_1.Rx_Cnt >= RXBUFFERSIZE)  //溢出判断
//			{
//				uart_1.Rx_Cnt = 0;
//				memset(uart_1.aRxBuffer,0x00,sizeof(uart_1.aRxBuffer));
//				//HAL_UART_Transmit_DMA(&huart1, (uint8_t *)"数据溢出", 10); 	
//				printf_1("数据溢出\r\n");			
//			}
//			else
//			{
//				
//				uart_1.aRxBuffer[uart_1.Rx_Cnt++] = uart_1.RxBuffer;   

//				if((uart_1.aRxBuffer[uart_1.Rx_Cnt-1] == 0x0A)&&(uart_1.aRxBuffer[uart_1.Rx_Cnt-2] == 0x0D)) //判断结束位
//				{
//					
//					/*
//					添加自己的处理代码
//					*/
//				
//					uart_1.Rx_Cnt = 0;
//					memset(uart_1.aRxBuffer,0x00,sizeof(uart_1.aRxBuffer)); //清空数组
//				
//					
//				}
//			}
//			
//			HAL_UART_Receive_DMA(&huart1, (uint8_t *)&uart_1.RxBuffer, 1);   //因为接收中断使用了一次即关闭，所以在最后加入这行代码即可实现无限使用
//	}	
//	else if(huart->Instance == USART2)
//	{
//		static int i;
//		OLED_Clear();
//		OLED_PrintSignedVal(0, 4, i++);

//		
//		if(uart_2.Rx_Cnt >= RXBUFFERSIZE)  //溢出判断
//		{
//			uart_2.Rx_Cnt = 0;
//			memset(uart_2.aRxBuffer,0x00,sizeof(uart_2.aRxBuffer));
//			//HAL_UART_Transmit_DMA(&huart2, (uint8_t *)"数据溢出", 10); 	
//			printf_2("数据溢出\r\n");						
//		}
//		else
//		{
//			uart_2.aRxBuffer[uart_2.Rx_Cnt++] = uart_2.RxBuffer; 

//			
//			if((uart_2.aRxBuffer[uart_2.Rx_Cnt-1] == 0x0A)&&(uart_2.aRxBuffer[uart_2.Rx_Cnt-2] == 0x0D)) //判断结束位
//			{
//				/*
//					添加自己的处理代码
//					*/
//				//printf_2("%s",uart_2.aRxBuffer);
//			
//				strcpy(uart2_input.uart_re, uart_2.aRxBuffer);
//				xQueueSendToBackFromISR(uart2_Queue,&uart2_input,0);

//				
//				uart_2.Rx_Cnt = 0;
//				memset(uart_2.aRxBuffer,0x00,sizeof(uart_2.aRxBuffer)); //清空数组
//				 
//			}
//		}
//				while(HAL_UART_Receive_DMA(&huart2,(uint8_t *)&uart_2.RxBuffer,1) != HAL_OK )   
//		{
//			 huart2.RxState = HAL_UART_STATE_READY;
//        __HAL_UNLOCK(&huart2);
//		}
//		HAL_UART_Receive_DMA(&huart2, (uint8_t *)&uart_2.RxBuffer, 2);   //因为接收中断使用了一次即关闭，所以在最后加入这行代码即可实现无限使用

//	}
//	
//		else if(huart->Instance == USART3)
//	{
//			while(HAL_UART_Receive_DMA(&huart3,(uint8_t *)&uart_3.RxBuffer,1) != HAL_OK )   
//						{
//							 huart3.RxState = HAL_UART_STATE_READY;
//								__HAL_UNLOCK(&huart3);
//						}
//		
//		if(uart_3.Rx_Cnt >= RXBUFFERSIZE)  //溢出判断
//		{
//			uart_3.Rx_Cnt = 0;
//			memset(uart_3.aRxBuffer,0x00,sizeof(uart_3.aRxBuffer));
//			//HAL_UART_Transmit_DMA(&huart3, (uint8_t *)"数据溢出", 10);
//			printf_3("数据溢出\r\n");			
//		}
//		else
//		{
//			uart_3.aRxBuffer[uart_3.Rx_Cnt++] = uart_3.RxBuffer; 

//			
//			if((uart_3.aRxBuffer[uart_3.Rx_Cnt-1] == 0x0A)&&(uart_3.aRxBuffer[uart_3.Rx_Cnt-2] == 0x0D)) //判断结束位
//			{
//					
//				/*
//					添加自己的处理代码
//					*/
//				//printf_3("%s\r\n",uart_3.aRxBuffer);
//				
//				uart_3.Rx_Cnt = 0;
//				memset(uart_3.aRxBuffer,0x00,sizeof(uart_3.aRxBuffer)); //清空数组
//				 
//			}
//		}
//		
//		HAL_UART_Receive_DMA(&huart3, (uint8_t *)&uart_3.RxBuffer, 1);   //因为接收中断使用了一次即关闭，所以在最后加入这行代码即可实现无限使用

//	}
//	
//	

//}

