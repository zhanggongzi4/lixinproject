#include "bsp_uart_driver.h"
#include "FreeRTOS.h"
#include "cmsis_os.h"
#include "task.h"

#include "usart.h"
#include "stm32f4xx_hal.h"
#include "queue.h"

#include "elog.h"
#include "mid_circular_buffer.h"


#define BUFFER_A 0
#define BUFFER_B 1

extern QueueSetHandle_t queue_irc_rec_A;


uint8_t flag_AB=BUFFER_A; 

uint8_t g_data_bufferA[1]={0x00};
uint8_t g_data_bufferB[1]={0x00};
void uart_driver_fuc(void *argument)
{
	 circular_buffer_t *p_circular_buffer = create_empty_circular_buffer();
	 if(NULL==p_circular_buffer)
	 {
			log_e("circular_buffer create failed");
			vTaskDelete(NULL);
			return;
	 }
	 log_i("circular_buffer create success");
	 
	  if(0x00==buffer_is_empty(p_circular_buffer))
	 {
		  log_i("buffer_is_empty");
	 }
	 if(0x00==buffer_is_full(p_circular_buffer))
	 {
		  log_i("buffer_is_full");
	 }
	     
    if ( 0x00 == insert_data(p_circular_buffer ,15))
    {
        log_i("buffer_inster_success");
        
    }
    uint8_t temp_data = 0;
    if ( 0x00 == get_data(p_circular_buffer,&temp_data))
    {
        log_i("buffer_get_success");
    }
    log_i("buffer_read_out = [%d]",temp_data );
    
    if ( 0x00 == buffer_is_empty(p_circular_buffer))
    {
        log_i("buffer_is_empty");
    }
    if ( 0x00 == buffer_is_full(p_circular_buffer) )
    {
        log_i("buffer_is_full");
    }
    
	
	 flag_AB=BUFFER_A;
	 HAL_StatusTypeDef ret =HAL_OK;
	 ret = HAL_UART_Receive_IT(&huart1, g_data_bufferA,1);
	  
	
	 if(HAL_OK==ret)
	 {
	  log_i("hal uart init success");
	 }
		else
		{
		log_i("hal_init failed");
		}
		
  for(;;)
	{
		osDelay(1);
	}


}
	


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  /* Prevent unused argument(s) compilation warning */
  
  /* NOTE: This function should not be modified, when the callback is needed,
           the HAL_UART_RxCpltCallback could be implemented in the user file
   */
  log_d("HAL_UART_RxCpltCallback");
	
	HAL_StatusTypeDef ret =HAL_OK;
	
	if (BUFFER_A==flag_AB)
	{
	 log_d("g_data_buffer_A=[%c]",g_data_bufferA[0]);
	 ret = HAL_UART_Receive_IT(&huart1, g_data_bufferB,1);
	 if(HAL_OK!=ret)
	 {
		 log_d("HAL_Receive_IT error");
	 }
	 flag_AB=BUFFER_B;
		
	}
	
	else
	{
		log_d("g_data_buffer_B=[%c]",g_data_bufferB[0]);
		ret = HAL_UART_Receive_IT(&huart1, g_data_bufferA,1);
		 if(HAL_OK!=ret)
	 {
		 log_d("HAL_Receive_IT error");
	 }
	 flag_AB=BUFFER_A;
	}
}
/* USER CODE END 1 */
