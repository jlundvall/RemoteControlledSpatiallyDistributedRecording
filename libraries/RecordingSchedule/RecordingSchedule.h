#ifndef RecordingSchedule_h
#define RecordingSchedule_h

#include "Arduino.h"
#include <RTClib.h>
#include <Wire.h>

struct Schedule{
    DateTime startTime;
    DateTime endTime;
    TimeSpan recDuration;
    TimeSpan repeatPeriod;
    int dailyRepeatPeriod = 1;

    Schedule(DateTime st, DateTime et, 
              TimeSpan dur, TimeSpan period, int dailyPeriod){
                startTime = st;
                endTime = et;
                recDuration = dur;
                repeatPeriod = period;
                dailyRepeatPeriod = dailyPeriod;
              }
};

enum States{WAITING, RECORDING, BREAK, DONE_FOR_THE_DAY, DONE};

enum Actions{NONE, START, STOP};

void Schedule_init(bool debug);

uint8_t Schedule_timeToRecord(Schedule sched, bool debug);

void printDateTime(DateTime dt);

void printTimeSpan(TimeSpan ts);

bool doneForTheDay(DateTime now, DateTime dt);

DateTime ReadTime();



#endif