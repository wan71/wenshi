#ifndef __FREERTOS_8266_H
#define __FREERTOS_8266_H

#include "driver_uart.h"
#define WIFI_SSID "K40"
#define WIFI_PASSWORD "WL020827#"

#define ESP8266_WIFI_INFO "AT+CWJAP=\"" WIFI_SSID "\",\"" WIFI_PASSWORD "\"\r\n"
#define ESP8266_ONENET_INFO		"AT+CIPSTART=\"TCP\",\"mqtts.heclouds.com\",1883\r\n"

#define A_DEVID	"9B7KjM6hUa"                           //设备id   TGpUxQSNgO9B7KjM6hUa

#define A_PIKEY	 "BQWXgxs/UT9Fjelj+Xnh87VvKyWbo0+xTREjg7k7ewM="       
//密码    GzljBwXFOzPDQ8Ujc4ELycsdWTtfN5MpWzXXp831dS8=

typedef struct{
	char res[receiveData_uart2];
	int i;
	
}g_rots_uart2;



//typedef struct Buffer
//{
//	
//	uint8_t	*_data;		//协议数据
//	
//	uint32_t	_len;		//写入的数据长度
//	
//	uint32_t	_size;		//缓存总大小
//	
//	uint8_t	_memFlag;	//内存使用的方案：0-未分配	1-使用的动态分配		2-使用的固定内存
//	
//} EDP_PACKET_STRUCTURE;

void uart2_Task(void *aram);
void esp8266_init(void *aram);
#endif