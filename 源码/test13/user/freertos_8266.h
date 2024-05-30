#ifndef __FREERTOS_8266_H
#define __FREERTOS_8266_H

#include "driver_uart.h"
#define WIFI_SSID "K40"
#define WIFI_PASSWORD "WL020827#"

#define ESP8266_WIFI_INFO "AT+CWJAP=\"" WIFI_SSID "\",\"" WIFI_PASSWORD "\"\r\n"
#define ESP8266_ONENET_INFO		"AT+CIPSTART=\"TCP\",\"mqtts.heclouds.com\",1883\r\n"
// 定义命令中的各个参数
  
#define ESP8266_a "AT+MQTTUSERCFG=0,1,\"d1\",\"9B7KjM6hUa\",\"version=2018-10-31&res=products%2F9B7KjM6hUa%2Fdevices%2Fd1&et=2031317695&method=md5&sign=nJsKYjG3prH6NF4SbZNs5g%3D%3D\",0,0,\"\"\r\n"
#define ESP8266_b "AT+MQTTCONN=0,\"mqtts.heclouds.com\",1883,0\r\n"
//#define ESP8266_a "AT+MQTTUSERCFG=0,1,\"d1\",\"9B7KjM6hUa\",\"version=2018-10-31&res=products%2F9B7KjM6hUa%2Fdevices%2Fd1&et=2031317695&method=md5&sign=nJsKYjG3prH6NF4SbZNs5g%3D%3D\",0,0,\"\""
#define A_DEVID	"9B7KjM6hUa"                           //设备id   TGpUxQSNgO9B7KjM6hUa

#define A_PIKEY	 "BQWXgxs/UT9Fjelj+Xnh87VvKyWbo0+xTREjg7k7ewM="       
//密码    GzljBwXFOzPDQ8Ujc4ELycsdWTtfN5MpWzXXp831dS8=

//void ESP8266_SendCmd(char *res,char *format, ...);
void esp8266_init(void *aram);
void ESP8266_SendCmd(char *res,int i);
void uart2_test(void *argument);
#endif