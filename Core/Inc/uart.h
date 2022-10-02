#include <stdint.h>
#include <stdlib.h>
#include "ncerr.h"
#include "cmsis_os.h"
#include "stm32f4xx_hal.h"

#define BUFF_SIZE 64
#define UART_LOG_SIZE 512

#ifndef CORE_INC_UART_H_
#define PROGRAM_INC_UARTCPP_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "main.h"

 void recieve_fsm(UART_HandleTypeDef *pHandle);

#ifdef __cplusplus
}
#endif



HAL_StatusTypeDef   send_uart_message(UART_HandleTypeDef *pHandle, const char* mes);
HAL_StatusTypeDef   get_uart_message(UART_HandleTypeDef *pHandle, uint8_t* pBuff, size_t msgSize, uint8_t print);
// HAL_StatusTypeDef   send_rec_uart_message(UART_HandleTypeDef *pHandle, uint8_t* pBuff, size_t msgSize, const char* msg);

uint8_t             is_ret_msg_valid(uint8_t *pBuff, const char* msg);
void reset_scratch_pointer();
void print_used_scratch_pointer();

extern const uint8_t* pScratch;

#endif /* PROGRAM_INC_UARTCPP_H_ */
