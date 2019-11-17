#include "ACS712.h"
ACS712::ACS712(int pin) {
  _pin = pin;
}

int ACS712::ACS712_Calibrate() {
  acc = 0;
  for (int i = 0; i < 10; i++) {
    acc += analogRead(A0);
  }
  ZeroAsc714Calibrate = acc / 10;
  return ZeroAsc714Calibrate;
}

double ACS712::GetCurrent() {
  double SumCurrent = 0;
  for (int i = 0; i < Count; i++) {
    double AdcNow = ((1.1 * analogRead(A0)) / 819) * 1000;
    double AdcOffSet = ((1.1 * ZeroAsc714Calibrate) / 819) * 1000;
    double Current = (AdcNow - AdcOffSet) / sensitivity;
    SumCurrent += Current;
  }
  double val = SumCurrent / Count;
  if (val <= 0) {
    val = 0.00 ;
  }
  return val ;
}

double ACS712::GetWatt(double P) {
  double W = P * GetCurrent() ;
  return W ;
}

//double ACS712::GetVolt(){
//
//}
