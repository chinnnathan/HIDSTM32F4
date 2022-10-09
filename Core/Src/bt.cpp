#include <bt.hpp>
#include <ncerr.h>
#include <stdio.h>
#include "stm32f4xx_hal.h"
#include "cmsis_os.h"

//*****************************************************************************
// RN42 Class
//*****************************************************************************

void RN42::mouse_command(uint8_t buttons, uint8_t x, uint8_t y)
{
    uint8_t buffer[] = { 0xFD, 0x05, 0x02, buttons, x, y, 0x00 };
    const char* pBuff = (const char*)buffer;
    for( uint8_t b : buffer)
    {
        this->uart->send_uart_byte(&b);
    }
    
}

void RN42::hard_reset()
{
    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_1, GPIO_PIN_RESET);
	osDelay(500);
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_1, GPIO_PIN_SET);
    this->uart->reset_buffer_ptr();
}

err RN42::soft_reset()
{
    return this->uart->send_rec_val_uart_message("R,1", "Reboot!\r\n");
}

std::string RN42::get_remote_connected()
{
    return this->uart->send_rec_uart_message("GR\r", "\r\n");
}

//*****************************************************************************
// BT Class
//*****************************************************************************

err RN42::process(msg* data)
{
    err status = NC_SUCCESS;

    if ( this->state != this->nextState )
    {
        status = this->state_enter(data);
        if (status != NC_SUCCESS)
            return status;
    }

    status = this->state_process(data);
    if (status != NC_SUCCESS)
            return status;

    status = this->state_end(data);
    if (status == NC_SUCCESS)
        this->state = this->nextState;
    
    return status;
}

err RN42::state_enter(msg* data)
{
    err status = NC_SUCCESS;

    switch (this->nextState)
    {
        case BTState::BT_INVALID:
            ;
        break;
        case BTState::BT_READY:
            this->hard_reset();
            status = NC_SUCCESS;
        break;
        case BTState::BT_CONNECT:
        case BTState::BT_GET_INFO:
        case BTState::BT_CMD_MODE:
        case BTState::BT_HID_MODE:
        case BTState::BT_SPP_MODE:
            if(this->state != BTState::BT_CMD_MODE)
                status = this->enter_command_mode();
        break;
    }
    return status;
}

err RN42::state_process(msg* data)
{
    err status = NC_SUCCESS;

    switch (this->nextState)
    {
        case BTState::BT_INVALID:
            status = NC_SUCCESS;
        break;
        case BTState::BT_READY:
            ;
        break;
        case BTState::BT_CONNECT:
            if (data)
            {
                std::string rmt((const char*)data->data, data->len);
                status = connect_new_remote(rmt);
            }
            else
            {
                status = connect_new_remote();
                // connect_stored_remote();
            }
        break;
        case BTState::BT_GET_INFO:
            ;
        break;
        case BTState::BT_CMD_MODE:
        break;
        case BTState::BT_HID_MODE:
            status = this->enter_hid_mode();
        break;
        case BTState::BT_SPP_MODE:
            status = this->enter_spp_mode();
        break;
    }
    return status;
}

err RN42::state_end(msg* data)
{
    err status = NC_SUCCESS;

    switch (this->nextState)
    {
        case BTState::BT_INVALID:
            status = NC_SUCCESS;
        break;
        case BTState::BT_READY:
            ;
        break;
        case BTState::BT_CONNECT:
            ;
        break;
        case BTState::BT_GET_INFO:
            ;
        break;
        case BTState::BT_CMD_MODE:
            ;
        break;
        case BTState::BT_HID_MODE:
            ;
        break;
        case BTState::BT_SPP_MODE:
            ;
        break;
    }
    return status;
}

template<uint16_t N>
err RN42::send_command_sequence(btCmd<N>* cmds, uint16_t interCmdDelay, std::string error)
{
    err status = NC_SUCCESS;
    for (auto i = 0; i < cmds->n; i++)
    {
        osDelay(interCmdDelay);
        status = this->uart->send_rec_val_uart_message( cmds->send[i], cmds->rec[i]);
        if (status != NC_SUCCESS)
        {
            if (this->uart->get_return_message(error).compare(error))
            {
                printf("Command not accepted: %s\n", cmds->send[i].c_str());
            }
        	printf("Failed at %s\n", cmds->send[i].c_str());
        }
    }
    return status;
}

err RN42::enter_command_mode()
{
    err status = this->send_command_sequence(&this->enterCmd, 1000);
    osDelay(1000);
    return status;
}

err RN42::enter_hid_mode()
{
    return this->send_command_sequence(&this->enterHID);
}

//*****************************************************************************
// BT Connection and Addressing
//*****************************************************************************
err RN42::connect_stored_remote()
{
    err status = this->uart->send_rec_val_uart_message( "C\r","TRYING\r\n");
    if (status != NC_SUCCESS)
	{
		printf("Failed at %s\n", this->uart->retstr.c_str());
	}
    return status;
}

err RN42::connect_new_remote()
{
    std::string connect = "C,";
    connect.append(this->remotes[0]);
    connect.append("\r");

    err status = this->uart->send_rec_val_uart_message( connect,"TRYING\r\n");
    if (status != NC_SUCCESS)
	{
		printf("Failed at %s\n", this->uart->retstr.c_str());
	}
    delete(&connect);
    return status;
}

err RN42::connect_new_remote(std::string str)
{
    std::string connect = "C,";
    connect.append(str.append("\r"));

    err status = this->uart->send_rec_val_uart_message( connect,"TRYING\r\n");
    if (status != NC_SUCCESS)
	{
		printf("Failed at %s\n", this->uart->retstr.c_str());
	}
    delete(&connect);
    return status;
}
