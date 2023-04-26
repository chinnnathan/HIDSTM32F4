#include <stdint.h>
#include <stdlib.h>
#include <uart.h>
#include "ncerr.h"
#include "cmsis_os.h"
#include "stm32f4xx_hal.h"

#ifndef PROGRAM_INC_TASKS_H_
#define PROGRAM_INC_TASKS_H_

#ifdef __cplusplus
 extern "C" {
#endif


err enter_uart_task(UART_HandleTypeDef *pHandle);

#ifdef __cplusplus
}
#endif

#endif /* PROGRAM_INC_TASKS_H_ */
