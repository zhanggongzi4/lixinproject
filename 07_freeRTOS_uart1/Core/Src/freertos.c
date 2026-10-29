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




/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdlib.h>
#include <string.h>
#include "queue.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
#define BUFFER_SIZE 1
#define DMA_CPLT_INT  0xA1
#define BUFFER1_READY 0X01
#define BUFFER2_READY 0X02
/* USER CODE BEGIN PTD */
uint32_t *buffer1=NULL;
uint32_t *buffer2=NULL;
QueueHandle_t xQueue1 =NULL;
QueueHandle_t xQueue2 =NULL;

uint32_t DMA_point=0;
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
osThreadId_t adc_ouputTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
const osThreadAttr_t adc_outputTask_attributes = {
  .name = "adc_outputTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void ADCTask(void *argument);
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
  xQueue2 = xQueueCreate(10, sizeof(uint32_t));
  if (xQueue2 == NULL)
  {
    Error_Handler();
  }
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);
	adc_ouputTaskHandle = osThreadNew(ADCTask, NULL, &adc_outputTask_attributes);

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
	DMA_point = 0;
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
	
	xQueue1= xQueueCreate( 10, 4);//总长和每个单位长度 创建要在传输到DMA之前不然 传完再创建
	//队列里就没有数据
	if(NULL==xQueue1)
	{
		printf("Queue1 create failed \r\n");
		return;
	}

	HAL_StatusTypeDef ret = HAL_OK;
	
	ret=HAL_ADC_Start_DMA(&hadc1,buffer1,BUFFER_SIZE);
	
	if(HAL_OK!=ret)
	{
		printf("HAL_ADC call  failed \r\n");
	}
	//queue test

	BaseType_t ret_1=pdPASS;
		
	uint32_t queue_data_2 =0xff;
	uint32_t queue2_pattern =BUFFER1_READY;
	
	
  for(;;)
  {
    printf("hello world \r\n");
		ret_1=xQueueReceive(xQueue1,&queue_data_2,portMAX_DELAY);//接收
		
	  printf("receive_ret_1=%ld \r\n",ret_1);
	
	  printf("receive=%d \r\n",queue_data_2);
  
	  if(0==DMA_point)	
		{

			queue2_pattern=BUFFER1_READY;
			ret_1=xQueueSend(xQueue2,&queue2_pattern,portMAX_DELAY);
			 printf("queue send to queue2 e=%ld \r\n",ret_1);

			HAL_ADC_Start_DMA(&hadc1,buffer2,BUFFER_SIZE);//这里是写完buffer1 再写buffer2
			DMA_point=1;
		}else
		{
			DMA_point=0;
			queue2_pattern=BUFFER2_READY;
			ret_1=xQueueSend(xQueue2,&queue2_pattern,portMAX_DELAY);
			printf("queue send to queue2 e=%ld \r\n",ret_1);

			HAL_ADC_Start_DMA(&hadc1,buffer1,BUFFER_SIZE);

		}
	
	
  }
  /* USER CODE END StartDefaultTask */
}
void ADCTask(void *argument)//解包线程
{
	printf("output thread \r\n");
	BaseType_t ret=pdPASS;
	uint32_t queue2_receive= BUFFER1_READY;

  for(;;)
  {

   ret=xQueueReceive(xQueue2,&queue2_receive,portMAX_DELAY);//接收

	  printf("receive_ret=%ld \r\n",ret);

	  printf("receive=%d \r\n",queue2_receive);
		if(BUFFER1_READY==queue2_receive)
		{
			printf("output buffer1 data=%d",buffer1[0]);
		}
		else
			printf("output  buffer2 data=%d",buffer2[0]);

  }
}
/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hadc);
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_ADC_ConvCpltCallback could be implemented in the user file
   */
	BaseType_t xHigherPriorityTaskWoken;
	xHigherPriorityTaskWoken = pdFALSE;
	
	uint32_t dma_pattern_cplt=DMA_CPLT_INT;
	
	xQueueSendFromISR(xQueue1, &dma_pattern_cplt, &xHigherPriorityTaskWoken);
	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);

	
}
void HAL_ADC_ErrorCallback(ADC_HandleTypeDef *hadc)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hadc);
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_ADC_ErrorCallback could be implemented in the user file
   */
	printf("ADC transfer error \r\n");
}
