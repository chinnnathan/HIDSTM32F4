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
//#define GET_INQ 1
//#define GET_MODE 1

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
    const char comp1[] = "SR,001A7DDA7115\r";
    const char comp2[] = "SR,60E32B7AB47F\r";
    const char comp3[] = "SR,A0A4C5965153\r";

    const char* connect[] = {
    "C,001A7DDA7115\r",
    "C,60E32B7AB47F\r",
    "C,A0A4C5965153\r",
    };
    /*
    A0A4C5965153,NCHINN-MOBL2,2A010C
    60E32B7AB47F,AP-J5MN2J3,2A010C
    001A7DDA7115,MAIN,2A0104
    Inquiry Done*/
    // const char comp1[] = "C,001A7DDA7115\r";
    // const char comp2[] = "C,3800251D32D2\r";
    // const char comp3[] = "C,60E32B7AB47F\r";
    // const char comp4[] = "C,A0A4C5965153\r";
    RxTxMachine uartMachine(uartCmdr1.pHandle, uartCmdr1.pBuff, RxRecieveResponse);

    const char* remotes[] = {comp1, comp2, comp3};
    uint16_t remoteIdx = 0;
    uint8_t updateRemote = 0;

    char remote[SIZE_REMOTE];
}



void FlashLed(GPIO_TypeDef* gpio, uint16_t gpioPin)
{
  for(uint16_t i = 0; i < 10; i++)
  {
    HAL_GPIO_TogglePin(gpio, gpioPin);
    osDelay(50);
  }
}

void switch_remote(uint16_t sel)
{
	if (sel & 0x0004)
    {
        remoteIdx = 2;
    }
    else if (sel & 0x0002)
    {
        remoteIdx = 1;
    }
    else if (sel & 0x0001)
    {
        remoteIdx = 0;
    }
//    remoteIdx = (remoteIdx >= NUM_REMOTE_OPTIONS-1) ? 0 : remoteIdx++;
    updateRemote = 1;
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

void set_bt_state(btState state)
{
    uartCmdr1.state = state;
}

char * get_remote_address()
{
    return remote;
    // auto status = uartMachine.send_rec_val_uart_message("$$$", "CMD\r\n");

    // char* retval = uartMachine.send_rec_uart_message(14, "GR\r");

    // uartMachine.send_uart_message("---\r");
    // return retval;
}

char * get_desired_remote_address()
{
    char* retVal = (char*)remotes[remoteIdx];
    retVal += 3; // get past SR,
    return retVal;
}

uint8_t update_remote()
{
    if (updateRemote == 1)
    {
        updateRemote = 0;
        return 1;
    }
}

err bt_start_task(UART_HandleTypeDef* pHandle)
{
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
#ifdef GET_INQ
        uartMachine.print_uart();
#endif
        break;
    case BT_READY:
        status = enterDeviceSearch(pHandle, pUartCmder);
        if (status == NC_NO_ERROR)
        {
            pUartCmder->state = BT_PRINT_UART;
            pUartCmder->pendingState = BT_ENTER_HID;
//            pUartCmder->pendingState = BT_CONNECT;
        }
        break;
    case BT_SEARCH_CONNECT:
    	// uartMachine.send_uart_message("K,\r");
		// osDelay(100);
        // uartMachine.send_uart_message(remotes[remoteIdx]);
		// status = resetRN42(pUartCmder);
		// if (status == NC_NO_ERROR)
		{
			pUartCmder->state = BT_CMD_MODE;
			pUartCmder->pendingState = BT_CONNECT;
		}
        break;
    case BT_CONNECT:
        uartMachine.send_rec_uart_message(connect[remoteIdx], "\r\n");
        status = enterDeviceSearch(pHandle, pUartCmder);

        uartMachine.send_rec_uart_message("GM\r", "\r\n");
        uartMachine.send_rec_uart_message("GK\r", "\r\n");
        osDelay(100);
        FlashLed();
        uartMachine.send_rec_uart_message("C\r", "\r\n");
//        uartMachine.send_uart_message("---\r");

        uartMachine.send_uart_message("---\r");
        pUartCmder->state = BT_HID_MODE;
        break;
    case BT_ENTER_HID:
        status = enterHID(pHandle, pUartCmder);
        if (status == NC_SUCCESS)
        {
        	pUartCmder->state = BT_CMD_MODE;
        	pUartCmder->pendingState = BT_CONNECT;
        }
        break;
    case BT_HID_MODE:
        // HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_7);
        osDelay(1000);
        status = NC_NO_ERROR;
        break;
    default:
        HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_7);
        osDelay(250);
        break;
    }

    return status;
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
    uartMachine.reset_buffer_ptr();
    return NC_NO_ERROR;
}

err enterHID(UART_HandleTypeDef *pHandle, btCommander *pUartCmder)
{
    const char* recArray[] = {
        "AOK\r\n", "AOK\r\n", "AOK\r\n", "Reboot!\r\n"
    };
    const char* issueArray[] = {
        "S~,6\r", "SH,0220\r", "SM,6\r", "R,1\r"
    };
    err ret = NC_NO_ERROR;

    ret = uartMachine.send_rec_val_uart_message(remotes[remoteIdx], "AOK\r\n");

    for (uint16_t i = 0; i < 4; i++)
    {
        ret = uartMachine.send_rec_val_uart_message(issueArray[i], recArray[i]);
        if (ret != NC_SUCCESS)
        {
            printf("Failed issue: %s : %s\n", issueArray[i], recArray[i]);
//            return ret;
        }
    }

    osDelay(500);
    uartMachine.send_uart_message("---\r");

    FlashLed();
    return ret;    
}

err enterDeviceSearch(UART_HandleTypeDef* pHandle, btCommander* pUartCmder)
{   
//    char* tv = uartMachine.send_rec_uart_message(SIZE_REMOTE, "GR\r");
    char* tv = uartMachine.send_rec_uart_message("GR\r", "\r\n");

    memcpy(remote, tv, SIZE_REMOTE);

#ifdef GET_INQ
    osDelay(500);
    uartMachine.send_rec_print_uart("I,10\r", "Done\r\n");
    osDelay(15000);
#endif
#ifdef GET_MODE
    osDelay(500);
	uartMachine.send_rec_print_uart("GM\r", "\r\n");
	osDelay(15000);
#endif

    return NC_NO_ERROR;
}

err mouseWiggle(btCommander* pUartCmder)
{
    UART_HandleTypeDef* pHandle = pUartCmder->pHandle;

	for(auto i = 0; i < 10; i++)
	{
		mouse_command(pHandle, 0,10,10);
		osDelay(50);
	}
	for(auto i = 0; i < 10; i++)
	{
		mouse_command(pHandle, 0,-10,-10);
		osDelay(50);
	}


    return NC_NO_ERROR;
}
