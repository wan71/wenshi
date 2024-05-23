
#ifndef _DRIVER_UART_H
#define _DRIVER_UART_H
#include "FreeRTOS.h"
#include "semphr.h"
/**********************************************************************
 * 函数名称： UART_Init
 * 功能描述： UART初始化函数
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 0 - 成功, 其他值 - 失败
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/05	     V1.0	  韦东山	      创建
 ***********************************************************************/


//#define RXBUFFERSIZE  256     //最大接收字节数
#define printf_1 print_usart1
#define printf_2 print_usart2
#define printf_3 print_usart3
#define DEBUG_UART_TIMEOUT 500


/*用于发给8266的函数*/
#define ESP8266_send print_usart2

/*
这里创建三个结构体，这三个结构体用于接受数据，
用定义的receiveData_uart1来控制接受数组的大小
*/
#define receiveData_uart1  256
#define receiveData_uart2  256  
#define receiveData_uart3  16


typedef struct{
	char aRxBuffer[receiveData_uart1];  //接收数据
	int i;
	SemaphoreHandle_t uart1_tx;
}driver_uart_1;
	
typedef struct{
	char aRxBuffer[receiveData_uart2];  //接收数据
	int i;
	SemaphoreHandle_t uart2_tx;
}driver_uart_2;

typedef struct{
	char aRxBuffer[receiveData_uart3];  //接收数据
	int i;
}driver_uart_3;




int stm32_uart_init(driver_uart_1 *pDev);
void UART_Init(void);
void print_usart1(char *format, ...);
void print_usart2(char *format, ...);
void print_usart3(char *format, ...);

/**********************************************************************
 * 函数名称： UART_Test
 * 功能描述： UART测试程序
 * 输入参数： 无
 * 输出参数： 无
 *            无
 * 返 回 值： 0 - 成功, 其他值 - 失败
 * 修改日期        版本号     修改人        修改内容
 * -----------------------------------------------
 * 2023/08/05        V1.0     韦东山       创建
 ***********************************************************************/
void UART_Test(void *argument);

#endif /* _DRIVER_UART_H */

