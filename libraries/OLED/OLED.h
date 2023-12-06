#ifndef OLED_h
#define OLED_h

#include "Arduino.h"
#include "RTClib.h"
#include <U8x8lib.h>



#define SCREEN_WIDTH 32 // OLED display width, in pixels
#define SCREEN_HEIGHT 16 // OLED display height, in pixels

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C



void OLED_init();

void OLED_displayNodes(uint16_t nodes);

void OLED_displayTime(DateTime dt);

void OLED_clearDisplay();

#endif