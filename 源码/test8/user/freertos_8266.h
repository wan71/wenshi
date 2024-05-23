#ifndef __FREERTOS_8266_H
#define __FREERTOS_8266_H

#include "driver_uart.h"
#define WIFI_SSID "K40"
#define WIFI_PASSWORD "WL020827#"

#define ESP8266_WIFI_INFO "AT+CWJAP=\"" WIFI_SSID "\",\"" WIFI_PASSWORD "\"\r\n"
#define ESP8266_ONENET_INFO		"AT+CIPSTART=\"TCP\",\"mqtts.heclouds.com\",1883\r\n"

#define A_DEVID	"9B7KjM6hUa"                           //�豸id   TGpUxQSNgO9B7KjM6hUa

#define A_PIKEY	 "BQWXgxs/UT9Fjelj+Xnh87VvKyWbo0+xTREjg7k7ewM="       
//����    GzljBwXFOzPDQ8Ujc4ELycsdWTtfN5MpWzXXp831dS8=

typedef struct{
	char res[receiveData_uart2];
	int i;
	
}g_rots_uart2;



//typedef struct Buffer
//{
//	
//	uint8_t	*_data;		//Э������
//	
//	uint32_t	_len;		//д������ݳ���
//	
//	uint32_t	_size;		//�����ܴ�С
//	
//	uint8_t	_memFlag;	//�ڴ�ʹ�õķ�����0-δ����	1-ʹ�õĶ�̬����		2-ʹ�õĹ̶��ڴ�
//	
//} EDP_PACKET_STRUCTURE;

void uart2_Task(void *aram);
void esp8266_init(void *aram);
#endif