#ifndef __FREERTOS_UART1_H
#define __FREERTOS_UART1_H


#define  PowerTime 30

void RTC_Task(void *argument);
void Alarm_set(uint8_t Hours,uint8_t Minutes,uint8_t Seconds);

#endif
