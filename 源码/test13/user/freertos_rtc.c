
/*
RTCʱ�Ӳο���https://blog.csdn.net/as480133937/article/details/105741893
���ǣ�û��ʹ�� tamper��PC13�����������У����������ʱ�ӡ�
*/

#include "driver.h"
#include "freertos_user.h"
#include "freertos_rtc.h"

RTC_AlarmTypeDef aTime;
RTC_DateTypeDef GetData;  //��ȡ���ڽṹ��
RTC_TimeTypeDef GetTime;   //��ȡʱ��ṹ��
extern RTC_HandleTypeDef hrtc;
extern TaskHandle_t oled_Handle;


void RTC_Task(void *argument)
{
	

	while(1)
	{
		TickType_t xLastWakeTime;
		 xLastWakeTime = xTaskGetTickCount();
		  /* Get the RTC current Time */
	   HAL_RTC_GetTime(&hrtc, &GetTime, RTC_FORMAT_BIN);
      /* Get the RTC current Date */
      HAL_RTC_GetDate(&hrtc, &GetData, RTC_FORMAT_BIN);

      /* Display date Format : yy/mm/dd */
      printf_1("%02d/%02d/%02d\r\n",2000 + GetData.Year, GetData.Month, GetData.Date);
		
      /* Display time Format : hh:mm:ss */
      printf_1("%02d:%02d:%02d\r\n",GetTime.Hours, GetTime.Minutes, GetTime.Seconds);

      printf_1("\r\n");
		 // HAL_Delay(1000);
			vTaskDelayUntil( &xLastWakeTime, 1000);


	}

}
	
void Alarm_set(uint8_t Hours,uint8_t Minutes,uint8_t Seconds)
{
	
	 //��������
	aTime.Alarm = 0;
	aTime.AlarmTime.Hours = Hours;
	aTime.AlarmTime.Minutes = Minutes;
	aTime.AlarmTime.Seconds = Seconds;
	HAL_RTC_SetAlarm_IT(&hrtc,&aTime,RTC_FORMAT_BIN);
}





//�ص�����
void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *nhrtc) //�����Ҫȡ����������hrtc��ͻ
{

	static int m;
//���µ�������ʵ��ÿ���1������Ӧһ���ж�--------------------
	int a=PowerTime+aTime.AlarmTime.Seconds;
	while(a>=60)
	{
		aTime.AlarmTime.Minutes+=1;
		a-=60;
		 if(aTime.AlarmTime.Minutes==60)
    {
      aTime.AlarmTime.Hours += 1;
	  if(aTime.AlarmTime.Hours == 24)
	    {
	      aTime.AlarmTime.Hours = 0;
	    }
      aTime.AlarmTime.Minutes=0;
    }
	}
	aTime.AlarmTime.Seconds=a%60;
  printf_1("Alarm--%d:%d:%d��m--%d\r\n",aTime.AlarmTime.Hours,aTime.AlarmTime.Minutes,aTime.AlarmTime.Seconds,m);

	
//	OLED_PrintSignedVal(0, 4, aTime.AlarmTime.Hours);
//	OLED_PrintString(2, 4, "/");
//	OLED_PrintSignedVal(3, 4, aTime.AlarmTime.Minutes);
//	OLED_PrintString(5, 4, "/");
//	OLED_PrintSignedVal(6, 4, aTime.AlarmTime.Seconds);
	m++;
//	OLED_PrintSignedVal(0, 4, m);
  HAL_RTC_SetAlarm_IT(&hrtc,&aTime,RTC_FORMAT_BIN);
	
}


	




















////�ص�����
//void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *nhrtc) //�����Ҫȡ����������hrtc��ͻ
//{
//  HAL_RTC_GetTime(nhrtc,&GetTime,RTC_FORMAT_BIN);
//  printf("Alarm--%d:%d:%d\r\n",GetTime.Hours,GetTime.Minutes,GetTime.Seconds);
// 
////���µ�������ʵ��ÿ���1������Ӧһ���ж�--------------------
//  GetTime.Minutes+=1;
//  if(GetTime.Minutes==60)
//    {
//      GetTime.Hours += 1;
//	  if(GetTime.Hours == 24)
//	    {
//	      GetTime.Hours = 0;
//	    }
//      GetTime.Minutes=0;
//    }
//  aTime.AlarmTime = GetTime;


//  HAL_RTC_SetAlarm_IT(&hrtc,&aTime,RTC_FORMAT_BIN);

//	}












//	/*����ϵͳʱ��*/
//	HAL_StatusTypeDef HAL_RTC_SetTime(RTC_HandleTypeDef *hrtc, RTC_TimeTypeDef *sTime, uint32_t Format) 
//	/*��ȡϵͳʱ��*/
//	HAL_StatusTypeDef HAL_RTC_GetTime(RTC_HandleTypeDef *hrtc, RTC_TimeTypeDef *sTime, uint32_t Format)
//	/*����ϵͳ����*/
//	HAL_StatusTypeDef HAL_RTC_SetDate(RTC_HandleTypeDef *hrtc, RTC_DateTypeDef *sDate, uint32_t Format)
//	/*��ȡϵͳ����*/
//	HAL_StatusTypeDef HAL_RTC_GetDate(RTC_HandleTypeDef *hrtc, RTC_DateTypeDef *sDate, uint32_t Format)
//	/*������������*/
//	HAL_StatusTypeDef HAL_RTC_SetAlarm(RTC_HandleTypeDef *hrtc, RTC_AlarmTypeDef *sAlarm, uint32_t Format)
//	/*���ñ����ж�*/
//	HAL_StatusTypeDef HAL_RTC_SetAlarm_IT(RTC_HandleTypeDef *hrtc, RTC_AlarmTypeDef *sAlarm, uint32_t Format)
//	/*����ʱ��ص�����*/
//	__weak void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc)
//	/*д��󱸴�����*/
//	void HAL_RTCEx_BKUPWrite(RTC_HandleTypeDef *hrtc, uint32_t BackupRegister, uint32_t Data)
//	/*��ȡ�󱸴�����*/
//	uint32_t HAL_RTCEx_BKUPRead(RTC_HandleTypeDef *hrtc, uint32_t BackupRegister  

