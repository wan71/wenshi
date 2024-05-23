#include "driver.h"
#include "freertos_user.h"
#include "freertos_rtc.h"


QueueHandle_t uart2_to_wifi_Queue;  //创建一个可以于uart2接受中断通信的队列
extern TaskHandle_t esp8266_init_Handle;
char rev_ok[10];




void uart2_Task(void *aram)
{
	static g_rots_uart2 uart2_to_esp;
	BaseType_t xReturn = pdPASS;/* 定义一个创建信息返回值，默认为pdPASS */

	uart2_to_wifi_Queue = xQueueCreate(3,sizeof(g_rots_uart2));
  if(NULL != uart2_to_wifi_Queue)
    printf_1("创建uart2_to_wifi_Queue消息队列成功!\r\n");
	
	
	while(1)
	{
		static int d=0;
		char *a="+";
		 xReturn =xQueueReceive(uart2_to_wifi_Queue,&uart2_to_esp,portMAX_DELAY);
		if(pdTRUE == xReturn)
		{
			d++;
      printf_1("this is %d,本次接收到的数据是%s\r\n",d,uart2_to_esp.res);
			//vTaskDelay(10);

			if(strstr(uart2_to_esp.res, rev_ok) != NULL)		//如果检索到关键词
			{
				uart2_to_esp.i+=1;
				uart2_to_esp.i=uart2_to_esp.i%10;
				 xTaskNotify(esp8266_init_Handle,(uint32_t)uart2_to_esp.i,eSetValueWithoutOverwrite);
			}
			
			
		}
    else
      printf_1("数据接收出错,错误代码0x%lx\n",xReturn);
		 //vTaskDelay(10);
		
		
	}
}


int ESP8266_SendCmd(char *cmd, char *res)
{
	start_time_1();
	int rev=0;
	strcpy(rev_ok,res);
	ESP8266_send(cmd);
	xTaskNotifyWait(0,0xFFFFFFFF,&rev,portMAX_DELAY);
  // rev=xTaskNotifyTake(pdTRUE,portMAX_DELAY);
	 if(rev == 10)
    {
       printf_1("fail is %s",cmd);
			 vTaskDelete(NULL);
    }
		else //如果不等于10就成功
		{
			 printf_1("success is %s",cmd);
			 stop_time_1();
		}
		

   
}
	
void esp8266_init(void *aram)
{
	
	unsigned char *dataPtr;
	unsigned char status = 1;
	create_time_1();
do
	{
		vTaskDelay(1000);
		//ESP8266_SendCmd("AT+RST\r\n","OK");
		ESP8266_SendCmd("AT\r\n","OK");
		ESP8266_SendCmd("AT+CWMODE=1\r\n","OK");
		ESP8266_SendCmd("AT+CWDHCP=1,1\r\n", "OK");
		ESP8266_SendCmd("AT+CWAUTOCONN=0\r\n","OK");
		ESP8266_SendCmd(ESP8266_WIFI_INFO,"OK");
		ESP8266_SendCmd("AT+CIFSR\r\n","OK");
		ESP8266_SendCmd(ESP8266_ONENET_INFO, "CONNECT");
		printf_1("ESP8266 init is success\r\n");
//		
		//while(OneNet_DevLink());
	
		
		vTaskDelete(NULL);
	}
while(1);
	
	
}
	
	
//	vTaskDelay(1000);
//	ESP8266_send("AT+CWMODE=1\r\n");
//	vTaskDelay(1000);
//	ESP8266_send("AT+CWAUTOCONN=0\r\n");
//	vTaskDelay(1000);
//	ESP8266_send(ESP8266_WIFI_INFO);
//	vTaskDelay(7000);
//	ESP8266_send("AT+CIFSR\r\n");
//	vTaskDelay(1000);
//	



