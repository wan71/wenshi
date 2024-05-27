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
//		if(strstr(uart2.res, rev_ok) != NULL)		//����������ؼ���
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
		// ����ַ���ĩβ�Ļ��з��Ϳո�
		char *res_ptr = uart2.res;
		while (*res_ptr && (*res_ptr == '\n' || *res_ptr == '\r' || *res_ptr == ' ')) {
				res_ptr++;
		}
		
		
		if(strstr(res_ptr, rev_ok) != NULL)		//����������ؼ���
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



// �������ESP8266_SendCmd������֧�ֿɱ����
int ESP8266_SendCmd(char *res, char *cmdFormat, ...) {
    start_time_1();
    int rev = 0;

    // ȷ����Ӧ�ַ�����������������С
    strncpy(rev_ok, res, sizeof(rev_ok) - 1);
    rev_ok[sizeof(rev_ok) - 1] = '\0';

    // ʹ��va_list����ɱ����
    va_list args;
    va_start(args, cmdFormat);
    ESP8266_send(cmdFormat, args);
    va_end(args);

    // �ȴ�֪ͨ��������Ƿ�ɹ�
    BaseType_t notifyResult = xTaskNotifyWait(0, 0xFFFFFFFF, (uint32_t *)&rev, portMAX_DELAY);

    if (notifyResult == pdTRUE) {
        if (rev == 10) {
            printf_1("Fail: %s\r\n", cmdFormat);
            //stop_time_1();
					 vTaskDelete(NULL);
            return -1;  // ���ش����룬��ʾʧ��
        } else {
					stop_time_1();
           printf_1("Success: %s\r\n", cmdFormat);
					 vTaskDelay(100);
            
            return 0;  // ����0����ʾ�ɹ�
        }
    } else {
        printf_1("Notification wait failed for command: %s\r\n", cmdFormat);
        stop_time_1();
        return -2;  // ���ش����룬��ʾ�ȴ�֪ͨʧ��
    }
}



