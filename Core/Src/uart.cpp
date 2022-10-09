#include <uart.h>
#include <uart_priv.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <cstring>
#include <string>
#include "cmsis_os.h"
#include "ncerr.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_uart.h"

constexpr uint32_t UART_TX_TIMEOUT = 1000;
constexpr uint32_t UART_RX_TIMEOUT = 1000;


namespace
{
    uint8_t buff[UART_LOG_SIZE];
    RxTxMachine *activePointer;
    uint8_t* pBuff = buff;
    uint16_t buffCounter = 0;
}

const uint8_t* pScratch = buff;

void set_active_machine(RxTxMachine *ptr)
{
    activePointer = ptr;
}



void print_used_scratch_pointer()
{
    auto i = 0;
    for (auto c : buff)
    {
    	printf("%c", c);
    	i++;
    	if (i == buffCounter)
    		break;
    }
    printf("\n\r");
    
}

void reset_scratch_pointer()
{
    pBuff = buff;
    buffCounter = 0;
}

void HAL_UART_RxHalfCpltCallback(UART_HandleTypeDef *huart)
{
#ifdef DEBUG
	printf("Halfway\n\r");
#endif
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
#ifdef DEBUG
	printf("Fully\n\r");
#endif
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);

    activePointer->flags.intHit = 1;
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    printf("Rx Event Callback hit\n");
}

std::string RxTxMachine::get_return_message(std::string search)
{
    retstr.clear();

    uint8_t* pChar;

    if (this->flags.intHit)
    {
        retstr.append((const char*)this->pBuffer, (STREAM_BUFF_SIZE - this->sizeBuffer));
        this->sizeBuffer = 0;
        this->pBuffer = this->data;
    }

    pChar = (uint8_t*)strstr((const char*)this->pBuffer, search.c_str());

    if(pChar)
    {
        uint16_t inc = (pChar - (uint8_t*)this->pBuffer) + search.size();
        retstr.append((const char*)this->pBuffer, inc);
        this->pBuffer += inc;
        this->sizeBuffer += inc;
    }

    return retstr;
}

HAL_StatusTypeDef RxTxMachine::start_uart_stream()
{
    auto retval = HAL_UART_Receive_DMA(this->pHandle, this->data, sizeof(this->data));

    this->flags.rxBuffRunning = (this->pHandle->RxState == HAL_UART_STATE_BUSY_RX) ? 1 :0;

    this->pBuffer = this->pHandle->pRxBuffPtr;
    this->sizeBuffer = 0;

    return retval;
}

err RxTxMachine::send_rec_val_uart_message(std::string msgTx, std::string msgVal)
{
    if (!is_rx_dma_running())
    {
        start_uart_stream();
    }

    auto status = HAL_UART_Transmit(this->pHandle, (const uint8_t*)msgTx.c_str(), msgTx.size(), UART_TX_TIMEOUT);

    osDelay(100);

    if (status != HAL_OK)
    {
        return NC_ERROR;
    }
    
    for (auto i = 0; i < this->retryCount; i++)
    {
        this->get_return_message(msgVal);

        if(!retstr.empty())
        {
            if (msgVal.compare(retstr) == 0)
                return NC_SUCCESS;
        }
    }

    return NC_ERROR;
}

std::string RxTxMachine::send_rec_uart_message(std::string msg, std::string search)
{
    std::string retstr;

    if (!is_rx_dma_running())
	{
		start_uart_stream();
	}

    HAL_UART_Transmit(this->pHandle, (const uint8_t*)msg.c_str(), msg.size(), UART_TX_TIMEOUT);

    osDelay(100);

    return this->get_return_message(search);
}

err RxTxMachine::send_uart_message(std::string msgTx)
{
    if (!is_rx_dma_running())
    {
        start_uart_stream();
    }

    auto status = HAL_UART_Transmit(this->pHandle, (const uint8_t*)msgTx.c_str(), msgTx.size(), UART_TX_TIMEOUT);

    if (status != HAL_OK)
    {
        return NC_ERROR;
    }
    
    return NC_SUCCESS;
}

err RxTxMachine::send_uart_byte(uint8_t* msgTx)
{
    if (!is_rx_dma_running())
    {
        start_uart_stream();
    }

    auto status = HAL_UART_Transmit(this->pHandle, (const uint8_t*)msgTx, 1, UART_TX_TIMEOUT);

    if (status != HAL_OK)
    {
        return NC_ERROR;
    }
    
    return NC_SUCCESS;
}

void RxTxMachine::print_uart()
{
    auto savedIdx = this->sizeBuffer;
    auto findcount = 0;
	auto findMax = strlen(this->strFind);

    printf("\n-----------------------\n");

    for (; this->sizeBuffer < STREAM_BUFF_SIZE; this->sizeBuffer++)
    {
        printf("%c", this->data[this->sizeBuffer]);
        if (this->data[this->sizeBuffer] == this->strFind[findcount])
        {
            findcount++;
            if (findcount == findMax)
            {
                this->flags.countUnaligned = 0;
                break;
            }
        }
        else if (findcount)
        {
        	findcount = 0;
        }
    }
    if (this->flags.intHit)
    {
        for (this->sizeBuffer = 0; this->sizeBuffer < savedIdx; this->sizeBuffer++)
        {
            printf("%c", this->data[this->sizeBuffer]);
            if (this->data[this->sizeBuffer] == this->strFind[findcount])
            {
                findcount++;
                if (findcount == findMax)
                {
                    this->flags.countUnaligned = 0;
                    this->flags.intHit = 0;
                    break;
                }                
            }
        }
    }
    printf("\n-----------------------\n");
}
