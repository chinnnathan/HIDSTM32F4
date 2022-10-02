#include <oled_io.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <cstring>

namespace 
{
    FontDef_t* useFont = &Font_7x10;
}
constexpr uint32_t maxLen = SSD1306_WIDTH / 7;
constexpr uint16_t startBlueY = 20;

void print_oled(oledPrintType pt, char* str)
{
    uint16_t startY;
    
    if (pt == OLED_INFO)
    {
        startY = 0;
    }
    else if (pt == OLED_DATA)
    {
        startY = startBlueY;
    }
    else if (pt == OLED_SUBDATA)
    {
        startY = startBlueY + (3+useFont->FontHeight);
    }
    else
    {
        startY = startBlueY + 2*(3+useFont->FontHeight);
    }

    char * buff = str;
    auto len = strlen(str);
    if ((len * useFont->FontWidth) < SSD1306_WIDTH)
    {        
        SSD1306_DrawFilledRectangle(0,startY, SSD1306_WIDTH, useFont->FontHeight, SSD1306_COLOR_BLACK);
        SSD1306_GotoXY(0,startY);
        SSD1306_Puts(str, useFont, SSD1306_COLOR_WHITE);
    }
    else
    {
        uint32_t i = 0;
        uint32_t buffInc = 0;
        // currently only support 1 line
        // while (buffInc < len) 
        {
            SSD1306_GotoXY(0,startY + (i * (3 + useFont->FontHeight)));
            SSD1306_Puts(buff+buffInc, useFont, SSD1306_COLOR_WHITE);
            i++;
            buffInc += maxLen;
        }
    }
    SSD1306_UpdateScreen();
}
