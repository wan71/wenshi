#include "driver.h"
#include "freertos_user.h"
#include "freertos_rtc.h"


void uart1_test(void *argument)
{
	
		struct g_rots_RxQueue uart1;
	struct UART_Device *pUARTDev = GetUARTDevice("stm32_dma_uart1");
	pUARTDev->Init(pUARTDev, 115200, 8, 'N', 1);
	print_usart1("uart1_test init success\r\n");

  while(1)
  {
		 
		
		  rece_1(&uart1,portMAX_DELAY);
		  //print_usart1("this is %s\r\n",uart1.res);
	  
		
		 vTaskDelay(1000);

		
  }
	
	
}