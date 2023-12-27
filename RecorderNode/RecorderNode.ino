#include "Radio.h"
#include "Record.h"
#include "ButtonInterrupt.h"

#define debugMode  0

#define MYADDRESS 1
#define CONTROLLERADDRESS 0


#define RFM69_RST     4
#define LED           9
#define SQUAD_TRIGGER 6


void setup() {
  Serial.begin(9600);
  while (!Serial);
 
  
  Button_init(); //button uses pin 3

  pinMode(SQUAD_TRIGGER, OUTPUT);
  digitalWrite(SQUAD_TRIGGER, LOW);
  //manual reset of radio
  pinMode(RFM69_RST, OUTPUT);
  digitalWrite(RFM69_RST, LOW);
  delay(10);
  digitalWrite(RFM69_RST, HIGH);
  delay(10);
  digitalWrite(RFM69_RST, LOW);
  delay(10);

  if (!Radio_init()){
    if(debugMode){
      Serial.println("init failed");
      }
    }

  if(debugMode){
    Serial.println("init succeeded");
  }
  Blink(LED, 40, 3);
}


bool recordingStatus = 0;

void loop() {

  uint8_t msg = Node_handleReceivedMsg(MYADDRESS);
  if (msg == 2 && !recordingStatus){
    if(debugMode) {Serial.println("starting");}
    USB_startRecording();
    Pin_trigger(SQUAD_TRIGGER);
    digitalWrite(LED, HIGH);
    recordingStatus = 1; 
  }
  else if (msg == 3 && recordingStatus){
    if(debugMode) Serial.println("stopping");
    USB_stopRecording();
    Pin_trigger(SQUAD_TRIGGER);
    digitalWrite(LED, LOW);
    recordingStatus = 0;
  }
  

  if(read_button()){
    if(Node_ConnectionTest(CONTROLLERADDRESS, MYADDRESS)){
      if(debugMode) Serial.println("node connected");
      Blink(LED, 10, 3);
      
    }
  }

  
}

void Blink(byte pin, byte delay_ms, byte loops) {
  while (loops--) {
    digitalWrite(pin, HIGH);
    delay(delay_ms);
    digitalWrite(pin, LOW);
    delay(delay_ms);
  }
}
