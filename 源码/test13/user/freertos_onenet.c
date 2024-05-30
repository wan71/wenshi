#include "driver.h"
#include "freertos_user.h"
#include "timecall.h"
#include "FreeRTOS.h"
#include "timers.h"
#include "edpkit.h"

extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart1;





_Bool OneNet_DevLink(void)
{
	EDP_PACKET_STRUCTURE edpPacket = {NULL, 0, 0, 0};				//协议包

	unsigned char *dataPtr;
	unsigned char status = 1;
	unsigned short count = 0;
	printf_1("OneNet_DevLink\r\n");
	printf_1("DEVID: %s,APIKEY: %s\r\n",A_DEVID, A_PIKEY);
	
	if(EDP_PacketConnect1(A_DEVID, A_PIKEY, 256, &edpPacket) == 0)		//根据devid 和 apikey封装协议包
	{
		char cmdBuf[32];
		vTaskDelay(100);
		sprintf(cmdBuf, "AT+CIPSEND=%d\r\n",edpPacket._len);		//发送命令
		ESP8266_SendCmd(cmdBuf,">");
		vTaskDelay(10);
		for(; count < edpPacket._len; count++)
		{
		HAL_UART_Transmit(&huart1, (uint8_t *)edpPacket._data++,1,0xFFFF);//发送数据
		}
		//Usart_SendString(edpPacket._data, edpPacket._len);
		//ESP8266_SendCmd(edpPacket._data,"OK");
		
		
		//ESP8266_SendData(edpPacket._data, edpPacket._len);			//上传平台
		
	}
	
	return !status;
}