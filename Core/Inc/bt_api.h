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

 err bt_start_task(UART_HandleTypeDef *pHandle);
 uint8_t bt_do_wiggle(void);

 void FlashLed(GPIO_TypeDef* gpio, uint16_t gpioPin);

typedef enum {
	BT_READY, 
    BT_INITIALIZED, 
    BT_PRINT_UART,
    BT_CMD_MODE,
    BT_SEARCH_CONNECT,
    BT_CONNECT,
    BT_HID_MODE, 
    BT_SPP_MODE,
    BT_ENTER_HID,
    BT_ENTER_SPP,
    BT_INVALID
} btState;

typedef struct btCommander {
	btState state;
    btState pendingState;
	UART_HandleTypeDef *pHandle;
	uint8_t pBuff[PBUFF_SIZE];
} btCommander;

btState get_bt_state();
void    set_bt_state(btState);
char * get_remote_address();
char * get_desired_remote_address();

void switch_remote(uint16_t sel);
uint8_t update_remote();


#ifdef __cplusplus
}
#endif

#endif /* PROGRAM_INC_BTAPI_H_ */





void mouse_command(UART_HandleTypeDef* pHandle, uint8_t buttons, uint8_t x, uint8_t y);

err resetRN42(btCommander* pUartCmder);
err enterCmdModeValidate(UART_HandleTypeDef *pHandle, btCommander *pUartCmder);
err enterDeviceSearch(UART_HandleTypeDef *pHandle, btCommander *pUartCmder);
err enterHID(UART_HandleTypeDef *pHandle, btCommander *pUartCmder);
err mouseWiggle(btCommander* pUartCmder);
