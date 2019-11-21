//===============================================================
//  Include WiFi WiFiClient WebServer
//===============================================================
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
//===============================================================
// Include FireBaseEsp32 And Define Host,Auth
//===============================================================
#include <FirebaseESP32.h>
#define FIREBASE_HOST "https://embedapp-9157e.firebaseio.com" //Do not include https:// in FIREBASE_HOST
#define FIREBASE_AUTH "P2aAecFYcDqsdHowVHZv1wJUsJivlpBSh82DN9ll"
//===============================================================
//   Include Index.h For Web Page
//===============================================================
#define LED 2
String  ledState1         = "OFF" ;
String  Relay1            = ""    ;
String  Hour_Off          = ""    ;
String  Min_Off           = ""    ;
bool    AlarmOn           = false ;
bool    AlarmOff          = false ;
bool    StateTaskAlarmOn  = false ;
bool    StateTaskAlarmOff = false ;
uint8_t Timer1OnHour      = 0     ;
uint8_t Timer1OnMin       = 0     ;
uint8_t Timer1OffHour     = 0     ;
uint8_t Timer1OffMin      = 0     ;
#include "index.h"  //Web page header file
//===============================================================
//  Include ACS712 For Read Analog From Acs712
//===============================================================
#include "ACS712.h"
double  Amp_Double = 0.00 ;
String  Amp_String    = "0" ;
String  Watt   = "0" ;
ACS712 Sensor(A0);
//===============================================================
//  Include AMC
//===============================================================
#include "AMC.h"
//===============================================================
//  Include Wire,Adafruit_GFX For OLED
//===============================================================
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128      // OLED display width, in pixels
#define SCREEN_HEIGHT 64      // OLED display height, in pixels
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

