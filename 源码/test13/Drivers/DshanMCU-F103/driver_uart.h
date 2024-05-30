
#ifndef _DRIVER_UART_H
#define _DRIVER_UART_H
#include <stdint.h>

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
#define printf_1  print_usart1
#define printf_2	print_usart2
#define printf_3  print_usart3

#define rece_1   rece_usart1
#define rece_2   rece_usart2
#define rece_3   rece_usart3

#define DEBUG_UART_TIMEOUT 500
#define UART_RX_QUEUE_LEN 100

/*用于发给8266的函数*/
#define ESP8266_send print_usart2

/*
这里创建三个结构体，这三个结构体用于接受数据，
用定义的receiveData_uart1来控制接受数组的大小
*/

struct g_rots_RxQueue
{
	char res[UART_RX_QUEUE_LEN];
	int i;
};

struct UART_Device {
    char *name;
    int (*Init)(struct UART_Device *pDev, int baud, int datas, char parity, int stop);
	  int (*Send)(struct UART_Device *pDev, uint8_t *datas, int len, int timeout_ms);
	  int (*Recv)(struct UART_Device *pDev, void *para, int timeout_ms);
    void *priv_data;
};

void print_usart1(char *format, ...) ;
void print_usart2(char *format, ...) ;
void print_usart3(char *format, ...) ;
int rece_usart1(void *para,int aTime);
int rece_usart2(void *para,int aTime) ;
int rece_usart3(void *para,int aTime);
struct UART_Device *GetUARTDevice(char *name);



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


#endif /* _DRIVER_UART_H */

