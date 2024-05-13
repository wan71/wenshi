/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "rtc.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>


/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
enum LowPower
{
	SLEEP=1,STOP,STANDBY //��������ģʽ��˯�ߣ�ֹͣ������
};
RTC_AlarmTypeDef aTime;
RTC_TimeTypeDef nTime;
extern RTC_HandleTypeDef hrtc;
RTC_TimeTypeDef GetTime;   //��ȡʱ��ṹ��
RTC_DateTypeDef GetData;  //��ȡ���ڽṹ��
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define RXBUFFERSIZE  256     //�������ֽ���
#define  LED_ON  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET)
#define  LED_OFF  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET)
#define  PowerTime 90
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
char RxBuffer[RXBUFFERSIZE];   //��������
uint8_t aRxBuffer;			//�����жϻ���
uint8_t Uart1_Rx_Cnt = 0;		//���ջ������
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
 
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_RTC_Init();
  /* USER CODE BEGIN 2 */
	HAL_UART_Receive_IT(&huart1, (uint8_t *)&aRxBuffer, 1);
	enum LowPower mode;
	 
	aTime.Alarm = 0;
	aTime.AlarmTime.Hours = 9;
	aTime.AlarmTime.Minutes = 41;
	aTime.AlarmTime.Seconds = 0;
	HAL_RTC_SetAlarm_IT(&hrtc,&aTime,RTC_FORMAT_BIN);
	 
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		//mode=SLEEP;  //ѡ��˯��ģʽ
	   mode=STOP;  //ѡ��ֹͣģʽ
	  //mode=STANDBY;  //ѡ�����ģʽ
		
	  LED_ON;
		HAL_Delay(500);
	
		switch(mode)
		{
			case SLEEP:
							printf("����˯��ģʽ\r\n");
							HAL_Delay(500);
							HAL_SuspendTick();	//ֹͣϵͳ�δ��ʱ��
							HAL_PWR_EnterSLEEPMode(PWR_LOWPOWERREGULATOR_ON,PWR_SLEEPENTRY_WFI);  //����˯��ģʽ
							HAL_ResumeTick();        //�ͷŵδ�ʱ��
				      HAL_Delay(500);  //�ȴ��δ�ʱ���ͷ�
							printf("�˳�˯��ģʽ\r\n");
							break;
			
			case STOP:
							printf("����ֹͣģʽ\r\n");
							HAL_Delay(500);
							HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON,PWR_SLEEPENTRY_WFI);//����ֹͣģʽ
							SystemClock_Config();     //��������ʱ��		
				      HAL_Delay(500);
							printf("�˳�ֹͣģʽ\r\n");
							/* Get the RTC current Time */
							HAL_RTC_GetTime(&hrtc, &GetTime, RTC_FORMAT_BIN);
							/* Get the RTC current Date */
							HAL_RTC_GetDate(&hrtc, &GetData, RTC_FORMAT_BIN);
							/* Display date Format : yy/mm/dd */
							printf("%02d/%02d/%02d\r\n",2000 + GetData.Year, GetData.Month, GetData.Date);
							/* Display time Format : hh:mm:ss */
							printf("%02d:%02d:%02d\r\n",GetTime.Hours, GetTime.Minutes, GetTime.Seconds);
							printf("\r\n");
							break;
			
			case STANDBY:
							printf("�������ģʽ\r\n");
							HAL_Delay(500);
							HAL_SuspendTick();	//ֹͣϵͳ�δ��ʱ��
							HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);  //���û������� PA0
							HAL_PWR_EnterSTANDBYMode();//�������ģʽ  
							//�˳�����ģʽ��������¿�ʼִ�У����к���û����
							break;			
			default:
				break;
				
		
		}
		LED_OFF;
		HAL_Delay(500);
	
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}



//�ص�����
void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *nhrtc) //�����Ҫȡ����������hrtc��ͻ
{
  //HAL_RTC_GetTime(nhrtc,&nTime,RTC_FORMAT_BIN);
 // printf("Alarm--%d:%d:%d\r\n",nTime.Hours,nTime.Minutes,nTime.Seconds);
 
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

  HAL_RTC_SetAlarm_IT(&hrtc,&aTime,RTC_FORMAT_BIN);
//---------
	}

/* USER CODE BEGIN 4 */
 
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(huart);
  /* NOTE: This function Should not be modified, when the callback is needed,
           the HAL_UART_TxCpltCallback could be implemented in the user file
   */
	
 
	if(Uart1_Rx_Cnt >= 255)  //����ж�
	{
		Uart1_Rx_Cnt = 0;
		memset(RxBuffer,0x00,sizeof(RxBuffer));
		HAL_UART_Transmit(&huart1, (uint8_t *)"�������", 10,0xFFFF); 	
        
	}
	else
	{
		RxBuffer[Uart1_Rx_Cnt++] = aRxBuffer;   //��������ת��
	
		if((RxBuffer[Uart1_Rx_Cnt-1] == 0x0A)&&(RxBuffer[Uart1_Rx_Cnt-2] == 0x0D)) //�жϽ���λ
		{
			HAL_UART_Transmit(&huart1, (uint8_t *)&RxBuffer, Uart1_Rx_Cnt,0xFFFF); //���յ�����Ϣ���ͳ�ȥ
            while(HAL_UART_GetState(&huart1) == HAL_UART_STATE_BUSY_TX);//���UART���ͽ���
			Uart1_Rx_Cnt = 0;
			memset(RxBuffer,0x00,sizeof(RxBuffer)); //�������
		}
	}
	
	HAL_UART_Receive_IT(&huart1, (uint8_t *)&aRxBuffer, 1);   //�ٿ��������ж�
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if( GPIO_Pin == switch_interrupt_Pin)//�ж��ⲿ�ж�Դ
	{
		HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);//��תPA5״̬
	}
}



/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