//#include <Wire.h>               // Only needed for Arduino 1.6.5 and earlier
//#include "SSD1306Wire.h"
//SSD1306Wire display(0x3c, 21, 22);
//===============================================================
//   Start WebServer Port80 And Firebase
//===============================================================
WebServer server(80);
FirebaseData firebaseData1; // For Set Data
FirebaseData firebaseData2; // For Read Data Led1
FirebaseData firebaseData3;
FirebaseData firebaseData4;
FirebaseData firebaseData5;
bool LED1status1 = LOW;
bool LED2status = LOW;
//===============================================================
//  Enter your SSID and PASSWORD
//===============================================================
//const char* ssid       = "iot";
//const char* password   = "C0mputinG";
//const char* ssid       = "iot-lab";
//const char* password   = "computing";
//const char* ssid       = "Renat";
//const char* password   = "nattapong2539nat";
const char* ssid       = "KFC Free WiFi";
const char* password   = "12345677";
//===============================================================
//  TaskHandle
//===============================================================
xTaskHandle Webserver_Task_Handle;
xTaskHandle ReadAsc712_Task_Handle;
xTaskHandle DisplayOled_Task_Handle;
xTaskHandle AlarmOn_Task_Handle;
xTaskHandle AlarmOff_Task_Handle;
//===============================================================
// This routine is executed when you open its IP in browser
//===============================================================
void handleRoot() {
  String s = MAIN_page; //Read HTML contents
  server.send(200, "text/html", s); //Send web page
}
//===============================================================
// Read ADC and send to WebServer
//===============================================================
void handleADC() {
  String SensorData_Led1 = "{\"Watt\":\"" + Watt
                           + "\", \"Amp\":\"" + Amp_String
                           + "\"}";
  server.send(200, "text/plane", SensorData_Led1); //Send ADC value only to client ajax request
}
//===============================================================
// Read LED1 From WebServer
//===============================================================
void handleLED1() {
  String t_state = server.arg("LEDstate1"); //Refer  xhttp.open("GET", "setLED?LEDstate1="+led, true);
  Serial.print("User Set ");
  Serial.print("Relay 1 :");
  Serial.println(t_state);
  Serial.println("");
  if (t_state == "1")
  {
    digitalWrite(LED, LOW); //LED ON
    ledState1 = "ON"; //Feedback parameter
  }
  else
  {
    digitalWrite(LED, HIGH); //LED OFF
    ledState1 = "OFF"; //Feedback parameter
  }
  Relay1 = ledState1;
  server.send(200, "text/plane", ledState1); //Send web page
  Firebase.setString(firebaseData2, "/Relay/Relay1", ledState1); // send Data to Firebase

}
//===============================================================
//
//===============================================================
void handleResetTimer() {
  Serial.println("User Reset Timer");
  Firebase.setBool(firebaseData1, "/Timer/Timer1On/State", false);
  Firebase.setBool(firebaseData1, "/Timer/Timer1Off/State", false);
  delay(100);
  Firebase.setInt(firebaseData2, "/Timer/Timer1On/Hour",   0);
  Firebase.setInt(firebaseData2, "/Timer/Timer1On/Min",    0);
  delay(100);
  Firebase.setInt(firebaseData3, "/Timer/Timer1Off/Hour",  0);
  Firebase.setInt(firebaseData3, "/Timer/Timer1Off/Min",   0);
  delay(100);
  if (StateTaskAlarmOn == true) {
    vTaskSuspend(AlarmOn_Task_Handle);
    vTaskDelete(AlarmOn_Task_Handle);
    StateTaskAlarmOn = false ;
  }
  if (StateTaskAlarmOff == true) {
    vTaskSuspend(AlarmOff_Task_Handle);
    vTaskDelete(AlarmOff_Task_Handle);
    StateTaskAlarmOff = false ;
  }
  Serial.println("Delete Alarm Task");

}
//===============================================================
// Read Timer1_ON From WebServer
//===============================================================
void handleTimer1OnHour() {
  Timer1OnHour = server.arg(0).toInt();
  Serial.print("User Set Timer On Hour:");
  Serial.println(Timer1OnHour);
  Firebase.setInt(firebaseData2, "/Timer/Timer1On/Hour", Timer1OnHour);
}
void handleTimer1OnMin() {
  Timer1OnMin = server.arg(0).toInt();
  Serial.print("User Set Timer On Min:");
  Serial.println(Timer1OnMin);
  Firebase.setInt(firebaseData3, "/Timer/Timer1On/Min", Timer1OnMin);
  Firebase.setBool(firebaseData3, "/Timer/Timer1On/State", true);
  //  vTaskSuspend(Alarm_Task_Handle);
  //  vTaskDelete(Alarm_Task_Handle);
  GetStateAlarm();
}
//===============================================================
// Read Timer1_OFF From WebServer
//===============================================================
void handleTimer1OffHour() {
  Timer1OffHour = server.arg(0).toInt();
  Serial.print("User Set Timer Off Hour:");
  Serial.println(Timer1OffHour);
  Firebase.setInt(firebaseData2, "/Timer/Timer1Off/Hour", Timer1OffHour);
}
void handleTimer1OffMin() {
  Timer1OffMin = server.arg(0).toInt();
  Serial.print("User Set Timer1 Off Min:");
  Serial.println(Timer1OffMin);
  Firebase.setInt(firebaseData3, "/Timer/Timer1Off/Min", Timer1OffMin);
  Firebase.setBool(firebaseData2, "/Timer/Timer1Off/State", true);
  //  vTaskSuspend(Alarm_Task_Handle);
  //  vTaskDelete(Alarm_Task_Handle);
  GetStateAlarm();
}
//===============================================================
// Read Timer1 From FireBase
//===============================================================
void GetDataDB() {
  if (Firebase.getString(firebaseData1, "/Relay/Relay1")) {
    if (firebaseData1.dataType() == "string") {
      Relay1 = firebaseData1.stringData();
      Serial.print("Get Relay1 DB :");
      Serial.println(Relay1);
      if (Relay1 == "ON") {
        digitalWrite(LED, LOW);
      }
      else {
        digitalWrite(LED, HIGH);
      }
    }
  }
  if (Firebase.getInt(firebaseData2, "/Timer/Timer1On/Hour")) { // True if can find path
    if (firebaseData2.dataType() == "int") {
      Timer1OnHour = firebaseData2.intData();
      Serial.print("Get Timer1OnHour DB :");
      Serial.println(Timer1OnHour);
    }
  }
  if (Firebase.getInt(firebaseData3, "/Timer/Timer1On/Min")) { // True if can find path
    if (firebaseData3.dataType() == "int") {
      Timer1OnMin = firebaseData3.intData();
      Serial.print("Get Timer1OnMin DB :");
      Serial.println(Timer1OnMin);
    }
  }
  if (Firebase.getInt(firebaseData2, "/Timer/Timer1Off/Hour")) { // True if can find path
    if (firebaseData2.dataType() == "int") {
      Timer1OffHour = firebaseData2.intData();
      Serial.print("Get Timer1OffHour DB :");
      Serial.println(Timer1OffHour);
    }
  }
  if (Firebase.getInt(firebaseData3, "/Timer/Timer1Off/Min")) { // True if can find path
    if (firebaseData3.dataType() == "int") {
      Timer1OffMin = firebaseData3.intData();
      Serial.print("Get Timer1OffMin DB :");
      Serial.println(Timer1OffMin);
    }
  }
  String data = "{     \"Led1\":\"" + Relay1
                + "\", \"Hour_On\":\"" + String(Timer1OnHour)
                + "\", \"Min_On\":\"" + String(Timer1OnMin)
                + "\",\"Hour_Off\":\"" + String(Timer1OffHour)
                + "\", \"Min_Off\":\"" + String(Timer1OffMin)
                + "\"}";
  server.send(200, "text/plane", data); //Send web page
}
//===============================================================
// Funtion GetStateAlarm Form DB
//===============================================================
void GetStateAlarm() {
  if (Firebase.getBool(firebaseData2, "/Timer/Timer1On/State")) { // True if can find path
    if (firebaseData2.dataType() == "boolean") {
      AlarmOn = firebaseData2.boolData();
      Serial.print("Get AlarmOn DB :");
      Serial.println(AlarmOn);
    }
  }
  if (Firebase.getBool(firebaseData3, "/Timer/Timer1Off/State")) { // True if can find path
    if (firebaseData3.dataType() == "boolean") {
      AlarmOff = firebaseData3.boolData();
      Serial.print("Get AlarmOff DB :");
      Serial.println(AlarmOff);
    }
  }
  if (AlarmOn) {
    Serial.print("Set AlarmOn ");
    StateTaskAlarmOn = true ;
    xTaskCreate(&AlarmOn_Task, "Alarm_Task", 8192, NULL, 3, &AlarmOn_Task_Handle);
  }
  if (AlarmOff) {
    Serial.print("Set AlarmOn ");
    StateTaskAlarmOn = true ;
    xTaskCreate(&AlarmOff_Task, "Alarm_Task", 8192, NULL, 3, &AlarmOff_Task_Handle);
  }
}
//===============================================================
// Funtion GetStateAlarm Form DB
//===============================================================
void handleACS712_Calibrate() {
  Serial.print("ACS712 Calibrate...");
  Serial.println(Sensor.ACS712_Calibrate());
}
//===============================================================
// Setup
//===============================================================

