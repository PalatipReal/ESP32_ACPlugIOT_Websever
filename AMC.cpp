#include "AMC.h"
AMC::AMC() {
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}

void AMC::SetAlarm(uint8_t Hour, uint8_t Min) {
  Alarm_Hour  = Hour ;
  Alarm_Min   = Min ;
  Serial.println("Failed to obtain time");
}

void AMC::DisplayNowTime() {
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}

uint8_t AMC::GetNowHour() {
  GetLocalTime();
  return  Hour ;
}

uint8_t AMC::GetNowMin() {
  GetLocalTime();
  return  Min ;
}
uint8_t AMC::GetNowSec() {
  GetLocalTime();
  return  Sec ;
}

bool  AMC::GetStateAlarm() {
  GetLocalTime();
  if (Alarm_Hour == Hour && Alarm_Min == Min) {
    return true ;
  }
  return false ;
}

void AMC::GetLocalTime() {
  getLocalTime(&timeinfo);
  Hour = timeinfo.tm_hour;
  Min = timeinfo.tm_min;
  Sec = timeinfo.tm_sec;
}
