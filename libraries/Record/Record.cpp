
#include "Arduino.h"
#include "Record.h"

void releaseKey() 
{
  uint8_t buf[8] = {0};
  buf[0] = 0;
  buf[2] = 0;
  Serial.write(buf, 8); // Release key
}

void USB_stopRecording(){
  uint8_t buf[8] = {0};
  buf[0] = KEY_LEFT_CTRL;
  buf[2] = 22; //S
  Serial.write(buf, 8); // Send keypress
  delay(200); //this needs delay for some reason
  releaseKey();
}

void USB_startRecording(){
  uint8_t buf[8] = {0};
  buf[0] = KEY_LEFT_CTRL;
  buf[2] = 21; //R
  Serial.write(buf, 8); // Send keypress
  releaseKey();
}

void USB_playBack(){
  uint8_t buf[8] = {0};
  buf[0] = 0;
  buf[2] = 44; //space
  Serial.write(buf, 8); // Send keypress
  releaseKey();
}


void Pin_trigger(byte pin){
  digitalWrite(pin, LOW);
  delay(1);
  digitalWrite(pin, HIGH);
  delay(10);
  digitalWrite(pin, LOW);
}