void setup(void) {
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  Serial.println();
  Serial.println("Booting Sketch...");
  /*
    //ESP32 As access point
    WiFi.mode(WIFI_AP); //Access Point mode
    WiFi.softAP(ssid, password);
  */
  //ESP32 connects to your wifi -----------------------------------
  WiFi.mode(WIFI_STA); //Connectto your wifi
  WiFi.begin(ssid, password);

  Serial.println("Connecting to ");
  Serial.print(ssid);

  //Wait for WiFi to connect
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.print(".");
  }

  //If connection successful show IP address in serial monitor
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  //IP address assigned to your ESP
  //----------------------------------------------------------------
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
  Firebase.setReadTimeout(firebaseData1, 1000 * 15);
  Firebase.setReadTimeout(firebaseData3, 1000 * 15);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
  }


  xTaskCreate(&ReadAsc712_Task, "ReadAsc712_Task", 1024, NULL, 1, &ReadAsc712_Task_Handle);
  xTaskCreate(&Webserver_Task, "Webserver_Task", 16384, NULL, 2, &Webserver_Task_Handle);
  //xTaskCreate(&DisplayOled_Task, "DisplayOled_Task", 8000, NULL, 3, &DisplayOled_Task_Handle);
  GetDataDB();
  GetStateAlarm();
}
void loop(void) {
}

//===============================================================
// Webserver_Task
//===============================================================
void Webserver_Task(void *p) {
  Serial.println("Start Webserver Task");
  server.on("/", handleRoot);      //This is display page
  server.on("/readADC", handleADC);//To get update of ADC Value only
  server.on("/setLED1", handleLED1);
  server.on("/setTimer1OnHour",   handleTimer1OnHour); // get From Sever
  server.on("/setTimer1OnMin",    handleTimer1OnMin); // get From Sever
  server.on("/setTimer1OffHour",  handleTimer1OffHour); // get From Sever
  server.on("/setTimer1OffMin",   handleTimer1OffMin); // get From Sever
  server.on("/setResetTimer",     handleResetTimer); // get From Sever
  server.on("/getDataDB", GetDataDB); // get From DB
  server.on("/setACS712_Calibrate",   handleACS712_Calibrate); // get From Sever
  server.begin();                  //Start server
  Serial.println("HTTP server started");
  while (1) {
    server.handleClient();
    vTaskDelay(10);
  }
}
//===============================================================
// ReadAsc712_Task
//===============================================================
void ReadAsc712_Task(void *p) {
  Serial.println("Start ReadAsc712 Task");
  Serial.print("ACS712 Calibrate...");
  Serial.println(Sensor.ACS712_Calibrate());
  while (1) {
    Amp_Double = Sensor.GetCurrent();
    Amp_String = String(Amp_Double);
    Watt = String(Sensor.GetWatt(Amp_Double));
    vTaskDelay(3000);
  }
}

