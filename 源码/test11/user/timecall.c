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
								(const char *)"task1",// ��ʱ������
								(TickType_t)TickType_time_1,// ��ʱ��������
								pdFALSE,// ��ʱ���ǵ���ִ�л�������ִ��, pdTRUEΪ����ִ�У�pdFALSEΪ����ִ��
								(void*)1,// ����ʱ�������ΨһID
								(TimerCallbackFunction_t)timeCallBackTask// ��ʱ���Ļص�����
								);


}


void start_time_1()
{
			if(xTimeHandle[0])
		{
			//printf_1("timecall_1 init success\r\n");
			xTimerStart(xTimeHandle[0],0); //������ʱ��
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
//	�����ʱʱ�䵽�ˣ��ͻ��������������һ��10����ʾ�Ѿ���ʱ
//	*/
//	//xTaskNotify(esp8266_init_Handle,(uint32_t)10,eSetValueWithoutOverwrite);
//}
