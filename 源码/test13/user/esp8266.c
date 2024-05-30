#include "driver.h"
#include "freertos_user.h"
#include "esp8266.h"

extern struct uart_task g_my_uart;

void chage(void *aram,char *res)
{
	struct uart_task *p=(struct uart_task *)aram;
	strcpy(p->res, res);
	p->used_num=10;
	
}