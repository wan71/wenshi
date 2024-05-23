#ifndef _FREERTOS_UART_H
#define _FREERTOS_UART_H

struct uart_task{
	char res[10];
	int used_num;
};



void InputTask(void *params);
void UART2_READ_Task(void *argument);

#endif /* _DRIVER_UART_H */