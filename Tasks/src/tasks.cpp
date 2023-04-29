#include "tasks.h"

#include "main.h"
#include "cmsis_os.h"

#include <uart.h>
#include <bt_api.h>
#include <stdio.h>
#include <ssd1306.h>
#include <oled_io.h>
#include "tasks.h"
#include "bitmasks.h"


extern I2C_HandleTypeDef hi2c1;

extern RNG_HandleTypeDef hrng;

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;
extern DMA_HandleTypeDef hdma_usart1_rx;
extern DMA_HandleTypeDef hdma_usart2_rx;

extern osThreadId defaultTaskHandle;
/* USER CODE BEGIN PV */

extern osThreadId wiggleTaskHandle;
extern osThreadId wiggleTaskUart1Handle;
extern uint16_t inputSelect;
extern volatile uint16_t toggle;
extern InterruptBitMask moduleConnectMask;


err enter_uart_task(UART_HandleTypeDef *pHandle)
{
    return NC_NO_ERROR;
}

/**
 * @brief StartWiggleTask will wiggle all uart states that are valid
 * 
 * @param argument 
 */
void WiggleTaskUart1(void const * argument)
{
    uint16_t secRuns = (60 * 8) / 2;
    err connectStatus = NC_INVALID;
	uint16_t secRun  = 0;
    char buffer[18];
    char subbuf[18];

    uint8_t runDelay = 0;

    uint16_t secRunArray[] = {secRun, secRun, secRun};

    uint16_t maxRetry = 2;
    uint16_t screenIdle = 0;

    printf("Start WiggleTaskUart1\n\r");

    // toggle = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_10);
    toggle = 1;
    moduleConnectMask.btModule0ConnectPin = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_0);
    moduleConnectMask.btModule1ConnectPin = HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_2);
    moduleConnectMask.btModule0TryConnect = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_7);
    moduleConnectMask.btModule1TryConnect = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_8);
    moduleConnectMask.idleEnablePin = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_10);

    for (;;)
    {
        if (!moduleConnectMask.idleEnablePin)
        {
            if (toggle)
            {
                SSD1306_Clear();
                snprintf(buffer, sizeof(buffer), "Idle");
                print_oled(OLED_DATA, buffer);
                toggle = 0;
                HAL_SuspendTick();
                HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
            } 
            continue;
        }
        inputSelect = (1 & (HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_7) == GPIO_PIN_SET)) << 0 |
                  (1 & (HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_8) == GPIO_PIN_SET)) << 1 |
                  (1 & (HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_9) == GPIO_PIN_SET)) << 2; // |
                //   (1 & (HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_10) == GPIO_PIN_SET)) << 3;

        switch_remote(inputSelect);
        set_active_bt_module(1, &huart2);
        if (moduleConnectMask.btModule1ConnectPin)
        {
            if(secRunArray[1])
            {
                secRunArray[1]--;
                snprintf(buffer, sizeof(buffer), "Next Wiggle: % 4d", secRunArray[1]);
                print_oled(OLED_SUBDATA, buffer);
                runDelay = 1;
                
            }
            else
            {
                bt_do_wiggle_random(&hrng);
                print_oled(OLED_INFO, "BlueTooth Wiggle");
                secRunArray[1] = (HAL_RNG_GetRandomNumber(&hrng) % secRuns) + secRuns;
                snprintf(subbuf, sizeof(subbuf), "ADR:%.12s", (char*)get_remote_address());
                print_oled(OLED_SUBDATA, subbuf);
            }
        }
        if(runDelay)
        {
            runDelay = 0;
            osDelay(990);
        }
    }
}

/**
 * @brief StartWiggleTask will wiggle all uart states that are valid
 * 
 * @param argument 
 */
