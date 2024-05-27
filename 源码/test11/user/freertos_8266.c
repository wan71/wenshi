#include "driver.h"
#include "freertos_user.h"
#include "freertos_rtc.h"
#include "stdarg.h"  // for va_list, va_start, va_end

#define res_strstr 10
extern TaskHandle_t esp8266_init_Handle;
char rev_ok[10]={"OK"};

//void uart2_test(void *argument)
//{
//	struct g_rots_RxQueue uart2;
//	struct UART_Device *pUARTDev_2 = GetUARTDevice("stm32_dma_uart2");
//	pUARTDev_2->Init(pUARTDev_2, 115200, 8, 'N', 1);
//	printf_2("uart2_test init success\r\n");
//	while(1)
//	{
//	  while(rece_2(&uart2,portMAX_DELAY));
//		printf_1("this is uart2_re:%s\r\n",uart2.res);
//		vTaskDelay(10);
//		if(strstr(uart2.res, rev_ok) != NULL)		//如果检索到关键词
//		{
//			uart2.i+=1;
//			uart2.i=uart2.i%10;
//			xTaskNotify(esp8266_init_Handle,(uint32_t)uart2.i,eSetValueWithoutOverwrite);
//		}
//		
//		
//	}
//	
//}


void uart2_test(void *argument)
{
	struct g_rots_RxQueue uart2;
	struct UART_Device *pUARTDev_2 = GetUARTDevice("stm32_dma_uart2");
	pUARTDev_2->Init(pUARTDev_2, 115200, 8, 'N', 1);
	printf_2("uart2_test init success\r\n");
	while(1)
	{
	  while(rece_2(&uart2,portMAX_DELAY));
		vTaskDelay(10);
		printf_1("this is uart2_re:%s\r\n",uart2.res);
		// 清除字符串末尾的换行符和空格
		char *res_ptr = uart2.res;
		while (*res_ptr && (*res_ptr == '\n' || *res_ptr == '\r' || *res_ptr == ' ')) {
				res_ptr++;
		}
		
		
		if(strstr(res_ptr, rev_ok) != NULL)		//如果检索到关键词
		{
			uart2.i+=1;
			uart2.i=uart2.i%10;
			xTaskNotify(esp8266_init_Handle,(uint32_t)uart2.i,eSetValueWithoutOverwrite);
		}
		
		
	}
	
}

void esp8266_init(void *aram)
{
	 
	vTaskDelay(1000);
	create_time_1();
	do
	{ 
		/* AT command to configure MQTT user */

    const char *atCommand_1 = ESP8266_a;
		const char *atCommand_2 = "AT+MQTTCONN=0,\"mqtts.heclouds.com\",1883,0\r\n";
		const char *atCommand_3 = "+MQTTSUB=0,\"$sys/9B7KjM6hUa/d1/thing/property/post/reply\",0";
		ESP8266_SendCmd("OK","AT\r\n");
		ESP8266_SendCmd("OK","AT+CWMODE=1\r\n");
		ESP8266_SendCmd("OK","AT+CIFSR\r\n");
		//ESP8266_SendCmd("OK","AT+RST\r\n");
		//printf_2("AT+RST\r\n");
		//stop_time_1();
 		//vTaskDelay(10000);
		
		//ESP8266_SendCmd("OK","AT\r\n");
	  ESP8266_SendCmd("OK",ESP8266_WIFI_INFO);
		ESP8266_SendCmd("CONNECT",ESP8266_ONENET_INFO );
    ESP8266_SendCmd("OK","AT+CIPMODE=1\r\n");
   	printf_1("ESP8266 init is success\r\n");

		 //ESP8266_SendCmd("OK", atCommand_1);
	
	printf_2("%s",atCommand_1);
		vTaskDelay(2000);
		printf_2("%s",atCommand_2);
	  vTaskDelay(2000);
	  printf_2("%s",atCommand_3);
		vTaskDelay(2000);
	}while(0);
	
	 vTaskDelete(NULL);
}



// 升级后的ESP8266_SendCmd函数，支持可变参数
int ESP8266_SendCmd(char *res, char *cmdFormat, ...) {
    start_time_1();
    int rev = 0;

    // 确保响应字符串不超过缓冲区大小
    strncpy(rev_ok, res, sizeof(rev_ok) - 1);
    rev_ok[sizeof(rev_ok) - 1] = '\0';

    // 使用va_list处理可变参数
    va_list args;
    va_start(args, cmdFormat);
    ESP8266_send(cmdFormat, args);
    va_end(args);

    // 等待通知，并检查是否成功
    BaseType_t notifyResult = xTaskNotifyWait(0, 0xFFFFFFFF, (uint32_t *)&rev, portMAX_DELAY);

    if (notifyResult == pdTRUE) {
        if (rev == 10) {
            printf_1("Fail: %s\r\n", cmdFormat);
            //stop_time_1();
					 vTaskDelete(NULL);
            return -1;  // 返回错误码，表示失败
        } else {
					stop_time_1();
           printf_1("Success: %s\r\n", cmdFormat);
					 vTaskDelay(100);
            
            return 0;  // 返回0，表示成功
        }
    } else {
        printf_1("Notification wait failed for command: %s\r\n", cmdFormat);
        stop_time_1();
        return -2;  // 返回错误码，表示等待通知失败
    }
}



