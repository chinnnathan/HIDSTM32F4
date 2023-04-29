#include "main.h"
#include <stdio.h>


void loop() {
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_1, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_3, GPIO_PIN_SET);

    HAL_Delay(500U);
}

int main() {
    while(1)
    {
        loop();
    }
    return 0;
}

