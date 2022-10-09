#include <stdint.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <uart_priv.hpp>
#include "ncerr.h"
#include "stm32f4xx_hal.h"

#ifndef CORE_INC_BT_HPP
#define CORE_INC_BT_HPP

enum class BTState : uint8_t{
    BT_INVALID,
    BT_READY,
    BT_CONNECT,
    BT_GET_INFO,
    BT_CMD_MODE,
    BT_HID_MODE,
    BT_SPP_MODE,
};

template<uint16_t num>
struct btCmd
{
    std::string rec[num];
    std::string send[num];
    uint16_t n = num;
};

struct msg
{
    uint8_t* data;
    size_t len;
};


class RN42
{
    private:
	GPIO_TypeDef *gpioReset = GPIOE;
	uint16_t      gpioResetPin = GPIO_PIN_1;

    

	btCmd<4> enterHID =
	{
		.rec = { "AOK\r\n", "AOK\r\n", "AOK\r\n", "Reboot!\r\n"},
		.send = { "S~,6\r", "SH,0220\r", "SM,0\r", "R,1\r"}
	};

	btCmd<1> enterCmd =
	{
		.rec = {"CMD\r\n"},
		.send = {"$$$"},
	};

    //*************************************************************************
    // Private Helper Functions
    //*************************************************************************
    template<uint16_t N>
    err send_command_sequence(btCmd<N>* cmds, uint16_t interCmdDelay=100, std::string error="?\r\n");

    protected:
    UART_HandleTypeDef *pHandle;
    BTState state;
    BTState nextState;

    public:
    std::vector<std::string> remotes;

    RxTxMachine *uart;

    RN42(RxTxMachine *handle)
    {
        uart = handle;
    }

    ~RN42(){}

    //*************************************************************************
    // State Machine Handlers
    //*************************************************************************
    err process(msg* data=nullptr);

    err state_enter(msg* data);
    err state_process(msg* data);
    err state_end(msg* data);

    void set_next_state(BTState ns) { this->nextState = ns; }
    BTState get_next_state() { return this->nextState; }
    BTState get_state() { return this->state; }

    //*************************************************************************
    // Bluetooth common commands
    //*************************************************************************
    void hard_reset();
    err soft_reset();
    err enter_command_mode();
    err enter_hid_mode();
    err enter_spp_mode() {return NC_SUCCESS; }
    err connect_stored_remote();
    err connect_new_remote(std::string);
    err connect_new_remote();

    void mouse_command(uint8_t buttons, uint8_t x, uint8_t y);

    //*************************************************************************
    // State Confirmation checks
    //*************************************************************************
	std::string get_remote_connected();

};

#endif
