#include <bt_api.h>
#include <bt.hpp>
#include <uart.h>
#include <uart_priv.hpp>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include "oled_io.h"
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
    RxTxMachine uartMachine;
    RN42 btModule(&uartMachine);
    std::vector<std::string> rm = { "001A7DDA7115", "60E32B7AB47F", "A0A4C5965153" };
    uint16_t remoteIdx = 0;
    /*
    A0A4C5965153,NCHINN-MOBL2,2A010C
    60E32B7AB47F,AP-J5MN2J3,2A010C
    001A7DDA7115,MAIN,2A0104
    Inquiry Done
    */
};

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

}

const char * get_remote_address()
{
    return btModule.remotes[remoteIdx].c_str();
    // auto status = uartMachine.send_rec_val_uart_message("$$$", "CMD\r\n");

    // char* retval = uartMachine.send_rec_uart_message(14, "GR\r");

    // uartMachine.send_uart_message("---\r");
    // return retval;
}

const char * get_desired_remote_address()
{
    return btModule.remotes[remoteIdx].c_str();
}

err connect_and_enter_hid(UART_HandleTypeDef* pHandle)
{
    err status = NC_NO_ERROR;

    switch (btModule.get_state())
    {
        case BTState::BT_INVALID:
            printf("Enter Invalid state\r\n");
            print_oled(OLED_BOTTOM, "BT Initialize");
            uartMachine.set_uart_handle(pHandle);
		    uartMachine.start_uart_stream();
            btModule.remotes = rm;
            btModule.set_next_state(BTState::BT_READY);
            status = btModule.process();
            status = NC_NO_ERROR;
        break;
        case BTState::BT_READY:
            printf("Enter Ready state\r\n");
            print_oled(OLED_BOTTOM, "BT Ready");
            btModule.set_next_state(BTState::BT_CMD_MODE);
            status = btModule.process();
            status = NC_NO_ERROR;
        break;
        case BTState::BT_GET_INFO:
            printf("Enter Info state\r\n");
            print_oled(OLED_BOTTOM, "BT Get Info");
        break;
        case BTState::BT_CMD_MODE:
            printf("Enter Command state\r\n");
            print_oled(OLED_BOTTOM, "BT Enter Cmd");
            btModule.set_next_state(BTState::BT_HID_MODE);
            status = btModule.process();
        break;
        case BTState::BT_HID_MODE:
            printf("Enter HID state\r\n");
            print_oled(OLED_BOTTOM, "BT HID Set");
            btModule.set_next_state(BTState::BT_CONNECT);
			status = btModule.process();
			if (status == NC_SUCCESS)
			{
				status = NC_COMPLETE;
			}
        break;
        case BTState::BT_SPP_MODE:
            printf("Enter SPP state\r\n");
        break;
        case BTState::BT_CONNECT:
		   printf("Enter Connect state\r\n");
           print_oled(OLED_BOTTOM, "BT Connect");
		   btModule.set_next_state(BTState::BT_READY);
		   status = btModule.process();
	    break;
    }

   return status;
}

err bt_do_wiggle(void)
{
    for(auto i = 0; i < 5; i++)
	{
        btModule.mouse_command(0,50,50);
		osDelay(50);
	}
	for(auto i = 0; i < 5; i++)
	{
        btModule.mouse_command(0,-50,-50);
		osDelay(50);
	}

    return NC_NO_ERROR;
}

err is_bt_module_connected()
{
    return btModule.is_connected();
}
