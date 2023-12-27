#include "RFtransceiver.h"

// Singleton instance of the radio driver
RH_RF69 driver(RFM69_CS,RFM69_INT);





bool Radio_init(){
    if (!driver.init()){
        Serial.println("init failed");
        return 0;
    }


  if (!driver.setFrequency(RF69_FREQ)) {
    Serial.println("setFrequency failed");
  }

  // ishighpowermodule is true 
  driver.setTxPower(7, true);

  driver.setHeaderFrom(CONTROLLER_ADDRESS);
  driver.setHeaderTo(BROADCAST_ADDRESS);

  return 1;
}

void Controller_sendStartRecording(){

    char msg[6] = "start";
    //Serial.print("Sending "); Serial.println(msg);
    driver.send((uint8_t *)msg, strlen(msg));
    driver.waitPacketSent();
}

void Controller_sendStopRecording(){

    char msg[6] = "stop";
    //Serial.print("Sending "); Serial.println(msg);
    driver.send((uint8_t *)msg, strlen(msg));
    driver.waitPacketSent();
} 

bool Controller_checkNodeResponse(uint8_t i){
    uint8_t numbers[4] = {i, (uint8_t)'h', (uint8_t)'e', (uint8_t)'y'};
    

    for(uint8_t j = 0; j < NUM_RESENDS; j++){
        driver.send((const uint8_t*)numbers, sizeof(numbers));
        driver.waitPacketSent();
        
        uint8_t buf[10];
        uint8_t len = sizeof(buf);

        unsigned long currentTime = millis();
        unsigned long previousTime = millis();

        while ((currentTime - previousTime) < RESEND_INTERVAL){
            currentTime = millis();
            if (driver.available()) {
                if(driver.recv(buf, &len)){
                    // Serial.println(buf[0]);
                    if(buf[0] == i && buf[1] == (uint8_t)'h' && buf[2] == (uint8_t)'e' && buf[3] == (uint8_t)'y'){
                        return 1;
                    }
                }
            }
        }
    }
    return 0;
}

uint16_t Controller_ConnectionTest(uint16_t activeNodes){
  uint16_t respondingNodes = 0;
  for(uint8_t i = 0; i < 32; i++){
    if(activeNodes & (1 << i)){    
        if(Controller_checkNodeResponse(i)){
            respondingNodes |= (1<<i);
        }
    }
  }
  return respondingNodes;
}


uint8_t Node_handleReceivedMsg(uint8_t address){
    uint8_t status = 0;

    if (driver.available()) {
        uint8_t buf[RH_RF69_MAX_MESSAGE_LEN];
        uint8_t len = sizeof(buf);


    if (driver.recv(buf, &len)) {
        buf[len] = 0;
        // Serial.print(buf[0]);
        // Serial.println((char*)buf);
        

        if(strcmp((char*)buf,"start") == 0){
            // Serial.println("status = 2");
            status = 2;
            return status;
        }

        if(strcmp((char*)buf,"stop") == 0){
            // Serial.println("status = 3");
            status = 3;
            return status;
        }

        

        if(buf[0] == address && buf[1] == (uint8_t)'h' &&
           buf[2] == (uint8_t)'e' && buf[3] == (uint8_t)'y' ){
                // Serial.println("hey from controller to me");

                //reply
                uint8_t numbers[4] = {address, (uint8_t)'h', (uint8_t)'e', (uint8_t)'y'};
    
                driver.send((const uint8_t*)numbers, sizeof(numbers));
                driver.waitPacketSent();

        }
    }
  }
  return status;
}

uint8_t Controller_handleReceivedMsg(uint8_t address){
    uint8_t sender = 0;
    if (driver.available()) {
        uint8_t buf[10];
        uint8_t len = sizeof(buf);


    if (driver.recv(buf, &len)) {
        buf[len] = 0;
        // Serial.print(buf[0]);
        // Serial.println((char*)buf);

        

        if(buf[0] == address && buf[2] == (uint8_t)'y' && buf[3] == (uint8_t)'o' ){
            // Serial.println("yo from node: ");
            // Serial.println(buf[1]);
            sender = buf[1];
            
            //reply
            uint8_t numbers[4] = {address, buf[1], (uint8_t)'y', (uint8_t)'o'};
            driver.send((const uint8_t*)numbers, sizeof(numbers));
            driver.waitPacketSent();
        }
    }
  }
  return sender;
}

uint8_t Node_ConnectionTest(uint8_t controllerAddress, uint8_t nodeAddress){
    uint8_t numbers[4] = {controllerAddress, nodeAddress, (uint8_t)'y', (uint8_t)'o'};
    
    for(uint8_t j = 0; j < NUM_RESENDS; j++){
        driver.send((const uint8_t*)numbers, sizeof(numbers));
        driver.waitPacketSent();
        
        uint8_t buf[10];
        uint8_t len = sizeof(buf);

        unsigned long currentTime = millis();
        unsigned long previousTime = millis();

        while ((currentTime - previousTime) < RESEND_INTERVAL){
            currentTime = millis();
            if (driver.available()) {
                if(driver.recv(buf, &len)){
                    // Serial.println(buf[0]);
                    if(buf[0] == controllerAddress && buf[1] == nodeAddress && 
                    buf[2] == (uint8_t)'y' && buf[3] == (uint8_t)'o'){
                        return 1;
                    }
                }
            }
        }
    }
    return 0;
}

