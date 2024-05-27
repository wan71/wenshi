#include "driver.h"
#include "freertos_user.h"

void uart3_test(void *argument)
{
	struct g_rots_RxQueue uart3;
	struct UART_Device *pUARTDev_3 = GetUARTDevice("stm32_dma_uart3");
	pUARTDev_3->Init(pUARTDev_3, 115200, 8, 'N', 1);
	printf_3("uart3_test init success\r\n");
	while(1)
	{
  	rece_3(&uart3,portMAX_DELAY);
	
		printf_3("this is %s\r\n",uart3.res);
	}
	
	
}
