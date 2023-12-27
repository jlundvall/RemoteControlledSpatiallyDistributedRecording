
#include "Radio.h"
#include "RecordingSchedule.h"
#include "OLED.h"
#include "ButtonInterrupt.h"
#include "Record.h"

#define CONTROLLERADDRESS  0
#define BROADCAST_ADDRESS   0xff

#define DEBUG_MODE     0
#define NUM_BROADCASTS 3

//general pins
#define RESET         4

// RFM69HCW pins
#define RFM69_CS      10
#define RFM69_INT     2


// I/O
#define START_BTN     5
//#define STOP_BTN      6
#define PLAYBACK_BTN  7
#define GREEN_LED     8
#define BLUE_LED      9
#define SQUAD_TRIGGER 6
#define MIC           A0

//sound level sensor parameters:
#define NUM_SAMPLES 100  // Number of microphone samples in averaging window
#define THRESHOLD   100   //bits out of 1024

//set to define recording schedule:
//DateTime: yyyy, mm, dd, h, min, s
//TimeSpan: days, h, min, s
Schedule sched(DateTime(0, 0, 0, 0, 0, 0),   //startTime: first date and time for first recording
               DateTime(0, 0, 0, 0, 0, 0),   //endTime: last date of recording, and time where no new recordings are started that day
               TimeSpan(0, 0, 10, 5),               //recording duration
               TimeSpan(0, 0, 10, 10),              //period: time between start of two following recordings
               1                                   //daily period: 1 = every day, 2 = every other day etc.
);




void setup() {
  Serial.begin(9600);
  while (!Serial);
  
  Button_init(); //stop button uses pin 3 and has interrupts
  Schedule_init(0);
  OLED_init();
  OLED_clearDisplay();
  OLED_displayTime(ReadTime());

  //pin configurations:
  //output pins
  pinMode(RESET, OUTPUT);
  pinMode(BLUE_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(SQUAD_TRIGGER, OUTPUT);
  //input pins
  pinMode(START_BTN, INPUT);
  //pinMode(STOP_BTN, INPUT);
  pinMode(PLAYBACK_BTN, INPUT);
  pinMode(MIC, INPUT);
  
  //init output pins
  digitalWrite(RESET, LOW);
  digitalWrite(BLUE_LED, LOW);
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(SQUAD_TRIGGER, LOW);
  
  manualReset();
  
  if(!Radio_init()){
    if(DEBUG_MODE)Serial.println("RFM69 radio init failed");
    while (1);
  }
  if(DEBUG_MODE)Serial.println("RFM69 radio init OK!");

  Blink(BLUE_LED,  40,  3);
}




uint16_t activeNodes = 0; //to keep track of which nodes are responding
uint16_t micSamples[NUM_SAMPLES] = {0}; //mic samples to calc rms
uint16_t micSampleIndex = 0; //index to keep track of the current sample


bool greenStatus = 0;
bool blueStatus = 0;


void loop() {
  //check if nodes are trying to connect:
  uint8_t activeNode = Controller_handleReceivedMsg(CONTROLLERADDRESS);
  if(activeNode){
    activeNodes |= (1<<activeNode);
  }

  //read start button
  if(digitalRead(START_BTN) == HIGH){
    if(greenStatus == 0){
      for(uint8_t i = 0; i < NUM_BROADCASTS; i++){
        Controller_sendStartRecording();
      }
      Pin_trigger(SQUAD_TRIGGER);
      USB_startRecording();
      digitalWrite(GREEN_LED, HIGH);
      greenStatus = 1;
    }
  }

  //read stop/connection test button interrupt flag
  if(read_button()){ //checking if stop button interruption has happened
    if(greenStatus == 1 or blueStatus == 1){
      for(uint8_t i = 0; i < NUM_BROADCASTS; i++){
        Controller_sendStopRecording();
      }
      Pin_trigger(SQUAD_TRIGGER);
      USB_stopRecording();
      digitalWrite(GREEN_LED, LOW);
      greenStatus = 0;
      digitalWrite(BLUE_LED, LOW);
      blueStatus = 0;
    }
    else{
      if(DEBUG_MODE)Serial.println("Doing connection test");
      uint16_t respondingNodes = Controller_ConnectionTest(activeNodes);
      OLED_clearDisplay();
      OLED_displayNodes(respondingNodes);
      if(DEBUG_MODE)Serial.print("responding nodes: ");
      if(DEBUG_MODE)Serial.println(respondingNodes, BIN);
    }
  }

  //read playback button
    if(digitalRead(PLAYBACK_BTN) == HIGH){
      if(greenStatus == 0){
          for(uint8_t i = 0; i < NUM_BROADCASTS; i++){
            Controller_sendStartRecording();
          }
          delay(500);
          USB_playBack();
          digitalWrite(GREEN_LED, HIGH);
          greenStatus = 1;
          digitalWrite(BLUE_LED, HIGH);
          blueStatus = 1;
      }
    }


   //read and store mic input
   uint16_t micValue = analogRead(MIC);
   //Serial.println(micValue);
   micSamples[micSampleIndex] = micValue;
   micSampleIndex = (micSampleIndex + 1) % NUM_SAMPLES;

   //calculate rms:
   float sumSquared = 0;
   for (int i = 0; i < arraySize; i++) {
    sumSquared += sq(micSamples[i]);
  }
  float meanSquare = sumSquared / NUM_SAMPLES;
  float rms = sqrt(meanSquare);
  
   if(rms > THRESHOLD){
    if(greenStatus == 0){
      if(DEBUG_MODE)Serial.println("mic avg over threshold");
      for(uint8_t i = 0; i < NUM_BROADCASTS; i++){
        Controller_sendStartRecording();
      }
      USB_startRecording();
      Pin_trigger(SQUAD_TRIGGER);
      digitalWrite(GREEN_LED, HIGH);
      greenStatus = 1;
    }
   }

   //check the schedule to see if it's time to record:
    uint8_t action = Schedule_timeToRecord(sched, DEBUG_MODE);
    if(action == START){
      Controller_sendStartRecording();
      USB_startRecording();
      Pin_trigger(SQUAD_TRIGGER);
      digitalWrite(GREEN_LED, HIGH);
      greenStatus = 1;
      
      
    }
    else if(action == STOP){
      Controller_sendStopRecording();
      USB_stopRecording();
      Pin_trigger(SQUAD_TRIGGER);
      
      digitalWrite(GREEN_LED, LOW);
      greenStatus = 0;
      digitalWrite(BLUE_LED, LOW);
      blueStatus = 0;
      
    }
  }


void manualReset(){
  digitalWrite(RESET, LOW);
  delay(10);
  digitalWrite(RESET, HIGH);
  delay(10);
  digitalWrite(RESET, LOW);
  delay(10);
}

void Blink(byte pin, byte delay_ms, byte loops) {
  while (loops--) {
    digitalWrite(pin, HIGH);
    delay(delay_ms);
    digitalWrite(pin, LOW);
    delay(delay_ms);
  }
}
