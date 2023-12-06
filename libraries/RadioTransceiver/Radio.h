#ifndef Radio_h
#define Radio_h

#include "Arduino.h"
#include <SPI.h>
#include <RH_RF69.h>

// RFM69HCW pins
#define RFM69_CS            10
#define RFM69_INT           2

#define RF69_FREQ           433.0
#define RESEND_INTERVAL     1000
#define NUM_RESENDS         3

//important addresses
#define CONTROLLER_ADDRESS        0
#define BROADCAST_ADDRESS   0xff


bool Radio_init();

void Controller_sendStartRecording();

void Controller_sendStopRecording();

uint8_t Node_handleReceivedMsg(uint8_t address);

uint16_t Controller_ConnectionTest(uint16_t activeNodes);

uint8_t Controller_handleReceivedMsg(uint8_t address);

uint8_t Node_ConnectionTest(uint8_t controllerAddress, uint8_t nodeAddress);


#endif