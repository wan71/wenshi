#include "driver.h"
#include "freertos_user.h"
#include "freertos_rtc.h"
#include "stdarg.h"  // for va_list, va_start, va_end

#define res_strstr 10
extern TaskHandle_t esp8266_init_Handle;


//��Ϣ���п���Ȩ��
QueueHandle_t xMy_8266_uart2_Queue;
struct rev_confirm{
	char rev_ok[10];
	int i; //�ж��Ƿ�Ҫ��⣬Ϊ1ΪҪ��⣬Ϊ0λ��
};



void uart2_test(void *argument)
{
	struct g_rots_RxQueue uart2;
	static struct rev_confirm p; //һ��Ҫ�Ǿ�̬��������Ϊ���л�ն�

	BaseType_t xReturn = pdPASS;/* ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS */
	
	struct UART_Device *pUARTDev_2 = GetUARTDevice("stm32_dma_uart2");
	pUARTDev_2->Init(pUARTDev_2, 115200, 8, 'N', 1);
	vTaskDelay(1000);
	printf_1("uart2_test init success\r\n");
	while(1)
	{
	
	  while(rece_2(&uart2,portMAX_DELAY));
		p.i=1;
		printf_1("this is uart2_re:%s\r\n",uart2.res);
		vTaskDelay(10);

		/*��֤��������*/
		 xReturn = xQueueReceive(xMy_8266_uart2_Queue,&p,1);
	
		if(p.i==1)
		{
			vTaskDelay(10);
	   if(strstr(uart2.res, p.rev_ok) != NULL)		//����������ؼ���
			 {
				uart2.i+=1;
				uart2.i=uart2.i%10;
				xTaskNotify(esp8266_init_Handle,(uint32_t)uart2.i,eSetValueWithoutOverwrite);
			}
		}
     
	}
	
}

void esp8266_init(void *aram)
{
	 
	vTaskDelay(1000);
	create_time_1();
	do
	{
		const char *atCommandFormat = "AT+MQTTUSERCFG=0,1,\"%s\",\"%s\",\"%s\",0,0,\"\"\r\n";
    const char *device = "d1";
    const char *user = "9B7KjM6hUa";
    const char *auth = "version=2018-10-31&res=products%2F9B7KjM6hUa%2Fdevices%2Fd1&et=2031317695&method=md5&sign=nJsKYjG3prH6NF4SbZNs5g%3D%3D";

	  const char *atCommand_2 = "AT+MQTTCONN=0,\"mqtts.heclouds.com\",1883,0\r\n";
		const char *atCommand_3 = "+MQTTSUB=0,\"$sys/9B7KjM6hUa/d1/thing/property/post/reply\",0\r\n";
		char *wifi_name="K40";
		char *wifi_mima="WL020827#";
		xMy_8266_uart2_Queue= xQueueCreate(1,sizeof(struct rev_confirm));
		if(NULL != xMy_8266_uart2_Queue)
		{
    printf_1("����xMy_8266_uart2_Queue��Ϣ���гɹ�!\r\n");
		}
		/*��һ��*/
		printf_2("AT\r\n");
		ESP8266_SendCmd("OK",1); 
		/*�ڶ���*/
		printf_2("AT+CWMODE=1\r\n");
		ESP8266_SendCmd("OK",1); 
		/*������*/
		printf_2("AT+CIFSR\r\n");
		ESP8266_SendCmd("OK",1); 
		
//		/*������*/
//		printf_2("AT+RST\r\n");
//		ESP8266_SendCmd("OK",0); 
//		vTaskDelay(2000);
//		
		
			/*���Ĳ�*/
		printf_2("AT+CWJAP=\"%s\",\"%s\"\r\n",wifi_name,wifi_mima);
		ESP8266_SendCmd("OK",1);
		
		/*���岽*/
		printf_2("AT+CIPSTART=\"TCP\",\"mqtts.heclouds.com\",1883\r\n");
		ESP8266_SendCmd("OK",1);
		/*������*/
		printf_2("AT+CIPMODE=1\r\n");
		ESP8266_SendCmd("OK",1);
		/*���߲�*/
		//printf_2("%s\r\n",atCommand_1);
		// ʹ��printf_2����ƴ�Ӳ�����AT����
   printf_2((char *)atCommandFormat, device, user, auth);
		ESP8266_SendCmd("OK",1);
	/*�ڰ˲�*/
		printf_2("%s",atCommand_2);
		//ESP8266_SendCmd("OK",1);
	 /*�ھŲ�*/
		printf_2("%s",atCommand_3);
		ESP8266_SendCmd("OK",1);
		
		
	
	}while(0);
	
	 vTaskDelete(NULL);
}


void ESP8266_SendCmd(char *res,int i) {
    
	static struct rev_confirm q;
	BaseType_t xReturn = pdPASS;/* ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS */
		start_time_1();
    int rev = 0;
    // ȷ����Ӧ�ַ�����������������С
    strncpy(q.rev_ok, res, sizeof(q.rev_ok) - 1);
    q.rev_ok[sizeof(q.rev_ok) - 1] = '\0';
	  q.i=i;
	
	  xReturn=xQueueSend(xMy_8266_uart2_Queue,&q,0);
		if(pdPASS != xReturn)
		{
					printf_1("��Ϣsend_data1����ʧ��!\n\n");
			   stop_time_1();
		    vTaskDelete(NULL);
		}
		if(q.i==1)
		{
    // �ȴ�֪ͨ��������Ƿ�ɹ�
    BaseType_t notifyResult = xTaskNotifyWait(0, 0xFFFFFFFF, (uint32_t *)&rev, portMAX_DELAY);
	  if (notifyResult == pdTRUE) 
			 {
					if (rev == 10) 
					{
						 printf_1("fail\r\n");
						 vTaskDelete(NULL);
					} 
					else 
					{
						stop_time_1();
						 printf_1("Success\r\n");
					}
				}
		}


		 
 }



