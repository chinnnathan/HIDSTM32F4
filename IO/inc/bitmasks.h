#include <stdio.h>

typedef union InterruptBitMask
{
    struct
    {
        uint8_t btModule0ConnectPin : 1;
        uint8_t btModule0TryConnect : 1;
        uint8_t btModule1ConnectPin : 1;
        uint8_t btModule1TryConnect : 1;
        uint8_t idleEnablePin       : 1;
        uint8_t reserved            : 3;
    };
    uint8_t all;
}InterruptBitMask;
