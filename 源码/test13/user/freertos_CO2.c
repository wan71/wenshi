#include "driver.h"
#include "freertos_user.h"

void uart3_test(void *argument)
{
	SD_ON;
	Sensors_on();
	struct g_rots_RxQueue uart3;
	struct UART_Device *pUARTDev_3 = GetUARTDevice("stm32_dma_uart3");
	pUARTDev_3->Init(pUARTDev_3, 115200, 8, 'N', 1);
	vTaskDelay(3000);
	printf_1("uart3_test init success\r\n");
	while(1)
	{
  	rece_3(&uart3,portMAX_DELAY);
	
		//printf_1("this is %s\r\n",uart3.res);
		printf_1("%d %d %d %d %d %d\r\n",uart3.res[0],uart3.res[1],uart3.res[2],uart3.res[3],uart3.res[4],uart3.res[5]);
	
	}
	
	
}
