#include <stdlib.h>
#include <stdio.h>
#include "freertos_demo.h"
#include "cmsis_os.h"
#include "stm32f1xx_hal.h"
#include "FreeRTOS.h"                   // ARM.FreeRTOS::RTOS:Core
#include "task.h"                       // ARM.FreeRTOS::RTOS:Core
#include "event_groups.h"               // ARM.FreeRTOS::RTOS:Event Groups
#include "semphr.h"                     // ARM.FreeRTOS::RTOS:Core
#include "gpio.h"
#include "driver.h"
#include "tim.h"
/*进入低功耗之前，要做的事情*/
void PRE_SLEEP_PROCESSING(void)
{
	//printf("进入低功耗\r\n");
	//HAL_SuspendTick(); 
	__HAL_RCC_GPIOA_CLK_DISABLE();
	__HAL_RCC_GPIOB_CLK_DISABLE();
	__HAL_RCC_GPIOC_CLK_DISABLE();
	
}
/*退出低功耗后，要做的事情*/
void POST_SLEEP_PROCESSING(void)
{
	//HAL_ResumeTick();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();

}