#include <uart.h>
#include <ncerr.h>
#include <string>
#include <cstring>

#ifndef CORE_INC_UART_PRIV_HPP
#define CORE_INC_UART_PRIV_HPP

typedef void (*registerRxCallback)(UART_HandleTypeDef *huart, uint16_t Size);
constexpr uint32_t STREAM_BUFF_SIZE = 256;

class RxTxMachine
{
    private:
    size_t sizeBuffer;

    uint8_t data[STREAM_BUFF_SIZE];

    uint8_t retryCount = 5;
    uint16_t retryDelay = 100;   

    public:
    std::string retstr, strFind;
    uint8_t* pBuffer = data;
    UART_HandleTypeDef *pHandle;

    registerRxCallback pCallback;

    uint8_t is_rx_dma_running()
    {
        this->flags.rxBuffRunning = (this->pHandle->RxState == HAL_UART_STATE_BUSY_RX) ? 1 :0;
        return this->flags.rxBuffRunning;
    }

    std::string get_return_message(std::string search);

    union uartFlags
    {
        struct
        {
            uint8_t intHit : 1;
            uint8_t rxBuffRunning : 1;
            uint8_t countUnaligned : 1;
            uint8_t initialized : 1;
            uint8_t reserved : 4;
        };
        uint8_t all;
    } flags;
    

    RxTxMachine(UART_HandleTypeDef* handle, uint8_t* buffer)
    {
        pHandle = handle;
        pBuffer = buffer;
        sizeBuffer = sizeof(buffer);
        flags.all = 0;
        flags.initialized = 1;
        strFind = "\r\n";        
    }

    RxTxMachine(UART_HandleTypeDef* handle)
    {
        pHandle = handle;
        sizeBuffer = 0;
        flags.all = 0;
        flags.initialized = 1;
        strFind = "\r\n";
    }

    RxTxMachine()
    {
        flags.all = 0;
    }

    ~RxTxMachine()
    {

    }

    err         send_rec_val_uart_message(std::string msgTx, std::string msgVal);
    std::string send_rec_uart_message(std::string msg, std::string search);
    err         send_uart_message(std::string msgTx);
    err         send_uart_byte(uint8_t* msgTx);

    void reset_buffer_ptr() { this->sizeBuffer = 0; }

    HAL_StatusTypeDef   start_uart_stream();

    void set_uart_handle(UART_HandleTypeDef* handle) { this->pHandle = handle; }

    void print_uart();
};

void set_active_machine(RxTxMachine *ptr);


#endif
