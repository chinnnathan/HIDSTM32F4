#include <stdint.h>
#include <stdlib.h>
#include "ncerr.h"
#include "cmsis_os.h"
#include "stm32f4xx_hal.h"

#define PBUFF_SIZE 32

#ifndef CORE_INC_BT_API_H_
#define PROGRAM_INC_BTAPI_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "main.h"
#define SIZE_REMOTE 14
#define NUM_REMOTE_OPTIONS 3

typedef enum {
	BT_ENTER_COMMAND_MODE,
	BT_ENTER_SPP_MODE,
	BT_GET,
	BT_SET,
	BT_TRY_CONNECT,
	BT_RESET
} btCommand;

const char * get_remote_address();
const char * get_desired_remote_address();

err connect_and_enter_hid(UART_HandleTypeDef* pHandle);
err bt_do_wiggle(void);

err is_bt_module_connected(UART_HandleTypeDef* pHandle);

void switch_remote(uint16_t sel);

#ifdef __cplusplus
}
#endif

#endif /* PROGRAM_INC_BTAPI_H_ */
