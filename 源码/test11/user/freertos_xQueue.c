#include "driver.h"
#include "freertos_user.h"
#include "freertos_uart.h"
#include "queue.h"

struct uart_receive_data uart1_input;
struct uart_receive_data uart2_input;
struct uart_receive_data uart3_input;


static QueueSetHandle_t xQueueSet  = NULL;  /*队列集句柄*/
QueueHandle_t uart1_Queue;  
QueueHandle_t uart2_Queue; 
QueueHandle_t uart3_Queue;

struct uart_task g_my_uart;  /*这个结构体负责给UART_READ_Task传递参数*/
BaseType_t xReturn = pdPASS;/* 定义一个创建信息返回值，默认为pdPASS */

/*创建队列以及队列集合*/
void creat_queue(void)
{
	
	/*创建串口1的队列，将串口1的内容交给统一的任务函数处理其中的数据*/
	uart1_Queue = xQueueCreate(3,sizeof(struct uart_receive_data));
	if(NULL != uart1_Queue)
	{
    printf_1("创建uart1_Queue消息队列成功!\r\n");
	}
	xQueueReset(uart1_Queue); //复位
	

	/*创建串口2的队列，将串口2的内容交给统一的任务函数处理其中的数据*/
	uart2_Queue = xQueueCreate(3,sizeof(struct uart_receive_data));
	if(NULL != uart2_Queue)
	{
    printf_1("创建uart2_Queue消息队列成功!\r\n");
	}
	xQueueReset(uart2_Queue); //复位
	
	
		/*创建串口2的队列，将串口2的内容交给统一的任务函数处理其中的数据*/
	uart3_Queue = xQueueCreate(3,sizeof(struct uart_receive_data));
	if(NULL != uart3_Queue)
	{
    printf_1("创建uart3_Queue消息队列成功!\r\n");
	}
	xQueueReset(uart3_Queue); //复位
	
	
	/*创建队列集*/
     xQueueSet= xQueueCreateSet(9);            /* 创建队列集   参数:队列总长度   当前为：2+2 */
    (xQueueSet == NULL)? printf_1("error create queue set \r\n"):printf_1(" create queue set ok! \r\n");
	
	/* 3. 把3个queue添加进queue set */
	  xQueueAddToSet(uart1_Queue, xQueueSet);
	  xQueueAddToSet(uart2_Queue, xQueueSet);
	  xQueueAddToSet(uart3_Queue, xQueueSet);


	
}




void UART2_READ_Task(void *argument)
{

	struct uart_task* p = (struct uart_task *)argument;
	
	xReturn = xQueueReceive( uart2_Queue,    /* 消息队列的句柄 */
                             &uart2_input,      /* 发送的消息内容 */
                             0); /* 等待时间 一直等 */
	if(pdTRUE == xReturn)
		{
		printf_1("本次接收到的数据是%s",uart2_input.uart_re);
		printf_1("used_num 是%d\r\n",p->used_num);
		printf_1("\r\n");
		if(strstr((char *)uart2_input.uart_re, p->res) != NULL)		//如果检索到关键词
		{
		//	printf_2(uart2_input.uart2_re);
			printf_2("AT+CWJAP=\"lalala\",\"asdf12345678\"\r\n");
		}
	}
		
}


/*读队列集处理串口数据*/
void InputTask(void *params)
{

	creat_queue();
	QueueSetMemberHandle_t xQueueHandle;
	while(1)
	{
		/* 读队列集, 得到有数据的队列句柄 */
		xQueueHandle = xQueueSelectFromSet(xQueueSet, portMAX_DELAY);
		if (xQueueHandle)
		{
			/* 读队列句柄得到数据,处理数据 */
			if(xQueueHandle == uart1_Queue)
			{
				printf_1("this is uart1_Queue\r\n");
				
			}
			else if(xQueueHandle == uart2_Queue)
			{
				
				printf_1("this is uart2_Queue\r\n");
				UART2_READ_Task(params);
				
			}
			else if(xQueueHandle == uart3_Queue)
			{
				printf_1("this is uart2_Queue\r\n");
				
			}
			
		}
	}
	
	
	
}