//===============================================================
// DisplayOled_Task
//===============================================================
//void DisplayOled_Task(void *p) {
//    Serial.println("Start Display Task");
//    Serial.println("Address OLED : 0x3c");
////    display.clearDisplay();
////    display.setTextSize(1);
////    display.setTextColor(WHITE);
////    display.setCursor(0,0);
////    display.print("AC Plug IOT");
////    display.display();
//  while (1) {/*
//    display.setCursor(0,0);
//    display.clearDisplay();
//    display.setTextSize(1);
//    display.setTextColor(WHITE);
//    display.print("IP: ");
//    display.println(WiFi.localIP());
//    display.print("ADC:");
//    display.println(Asc712_ADC);
//    display.print("Replay1:");
//    display.println(Relay1);
//    display.print("Replay2:");
//    display.println(Relay2);
//    display.display();*/
//    vTaskDelay(2000);
//  }
//}
//===============================================================
// AlarmOn_Task
//===============================================================
void AlarmOn_Task(void *p) {
  AMC TimerOn;
  TimerOn.SetAlarm(Timer1OnHour, Timer1OnMin);
  Serial.print("##### SetAlarm ON :");
  Serial.print(Timer1OnHour);
  Serial.print(Timer1OnMin);
  Serial.println("#####");
  TimerOn.DisplayNowTime();
  bool StateAlarm_On = false ;
  while (1) {
    if (TimerOn.GetStateAlarm() && !StateAlarm_On) {
      Serial.println("***** AlarmOn Now!!! *****");
      Firebase.setString(firebaseData1, "/Relay/Relay1", "ON"); // send Data to Firebase
      Serial.println("Set Relay1 : ON");
      StateAlarm_On = true ;
      if (Firebase.getString(firebaseData2, "/Relay/Relay1")) {
        if (firebaseData1.dataType() == "string") {
          Relay1 = firebaseData2.stringData();
          Serial.print("Get Relay1 DB :");
          Serial.println(Relay1);
          if (Relay1 == "ON") {
            digitalWrite(LED, LOW);
          }
          else {
            digitalWrite(LED, HIGH);
          }
        }
      }
      else
      {
        Serial.println("FAILED");
        Serial.println("REASON: " + firebaseData2.errorReason());
        Serial.println("------------------------------------");
        Serial.println();
      }
    }
    if (TimerOn.GetNowMin() > Timer1OnMin) {
      StateAlarm_On = false ;
    }

    vTaskDelay(1000);
  }
}
//===============================================================
// AlarmOff_Task
//===============================================================
void AlarmOff_Task(void *p) {
  AMC TimerOff;
  TimerOff.SetAlarm(Timer1OffHour, Timer1OffMin);
  Serial.print("##### SetAlarm OFF :");
  Serial.print(Timer1OffHour);
  Serial.print(Timer1OffMin);
  Serial.println("#####");
  TimerOff.DisplayNowTime();
  bool StateAlarm_Off = false ;
  while (1) {
    if (TimerOff.GetStateAlarm() && !StateAlarm_Off) {
      Serial.println("***** AlarmOff Now!!! *****");
      Firebase.setString(firebaseData1, "/Relay/Relay1", "OFF"); // send Data to Firebase
      Serial.println("Set Relay1 : OFF");
      StateAlarm_Off = true ;
      if (Firebase.getString(firebaseData2, "/Relay/Relay1")) {
        if (firebaseData1.dataType() == "string") {
          Relay1 = firebaseData2.stringData();
          Serial.print("Get Relay1 DB :");
          Serial.println(Relay1);
          if (Relay1 == "OFF") {
            digitalWrite(LED, HIGH);
          }
          else {
            digitalWrite(LED, LOW);
          }
        }
      }
      else
      {
        Serial.println("FAILED");
        Serial.println("REASON: " + firebaseData2.errorReason());
        Serial.println("------------------------------------");
        Serial.println();
      }
    }
    if (TimerOff.GetNowMin() > Timer1OffMin) {
      StateAlarm_Off = false ;
    }

    vTaskDelay(1000);
  }
}
