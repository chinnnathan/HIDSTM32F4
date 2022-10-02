#include <bt_api.h>
#include <uart.h>
#include <uart_priv.hpp>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include "cmsis_os.h"
#include "ncerr.h"
#include "stm32f4xx_hal.h"

// http://ww1.microchip.com/downloads/en/DeviceDoc/bluetooth_cr_UG-v1.0r.pdf?_ga=2.234924162.2027616133.1648144359-2055943580.1648144359


#define UART_TX_TIMEOUT 1000
#define UART_RX_TIMEOUT 1000


namespace
{
    btCommander uartCmdr1 = { .state = BT_INITIALIZED };
}

static void FlashLed()
{
  for(uint16_t i = 0; i < 10; i++)
  {
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_7);
    osDelay(50);
  }
}

static void RxRecieveResponse(uint8_t* pBuff)
{
    if (uartCmdr1.state == BT_CMD_MODE)
    {
        uartCmdr1.state = uartCmdr1.pendingState;
        
    }
}

namespace
{
    const char comp1[] = "C,001A7DDA7115\r";
    const char comp2[] = "C,3800251D32D2\r";
    const char comp3[] = "C,60E32B7AB47F\r";
    RxTxMachine uartMachine(uartCmdr1.pHandle, uartCmdr1.pBuff, RxRecieveResponse);

    char* remote;
}



void FlashLed(GPIO_TypeDef* gpio, uint16_t gpioPin)
{
  for(uint16_t i = 0; i < 10; i++)
  {
    HAL_GPIO_TogglePin(gpio, gpioPin);
    osDelay(50);
  }
}



void mouse_command(UART_HandleTypeDef* pHandle, uint8_t buttons, uint8_t x, uint8_t y) 
{
    uint8_t buffer[] = { 0xFD, 0x05, 0x02, buttons, x, y, 0x00 };
    const uint8_t* pBuff = buffer;
    HAL_UART_Transmit(pHandle, pBuff, sizeof(buffer), UART_TX_TIMEOUT);
}

uint8_t bt_do_wiggle(void)
{
    if (uartCmdr1.state == BT_HID_MODE)
    {
        mouseWiggle(&uartCmdr1);
        return 1;
    }
    return 0;
}

btState get_bt_state()
{
    return uartCmdr1.state;
}

char * get_remote_address()
{
    return remote;
    // auto status = uartMachine.send_rec_val_uart_message("$$$", "CMD\r\n");

    // char* retval = uartMachine.send_rec_uart_message(14, "GR\r");

    // uartMachine.send_uart_message("---\r");
    // return retval;
}

void bt_start_task(UART_HandleTypeDef* pHandle)
{
    printf("Entered bt_start_task\n\r");
    btCommander* pUartCmder = &uartCmdr1;

    err status = NC_ERROR;

    switch (pUartCmder->state)
    {
    case BT_INITIALIZED:
    	set_active_machine(&uartMachine);
		uartMachine.set_uart_handle(pHandle);
		uartMachine.start_uart_stream();

    	uartCmdr1.pHandle = pHandle;
		status = resetRN42(pUartCmder);
		if (status == NC_NO_ERROR)
		{
			pUartCmder->state = BT_CMD_MODE;
            pUartCmder->pendingState = BT_READY;
		}
        break;
    case BT_CMD_MODE:
        status = enterCmdModeValidate(pHandle, pUartCmder);
        if (status == NC_SUCCESS)
        {
            pUartCmder->state = pUartCmder->pendingState;
        }
        break;
    case BT_PRINT_UART:
    	pUartCmder->state = pUartCmder->pendingState;
#ifdef DEBUG
        uartMachine.print_uart();
#endif
        break;
    case BT_READY:
        status = enterDeviceSearch(pHandle, pUartCmder);
        if (status == NC_NO_ERROR)
        {
            pUartCmder->state = BT_PRINT_UART;
            // pUartCmder->pendingState = BT_SEARCH_CONNECT;
            pUartCmder->pendingState = BT_CONNECT;
        }
        break;
    case BT_SEARCH_CONNECT:
        status = enterHID(pHandle, pUartCmder);
        if (status == NC_NO_ERROR)
        {
            pUartCmder->state = BT_CMD_MODE;
            pUartCmder->pendingState = BT_CONNECT;
        }
        break;
    case BT_CONNECT:
        uartMachine.send_uart_message(comp1);
        osDelay(100);
        uartMachine.send_uart_message("---\r");
        pUartCmder->state = BT_HID_MODE;
        printf("Moving to Idle Mode");
        break;
    case BT_HID_MODE:
        HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_7);
        osDelay(1000);
        break;
    default:
        HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_7);
        osDelay(250);
        break;
    }
}

err enterCmdModeValidate(UART_HandleTypeDef* pHandle, btCommander* pUartCmder)
{   
	return uartMachine.send_rec_val_uart_message("$$$", "CMD\r\n");
 }

err resetRN42(btCommander* pUartCmder)
{
	// Sometimes you just gotta powercycle the actual hardware.
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_1, GPIO_PIN_RESET);
	osDelay(500);
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_1, GPIO_PIN_SET);
    return NC_NO_ERROR;
}

err enterHID(UART_HandleTypeDef *pHandle, btCommander *pUartCmder)
{
    uartMachine.send_uart_message("S~,6\r");
    uartMachine.send_uart_message("SH,0220\r");
    uartMachine.send_uart_message("SM,0\r");
    uartMachine.send_uart_message("R,1\r");
    osDelay(500);
    uartMachine.send_uart_message("---\r");

    FlashLed();
    return NC_NO_ERROR;    
}

err enterDeviceSearch(UART_HandleTypeDef* pHandle, btCommander* pUartCmder)
{
    // enterCmdModeValidate(pHandle, pUartCmder);

	// get_uart_message(pHandle, pUartCmder->pBuff, BUFF_SIZE, 1);
	// uartMachine.send_uart_message("GR\r");
    
    remote = uartMachine.send_rec_uart_message(SIZE_REMOTE, "GR\r");

    // uartMachine.send_uart_message("C\r");
    // uartMachine.send_uart_message("---\r");
    
    // uartMachine.send_uart_message("C\r");
    // get_uart_message(pHandle, pUartCmder->pBuff, 1);
    // uartMachine.send_uart_message("---\r");
    // get_uart_message(pHandle, pUartCmder->pBuff, 1);


    return NC_NO_ERROR;
}

err mouseWiggle(btCommander* pUartCmder)
{
    UART_HandleTypeDef* pHandle = pUartCmder->pHandle;

	for(auto i = 0; i < 10; i++)
	{
		mouse_command(pHandle, 0,232,232);
		osDelay(50);
	}
	for(auto i = 0; i < 10; i++)
	{
		mouse_command(pHandle, 0,24,24);
		osDelay(50);
	}


    return NC_NO_ERROR;
}
