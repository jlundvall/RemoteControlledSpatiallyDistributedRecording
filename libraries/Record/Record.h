#ifndef Record_h
#define Record_h

#include "Arduino.h"

#define KEY_LEFT_CTRL   0x01

void USB_startRecording();

void USB_stopRecording(); 

void USB_playBack();

void Pin_trigger(byte pin);

#endif