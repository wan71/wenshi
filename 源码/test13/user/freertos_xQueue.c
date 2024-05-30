#include "driver.h"
#include "freertos_user.h"
#include "freertos_uart.h"
#include "queue.h"

struct uart_receive_data uart1_input;
struct uart_receive_data uart2_input;
struct uart_receive_data uart3_input;


static QueueSetHandle_t xQueueSet  = NULL;  /*���м����*/
QueueHandle_t uart1_Queue;  
QueueHandle_t uart2_Queue; 
QueueHandle_t uart3_Queue;

struct uart_task g_my_uart;  /*����ṹ�帺���UART_READ_Task���ݲ���*/
BaseType_t xReturn = pdPASS;/* ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS */

/*���������Լ����м���*/
void creat_queue(void)
{
	
	/*��������1�Ķ��У�������1�����ݽ���ͳһ���������������е�����*/
	uart1_Queue = xQueueCreate(3,sizeof(struct uart_receive_data));
	if(NULL != uart1_Queue)
	{
    printf_1("����uart1_Queue��Ϣ���гɹ�!\r\n");
	}
	xQueueReset(uart1_Queue); //��λ
	

	/*��������2�Ķ��У�������2�����ݽ���ͳһ���������������е�����*/
	uart2_Queue = xQueueCreate(3,sizeof(struct uart_receive_data));
	if(NULL != uart2_Queue)
	{
    printf_1("����uart2_Queue��Ϣ���гɹ�!\r\n");
	}
	xQueueReset(uart2_Queue); //��λ
	
	
		/*��������2�Ķ��У�������2�����ݽ���ͳһ���������������е�����*/
	uart3_Queue = xQueueCreate(3,sizeof(struct uart_receive_data));
	if(NULL != uart3_Queue)
	{
    printf_1("����uart3_Queue��Ϣ���гɹ�!\r\n");
	}
	xQueueReset(uart3_Queue); //��λ
	
	
	/*�������м�*/
     xQueueSet= xQueueCreateSet(9);            /* �������м�   ����:�����ܳ���   ��ǰΪ��2+2 */
    (xQueueSet == NULL)? printf_1("error create queue set \r\n"):printf_1(" create queue set ok! \r\n");
	
	/* 3. ��3��queue��ӽ�queue set */
	  xQueueAddToSet(uart1_Queue, xQueueSet);
	  xQueueAddToSet(uart2_Queue, xQueueSet);
	  xQueueAddToSet(uart3_Queue, xQueueSet);


	
}




void UART2_READ_Task(void *argument)
{

	struct uart_task* p = (struct uart_task *)argument;
	
	xReturn = xQueueReceive( uart2_Queue,    /* ��Ϣ���еľ�� */
                             &uart2_input,      /* ���͵���Ϣ���� */
                             0); /* �ȴ�ʱ�� һֱ�� */
	if(pdTRUE == xReturn)
		{
		printf_1("���ν��յ���������%s",uart2_input.uart_re);
		printf_1("used_num ��%d\r\n",p->used_num);
		printf_1("\r\n");
		if(strstr((char *)uart2_input.uart_re, p->res) != NULL)		//����������ؼ���
		{
		//	printf_2(uart2_input.uart2_re);
			printf_2("AT+CWJAP=\"lalala\",\"asdf12345678\"\r\n");
		}
	}
		
}


/*�����м�����������*/
void InputTask(void *params)
{

	creat_queue();
	QueueSetMemberHandle_t xQueueHandle;
	while(1)
	{
		/* �����м�, �õ������ݵĶ��о�� */
		xQueueHandle = xQueueSelectFromSet(xQueueSet, portMAX_DELAY);
		if (xQueueHandle)
		{
			/* �����о���õ�����,�������� */
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


