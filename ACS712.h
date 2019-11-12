#ifndef ACS712_h
#define ACS712_h
#include "Arduino.h"
#define Count 200
#define sensitivity 100
class ACS712
{
    public:
        ACS712(int pin);
        double GetCurrent();
        double GetWatt(double P);
        //double GetVolt();
        int ACS712_Calibrate();
    private:
        uint8_t _pin ;
        int ZeroAsc714Calibrate = 0 ;
        uint16_t acc = 0;
};
#endif
