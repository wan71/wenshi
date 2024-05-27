#include "driver.h"
#include "freertos_user.h"
#include "timecall.h"
#include "FreeRTOS.h"
#include "timers.h"


static TimerHandle_t xTimeHandle[2];
extern TaskHandle_t esp8266_init_Handle;

void timeCallBackTask(void)
{

	xTaskNotify(esp8266_init_Handle,(uint32_t)10,eSetValueWithoutOverwrite);
	
}

void stop_time_1()
{
	xTimerStop(xTimeHandle[0],portMAX_DELAY);
	

}


void create_time_1()
{
	

	xTimeHandle[0] = xTimerCreate(
								(const char *)"task1",// 定时器名字
								(TickType_t)TickType_time_1,// 定时器的周期
								pdFALSE,// 定时器是单次执行还是周期执行, pdTRUE为周期执行，pdFALSE为周期执行
								(void*)1,// 给定时器分配的唯一ID
								(TimerCallbackFunction_t)timeCallBackTask// 定时器的回调函数
								);


}


void start_time_1()
{
			if(xTimeHandle[0])
		{
			//printf_1("timecall_1 init success\r\n");
			xTimerStart(xTimeHandle[0],0); //开启定时器
		}
		
}

void timecall_test(void *aram)
{
	create_time_1();
	start_time_1();
	while(1);
	
}


//void timeCallBackTask(void)
//{
//	//static int i=0;
//	printf_1("this is timecall_1\r\n");
//	
//  //OLED_PrintSignedVal(4, 4, i++);
//	//	xTaskNotify(esp8266_init_Handle,(uint32_t)10,eSetValueWithoutOverwrite);
//	/*
//	如果定时时间到了，就会用任务调度来发一个10，表示已经超时
//	*/
//	//xTaskNotify(esp8266_init_Handle,(uint32_t)10,eSetValueWithoutOverwrite);
//}
