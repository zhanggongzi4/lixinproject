#include "uart_parse_task.h"
#include "FreeRTOS.h"
#include "cmsis_os.h"
#include "task.h"
#include "elog.h"

#include "stm32f4xx_hal.h"
#include "queue.h"
QueueSetHandle_t queue_irc_rec_A=NULL;

void uart_rec_A_func(void *argument)
{
  /* USER CODE BEGIN uart_rec_A_func */
  /* Infinite loop */
	queue_irc_rec_A=NULL;
	queue_irc_rec_A=xQueueCreate(1,4);
	if(NULL==queue_irc_rec_A)
	{
		log_i("queuecreate successed \r\n");
	}
	else
		log_i("queuecreate failed \r\n");
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END uart_rec_A_func */
}
