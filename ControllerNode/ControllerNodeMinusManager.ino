
#include "Radio.h"
#include "Schedule.h"
#include "OLED.h"
#include "ButtonInterrupt.h"
#include "Record.h"

#define CONTROLLERADDRESS  0
#define BROADCAST_ADDRESS   0xff

#define DEBUG_MODE     1
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

//Mic input values

#define NUM_SAMPLES 100  // Number of samples to consider
#define THRESHOLD   100   //bits out of 1024


//Real Time Clock stuff:
RTC_DS3231 rtc;


void setup() {
  if(DEBUG_MODE){
  Serial.begin(9600);
  while (!Serial);
  }

  //OLED_init();
  Button_init(); //stop button uses pin 3 and has interrupts

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
    Serial.println("RFM69 radio init failed");
    while (1);
  }
  Serial.println("RFM69 radio init OK!");

  //init RTC
}

uint32_t activeNodes = 0; //to keep track of which nodes are responding
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
      USB_startRecording();
      Pin_trigger(SQUAD_TRIGGER);
      digitalWrite(GREEN_LED, HIGH);
      greenStatus = 1;
    }
  }

  //read stop button interrupt flag
  if(read_button()){ //checking if stop button interruption has happened
    if(greenStatus == 1 or blueStatus == 1){
 
      for(uint8_t i = 0; i < NUM_BROADCASTS; i++){
        Controller_sendStopRecording();
      }
      USB_stopRecording();
      Pin_trigger(SQUAD_TRIGGER);
      
      digitalWrite(GREEN_LED, LOW);
      greenStatus = 0;
      digitalWrite(BLUE_LED, LOW);
      blueStatus = 0;
    }
    else{
      Serial.println("Doing connection test");
      
      uint32_t respondingNodes = Controller_ConnectionTest(activeNodes);
      //OLED_printNodes(respondingNodes);
      Serial.print("responding nodes: ");
      Serial.println(respondingNodes, BIN);
    }
  }

  //read playback button
    if(digitalRead(PLAYBACK_BTN) == HIGH){
      if(greenStatus == 0){
          for(uint8_t i = 0; i < NUM_BROADCASTS; i++){
            Controller_sendStartRecording();
          }
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

   //calculate average
   int sum = 0;
   for (int i = 0; i < NUM_SAMPLES; i++){
    sum += micSamples[i];
   }
   int avg = sum/NUM_SAMPLES;
   if(avg > THRESHOLD){
    
    if(greenStatus == 0){
      Serial.println("mic avg over threshold");
      for(uint8_t i = 0; i < NUM_BROADCASTS; i++){
        Controller_sendStartRecording();
      }
      USB_startRecording();
      Pin_trigger(SQUAD_TRIGGER);
      digitalWrite(GREEN_LED, HIGH);
      greenStatus = 1;
    }
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
