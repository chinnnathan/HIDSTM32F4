#include <uart.h>
#include <uart_priv.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <cstring>
#include "cmsis_os.h"
#include "ncerr.h"
#include "stm32f4xx_hal.h"

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

HAL_StatusTypeDef   RxTxMachine::start_uart_stream()
{
    auto retval = HAL_UART_Receive_DMA(this->pHandle, this->data, sizeof(this->data));

    this->flags.rxBuffRunning = (this->pHandle->RxState == HAL_UART_STATE_BUSY_RX) ? 1 :0;

    this->pBuffer = this->pHandle->pRxBuffPtr;
    this->sizeBuffer = 0;

    return retval;
}

char* RxTxMachine::send_rec_uart_message(size_t msgSizeRx, const char* msg)
{
    size_t msgSizeTx = strlen(msg);

    if (!is_rx_dma_running())
	{
		start_uart_stream();
	}

    this->pBuffer = this->data + this->sizeBuffer;

    HAL_UART_Transmit(this->pHandle, (const uint8_t*)msg, msgSizeTx, UART_TX_TIMEOUT);

    osDelay(100);

    this->sizeBuffer += msgSizeRx;

    return (char*)this->pBuffer;
}

char* RxTxMachine::send_rec_uart_message(const char* msg, const char* str)
{
    size_t msgSizeTx = strlen(msg);
    uint16_t savedIdx = this->sizeBuffer;
    uint16_t findcount = 0;
	uint16_t findMax = strlen(this->strFind);

    if (!is_rx_dma_running())
	{
		start_uart_stream();
	}

    this->pBuffer = this->data + this->sizeBuffer;

    HAL_UART_Transmit(this->pHandle, (const uint8_t*)msg, msgSizeTx, UART_TX_TIMEOUT);

    osDelay(100);
    
    for (; this->sizeBuffer < STREAM_BUFF_SIZE; this->sizeBuffer++)
    {
        printf("%c", this->data[this->sizeBuffer]);
        if (this->data[this->sizeBuffer] == str[findcount])
        {
            findcount++;
            if (findcount == findMax)
            {
                this->sizeBuffer++;
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
            if (this->data[this->sizeBuffer] == str[findcount])
            {
                findcount++;
                if (findcount == findMax)
                {
                    this->flags.intHit = 0;
                    this->sizeBuffer++;
                    break;
                }                
            }
        }
    }

    return (char*)this->pBuffer;
}

err RxTxMachine::send_rec_val_uart_message(const char* msgTx, const char* msgVal)
{
    size_t msgSizeTx = strlen(msgTx);
    size_t msgSizeVal = strlen(msgVal);

    if (!is_rx_dma_running())
    {
        start_uart_stream();
    }

    auto status = HAL_UART_Transmit(this->pHandle, (const uint8_t*)msgTx, msgSizeTx, UART_TX_TIMEOUT);

    if (status != HAL_OK)
    {
        return NC_ERROR;
    }

    this->pBuffer = this->data + this->sizeBuffer;
    this->sizeBuffer += msgSizeVal;

    for (auto retry = this->retryCount; retry > 0; retry--)
    {
        if (!memcmp(this->pBuffer, msgVal, msgSizeVal))
        {
            return NC_SUCCESS;
        }
        osDelay(this->retryDelay);
    }
    
    return NC_ERROR;
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

err RxTxMachine::send_rec_print_uart(const char* msgTx, const char* strFind)
{
    size_t msgSizeTx = strlen(msgTx);

    if (msgTx[msgSizeTx-1] == 0)
        msgSizeTx--;

    if (!is_rx_dma_running())
	{
		start_uart_stream();
	}

    HAL_UART_Transmit(this->pHandle, (const uint8_t*)msgTx, msgSizeTx, UART_TX_TIMEOUT);

    this->flags.countUnaligned = 1;
    this->strFind = (char*)strFind;

    return NC_NO_ERROR;
} 

HAL_StatusTypeDef get_uart_message(UART_HandleTypeDef *pHandle, uint8_t* pBuff, size_t msgSize, uint8_t print)
{
    memset(pBuff, 0, msgSize);        
//    auto retval = HAL_UART_Receive(pHandle, pCmder->pBuff, msgSize, UART_RX_TIMEOUT);
    auto retval = HAL_UART_Receive_DMA(pHandle, pBuff, msgSize);
    // auto retval = HAL_UART_Receive_IT(pHandle, pBuff, msgSize);
    // auto retval = UART_Start_Receive_DMA(pHandle, pBuff, msgSize);



    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET);
    return retval;
}

HAL_StatusTypeDef RxTxMachine::send_uart_message(const char* mes)
{
    size_t msgSize = strlen(mes);
    if (mes[msgSize-1] == 0) 
        msgSize--;

    auto status = HAL_UART_Transmit(this->pHandle, (const uint8_t*)mes, msgSize, UART_TX_TIMEOUT);

#ifdef DEBUG
    osDelay(100);
#endif
    return status;
}

HAL_StatusTypeDef send_uart_message(UART_HandleTypeDef *pHandle, const char* mes)
{
    size_t msgSize = strlen(mes);
    if (mes[msgSize-1] == 0) 
        msgSize--;

    auto status = HAL_UART_Transmit(pHandle, (const uint8_t*)mes, msgSize, UART_TX_TIMEOUT);
#ifdef DEBUG
    osDelay(100);
#endif
    return status;
}

uint8_t is_ret_msg_valid(uint8_t * pBuff, const char* msg)
{
    size_t msgSize = strlen(msg);

    for (size_t i = 0; i < msgSize; i++)
    {
        if (pBuff[i] != msg[i])
            return 0;
    }
    return 1;
}
