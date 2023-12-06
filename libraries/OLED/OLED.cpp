#include "OLED.h"


U8X8_SH1106_128X64_NONAME_HW_I2C u8x8(U8X8_PIN_NONE);


void OLED_init(){
  u8x8.begin();
  u8x8.setPowerSave(0);
  u8x8.setFont(u8x8_font_chroma48medium8_r);
}


void OLED_displayNodes(uint16_t nodes){
  u8x8.drawString(0,0,"Nodes in range: ");


  uint8_t x_index = 1;
  uint8_t y_index = 2;
  for (uint8_t i = 1; i < 16; i++){
    if(nodes & (1 << i)){
      u8x8.setCursor(x_index, y_index);
      u8x8.print(i);
      if(x_index > 10){
        x_index = 1;
        y_index +=2;
        }
      else{x_index += 3;}
    }
  }  
}

void OLED_displayTime(DateTime dt){
    u8x8.setCursor(2, 3);
    u8x8.print(dt.hour());
    u8x8.print(":");
    u8x8.print(dt.minute());
    u8x8.print(":");
    u8x8.print(dt.second());
    u8x8.setCursor(2, 5);
    u8x8.print(dt.day());
    u8x8.print("/");
    u8x8.print(dt.month());
    u8x8.print("-");
    u8x8.print(dt.year());
}

void OLED_clearDisplay(){
    u8x8.clearDisplay();
}