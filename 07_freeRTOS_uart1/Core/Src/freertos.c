/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include <stdlib.h>


#define BUFFER_SIZE 1

uint32_t *buffer1=NULL;
uint32_t *buffer2=NULL;
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdlib.h>
#include "queue.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
extern ADC_HandleTypeDef hadc1;
extern DMA_HandleTypeDef hdma_adc1;
/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
	buffer1 =(uint32_t*)malloc(sizeof(uint32_t)*BUFFER_SIZE);
	buffer2 =(uint32_t*)malloc(sizeof(uint32_t)*BUFFER_SIZE);
	if(buffer1==NULL)
	{
		printf("buffer1 malloc failed \r\n");
		return;
	}
		if(buffer2==NULL)
	{
		printf("buffer2 malloc failed \r\n");
		return;
	}
	memset(buffer1,0xff,(sizeof(uint32_t)*BUFFER_SIZE));
	memset(buffer2,0xff,(sizeof(uint32_t)*BUFFER_SIZE));
	printf("Unit TEST ADC+DMA\r\n");
	
	HAL_StatusTypeDef ret = HAL_OK;
	
	ret=HAL_ADC_Start_DMA(&hadc1,buffer1,BUFFER_SIZE);
	
	if(HAL_OK!=ret)
	{
		printf("HAL_ADC call  failed \r\n");
	}
	//queue test
	 QueueHandle_t xQueue1 =NULL;
	 xQueue1= xQueueCreate( 10, 4);//总长和每个单位长度 创建
	if(NULL==xQueue1)
	{
		printf("Queue create failed \r\n");
		return;
	}
	
	uint32_t queue_data_1=123;
	
	BaseType_t ret_1=pdPASS;
	
	ret_1=xQueueSend( xQueue1,&queue_data_1, 0 ); //发送
	
	printf("send=%ld \r\n",ret_1);
	
	ret_1=pdPASS;	
	
	
	uint32_t queue_data_2 =0xff;
	
	ret_1=xQueueReceive(xQueue1,&queue_data_2, 0 );//接收
	
	printf("receive=%ld \r\n",ret_1);
	
	printf("queue_data_2=%d \r\n",queue_data_2);
	
	
	
  for(;;)
  {
    printf("hello world \r\n");
		
		printf("buffer data =%d\r\n",buffer1[0]);
    osDelay(1000);
  }
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

__weak void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)//这里是创建DMA回调函数进行中断打印
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hadc);
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_ADC_ConvCpltCallback could be implemented in the user file
   */
	printf("buffer data =%d\r\n",buffer1[0]);
}
__weak void HAL_ADC_ErrorCallback(ADC_HandleTypeDef *hadc)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hadc);
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_ADC_ErrorCallback could be implemented in the user file
   */
	printf("ADC transfer error \r\n");
}
