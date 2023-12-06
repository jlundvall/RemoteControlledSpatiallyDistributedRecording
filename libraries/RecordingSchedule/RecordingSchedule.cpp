#include "RecordingSchedule.h"



RTC_DS3231 rtc;

uint8_t state = WAITING;
int n = 1;
uint8_t timesRepeatedToday = 0;
TimeSpan daysRepeated(0,0,0,0);
DateTime startOfRecording(0,0,0,0,0,0);

void Schedule_init(bool debug){
    state = WAITING;
    Wire.begin();
    if (! rtc.begin()){
        if(debug) Serial.println(" RTC Module not Present");
        while (1);
    }
    if (rtc.lostPower()){
        if(debug) Serial.println("RTC power failure, reset the time!");
        rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    }
    if(debug) rtc.adjust(DateTime(2023,9,22,0,0,0));
}

void printDateTime(DateTime dt){
    Serial.print(dt.hour());
    Serial.print(":");
    Serial.print(dt.minute());
    Serial.print(":");
    Serial.print(dt.second());
    Serial.print(" ");
    Serial.print(dt.day());
    Serial.print("/");
    Serial.print(dt.month());
    Serial.print("-");
    Serial.print(dt.year());
    Serial.print("\n");
}

void printTimeSpan(TimeSpan ts){
    Serial.print(ts.days());
    Serial.print(" d, ");
    Serial.print(ts.hours());
    Serial.print(" h, ");
    Serial.print(ts.minutes());
    Serial.print(" min, ");
    Serial.print(ts.seconds());
    Serial.print(" s, \n");
}

bool doneForTheDay(DateTime now, DateTime dt){
    if(now.hour() > dt.hour()){
        return 1;
    }
    if (now.minute() > dt.minute() and now.hour() == dt.hour()){
        return 1;
    }
    if(now.second() >= dt.second() and now.minute() >= dt.minute() and now.hour() >= dt.hour()){
        return 1;
    }
    return 0;
}


uint8_t Schedule_timeToRecord(Schedule sched, bool debug){
    uint8_t action = NONE; 
    switch(state) {
        case WAITING:
            if (daysRepeated.days() % sched.dailyRepeatPeriod == 0){
                if(rtc.now() >= (sched.startTime + daysRepeated)){
                    timesRepeatedToday ++;
                    state = RECORDING;
                    startOfRecording = rtc.now();

                    if (debug){
                        printDateTime(rtc.now());
                        Serial.println("start recording\n");
                    }
                    
                    action = START;
                    return action;
                    //USB_startRecording();
                    }else{
                        state = WAITING;
                        break;
                    }
            }else{
          state = DONE_FOR_THE_DAY;
          break;
        }


        case RECORDING:
            if(rtc.now() >= (startOfRecording + sched.recDuration)){
                state = BREAK;
                
                if (debug){
                        printDateTime(rtc.now());
                        Serial.println("stop recording\n");
            }
            action = STOP;
            return action;
            //USB_stopRecording();

            }else{
            state = RECORDING;
            break;
            }

        case BREAK:
            if(doneForTheDay(rtc.now(), sched.endTime) == 1){
            state = DONE_FOR_THE_DAY;
            }else{
                if(rtc.now() >= startOfRecording + sched.repeatPeriod){
                    state = RECORDING;
                    startOfRecording = rtc.now();

                    if (debug){
                        printDateTime(rtc.now());
                        Serial.println("start recording\n");
                    }
                    action = START;
                    return action;
                    //USB_startRecording();
                   // break;


                }else{
                    state = BREAK;
                    break;
                }
            }

        case DONE_FOR_THE_DAY:
            if (debug){
                printDateTime(rtc.now()); 
                Serial.println("done for the day\n");
            }

            daysRepeated = daysRepeated + TimeSpan(1,0,0,0); 
            timesRepeatedToday = 0;

            if(rtc.now() >= sched.endTime){
                state = DONE;
            }else{
                state = WAITING;

            
            if (debug){
                /*emulate day change happening - REMOVE*/
                rtc.adjust(DateTime(2023,9,22+n,0,0,0));
                n++;
                Serial.println("\nchanging day to next day\n");
            }
            /*---------------------------------------*/
            
            break;
            }

        case DONE: 
            action = NONE;

    }

    return action;
}


DateTime ReadTime(){
    DateTime now = rtc.now();
    return now;
}