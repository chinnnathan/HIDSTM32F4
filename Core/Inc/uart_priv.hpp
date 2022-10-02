#include <uart.h>
#include <ncerr.h>

typedef void (*registerRxCallback)(uint8_t* pBuff);
constexpr uint32_t STREAM_BUFF_SIZE = 64;

class RxTxMachine
{
    private:
    UART_HandleTypeDef *pHandle;
    uint8_t* pBuffer;
    size_t sizeBuffer;

    uint8_t data[STREAM_BUFF_SIZE];

    uint8_t retryCount = 5;
    uint16_t retryDelay = 100;
    public:

    registerRxCallback pCallback;

    uint8_t is_rx_dma_running()
    {
        this->flags.rxBuffRunning = (this->pHandle->RxState == HAL_UART_STATE_BUSY_RX) ? 1 :0;
        return this->flags.rxBuffRunning;
    }

    union uartFlags
    {
        struct
        {
            uint8_t intHit : 1;
            uint8_t rxBuffRunning : 1;
            uint8_t countUnaligned : 1;
            uint8_t reserved : 5;
        };
        uint8_t all;
    } flags;
    

    RxTxMachine(UART_HandleTypeDef* handle, uint8_t* buffer, registerRxCallback callback)
    {
        pHandle = handle;
        pBuffer = buffer;
        sizeBuffer = sizeof(buffer);
        pCallback = callback;
        flags.all = 0;
    }

    ~RxTxMachine()
    {

    }

    HAL_StatusTypeDef   start_uart_stream();

    void set_uart_handle(UART_HandleTypeDef* handle) { this->pHandle = handle; }

    void print_uart();

    err     send_rec_val_uart_message(const char* msgTx, const char* msgVal);
    char*   send_rec_uart_message(size_t msgSize, const char* msg);
    err     send_rec_print_uart(const char* msgTx);
    HAL_StatusTypeDef send_uart_message(const char* mes);

    HAL_StatusTypeDef   send_uart_message(UART_HandleTypeDef *pHandle, const char* mes);
    HAL_StatusTypeDef   get_uart_message(UART_HandleTypeDef *pHandle, uint8_t* pBuff, size_t msgSize=BUFF_SIZE, uint8_t print=1);
};

void set_active_machine(RxTxMachine *ptr);
