
#include <ssd1306.h>

#ifndef CORE_INC_OLED_API_H_
#define PROGRAM_INC_OLED_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "main.h"

typedef enum {
    OLED_INFO,
    OLED_DATA,
    OLED_SUBDATA
}oledPrintType;

void print_oled(oledPrintType pt, char* str);

#ifdef __cplusplus
}
#endif

#endif /* PROGRAM_INC_OLED_H_ */