void StartWiggleTask(void const * argument)
{
    uint16_t secRuns = (60 * 8) / 2;
    err connectStatus = NC_INVALID;
	uint16_t secRun  = 0;
    char buffer[18];
    char subbuf[18];

    uint8_t runDelay = 0;

    uint16_t secRunArray[] = {secRun, secRun, secRun};

    uint16_t maxRetry = 2;
    uint16_t screenIdle = 0;

    // toggle = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_10);
    toggle = 1;

    moduleConnectMask.btModule0ConnectPin = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_0);
    moduleConnectMask.btModule1ConnectPin = HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_2);
    moduleConnectMask.btModule0TryConnect = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_7);
    moduleConnectMask.btModule1TryConnect = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_8);
    moduleConnectMask.idleEnablePin = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_10);

    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_1, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_3, GPIO_PIN_SET);
    osDelay(50);
  /* Infinite loop */
  for(;;)
  {
    if (!moduleConnectMask.idleEnablePin)
    {
        if (toggle)
        {
            SSD1306_Clear();
            snprintf(buffer, sizeof(buffer), "Idle");
            print_oled(OLED_DATA, buffer);
            toggle = 0;
            HAL_SuspendTick();
            HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
        } 
        continue;
    }
    

    inputSelect = (1 & (HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_7) == GPIO_PIN_SET)) << 0 |
                  (1 & (HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_8) == GPIO_PIN_SET)) << 1 |
                  (1 & (HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_9) == GPIO_PIN_SET)) << 2; // |
                //   (1 & (HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_10) == GPIO_PIN_SET)) << 3;

    switch_remote(inputSelect);

    // Case 1: The BT modules are connected, and we want them to be
    if (moduleConnectMask.btModule0ConnectPin)
    {
        if(secRunArray[0])
        {
            secRunArray[0]--;
            snprintf(buffer, sizeof(buffer), "Next Wiggle: % 4d", secRunArray[0]);
            print_oled(OLED_DATA, buffer);
            runDelay = 1;
            
        }
        else
        {
            set_active_bt_module(0, &huart1);
            bt_do_wiggle_random(&hrng);
            print_oled(OLED_INFO, "BlueTooth Wiggle");
            secRunArray[0] = (HAL_RNG_GetRandomNumber(&hrng) % secRuns) + secRuns;
            snprintf(subbuf, sizeof(subbuf), "ADR:%.12s", (char*)get_remote_address());
            print_oled(OLED_DATA, subbuf);
        }
    }

    if (moduleConnectMask.btModule1ConnectPin)
    {
        if(secRunArray[1])
        {
            secRunArray[1]--;
            snprintf(buffer, sizeof(buffer), "Next Wiggle: % 4d", secRunArray[1]);
            print_oled(OLED_SUBDATA, buffer);
            runDelay = 1;
            
        }
        else
        {
            set_active_bt_module(1, &huart2);
            bt_do_wiggle_random(&hrng);
            print_oled(OLED_INFO, "BlueTooth Wiggle");
            secRunArray[1] = (HAL_RNG_GetRandomNumber(&hrng) % secRuns) + secRuns;
            snprintf(subbuf, sizeof(subbuf), "ADR:%.12s", (char*)get_remote_address());
            print_oled(OLED_SUBDATA, subbuf);
        }
    }

    // Case 2: The BT modules are not connected, and we want them to be
    if (moduleConnectMask.btModule0TryConnect)
    {
        set_active_bt_module(0, &huart1);
        connectStatus = connect_and_enter_hid(&huart1);
        print_oled(OLED_INFO, "Initializing BT");
        if (connectStatus == NC_ERROR)
        {
            print_oled(OLED_INFO, "Connect Failed");
        }
        else if (connectStatus == NC_COMPLETE)
        {
            for(uint16_t j = 0; j < maxRetry; j++)
            {
                if(is_bt_module_connected() == NC_SUCCESS)
                {
                    print_oled(OLED_INFO, "BT Connected");
                    break;
                }
                osDelay(50);
            }
        }
    }

    if (moduleConnectMask.btModule1TryConnect)
    {
        set_active_bt_module(1, &huart2);
        connectStatus = connect_and_enter_hid(&huart2);
        print_oled(OLED_INFO, "Initializing BT");
        if (connectStatus == NC_ERROR)
        {
            print_oled(OLED_INFO, "Connect Failed");
        }
        else if (connectStatus == NC_COMPLETE)
        {
            for(uint16_t j = 0; j < maxRetry; j++)
            {
                if(is_bt_module_connected() == NC_SUCCESS)
                {
                    print_oled(OLED_INFO, "BT Connected");
                    break;
                }
                osDelay(50);
            }
        }
    }

    if(runDelay)
    {
        runDelay = 0;
        osDelay(990);
    }
  }
}