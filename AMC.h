#ifndef AMC_h
#define AMC_h
#include "Arduino.h"
#include "time.h"
class AMC
{
    public:
        AMC();
        void  SetAlarm(uint8_t Hour,uint8_t Min);
        void  DisplayNowTime();
        uint8_t   GetNowHour();
        uint8_t   GetNowMin();
        uint8_t   GetNowSec();
        bool  GetStateAlarm();
        void  GetLocalTime();
        
    private:
        const char* ntpServer = "pool.ntp.org";
        const long  gmtOffset_sec = 21600;
        const int   daylightOffset_sec = 3600;
        bool StateAlarm        = false  ;
        uint8_t Hour              = 0 ;
        uint8_t Min               = 0 ;
        uint8_t Sec               = 0 ;
        uint8_t Alarm_Hour        = 0 ;
        uint8_t Alarm_Min         = 0 ;
        struct tm timeinfo;
        
};
#endif
