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
#define LED2 3
String ledState1 = "OFF";
String ledState2 = "OFF";
String Relay1 = "" ;
String Relay2 = "" ;
#include "index.h"  //Web page header file
//===============================================================
//  Include PCF8591 For Read Analog From Acs712
//===============================================================
#include "PCF8591.h"
#define PCF8591_I2C_ADDRESS 0x48
PCF8591 pcf8591(PCF8591_I2C_ADDRESS);
String Asc712_ADC = "0";
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
 server.send(200, "text/plane", Asc712_ADC); //Send ADC value only to client ajax request
}
//===============================================================
// Read LED1 From WebServer
//===============================================================
void handleLED1() {
 String t_state = server.arg("LEDstate1"); //Refer  xhttp.open("GET", "setLED?LEDstate="+led, true);
 Serial.print("User Set ");
 Serial.print("Relay 1 :");
 Serial.println(t_state);
 Serial.println("");
 if(t_state == "1")
 {
  digitalWrite(LED,LOW); //LED ON
  ledState1 = "ON"; //Feedback parameter
 }
 else
 {
  digitalWrite(LED,HIGH); //LED OFF
  ledState1 = "OFF"; //Feedback parameter  
 }
 Relay1=ledState1;
 Firebase.setString(firebaseData2,"/Relay/Relay1", ledState1); // send Data to Firebase
 server.send(200, "text/plane", ledState1); //Send web page
}
//===============================================================
// Read LED2 From WebServer
//===============================================================
void handleLED2() {
 String t_state = server.arg("LEDstate2"); //Refer  xhttp.open("GET", "setLED?LEDstate="+led, true);
 Serial.print("User Set ");
 Serial.print("Relay 2 :");
 Serial.println(t_state);
 Serial.println("");
 if(t_state == "1")
 {
  digitalWrite(LED2,LOW); //LED ON
  ledState2 = "ON"; //Feedback parameter
 }
 else
 {
  digitalWrite(LED2,HIGH); //LED OFF
  ledState2 = "OFF"; //Feedback parameter  
 }
 Relay2=ledState2;
 Firebase.setString(firebaseData2,"/Relay/Relay2", ledState2); // send Data to Firebase
 server.send(200, "text/plane", ledState2); //Send web page
}
//===============================================================
// Get Data from firebase and send to Webserver
//===============================================================
void GetLED1(){ // Get Data From Firebase 
  if (Firebase.getString(firebaseData1,"/Relay/Relay1")) { // True if can find path
    if (firebaseData1.dataType() == "string") {
      Relay1 = firebaseData1.stringData();
      Serial.print("Get Relay1 DB :");
      Serial.println(Relay1);
      server.send(200, "text/plane", Relay1);
      if(Relay1== "ON"){
        digitalWrite(LED,LOW);
        }
      else{
        digitalWrite(LED,HIGH);
        }
    }
  } else {
    Serial.println(firebaseData1.errorReason());
  }
}
//===============================================================
// Get Data from firebase and send to Webserver
//===============================================================
void GetLED2(){ // Get Data From Firebase 
  if (Firebase.getString(firebaseData3,"/Relay/Relay2")) { // True if can find path
    if (firebaseData3.dataType() == "string") {
      Relay2 = firebaseData3.stringData();
      Serial.print("Get Relay2 DB :");
      Serial.println(Relay2);
      server.send(200, "text/plane", Relay2);
      if(Relay2== "ON"){
        digitalWrite(LED2,LOW);
        }
      else{
        digitalWrite(LED2,HIGH);
        }
    }
  } else {
    Serial.println(firebaseData3.errorReason());
  }
}
//===============================================================
// Read Timer1 From WebServer
//===============================================================
void handleTimer1(){
  String Timer = server.arg(0); //Refer  xhttp.open("GET","getTimer1"+Timer1 ,true);
  Serial.print("User Set Timer1:");
  Serial.println(Timer);
}
//===============================================================
// Setup
//===============================================================

void setup(void){
  Serial.begin(115200);
  pinMode(LED,OUTPUT); 
  pcf8591.begin(); 
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
  while(WiFi.waitForConnectResult() != WL_CONNECTED){      
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
    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    }

  
  xTaskCreate(&ReadAsc712_Task, "ReadAsc712_Task", 1024, NULL, 1, &ReadAsc712_Task_Handle);
  xTaskCreate(&Webserver_Task, "Webserver_Task", 16384, NULL, 2, &Webserver_Task_Handle);
  xTaskCreate(&DisplayOled_Task, "DisplayOled_Task", 8000, NULL, 3, &DisplayOled_Task_Handle);
  GetLED1();
  GetLED2();
}
void loop(void){
}

//===============================================================
// Webserver_Task 
//===============================================================
void Webserver_Task(void *p) {
  Serial.println("Start Webserver Task");
  server.on("/", handleRoot);      //This is display page
  server.on("/readADC", handleADC);//To get update of ADC Value only
  server.on("/setLED1", handleLED1);
  server.on("/setLED2", handleLED2);
  server.on("/getLED1", GetLED1); // get From DB
  server.on("/getLED2", GetLED2); // get From DB
  server.on("/setTimer1", handleTimer1); // get From Sever
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
  while (1) {
    Asc712_ADC = String(pcf8591.analogRead(AIN0));
    /*
    Lib ASC712 Here...
    *--- Conver adc-Amp , amp-watt 
    */
    vTaskDelay(1000);
  }
}
void DisplayOled_Task(void *p) {
    Serial.println("Start Display Task");
    Serial.println("Address OLED : 0x3c");
//    display.clearDisplay();
//    display.setTextSize(1);
//    display.setTextColor(WHITE);
//    display.setCursor(0,0);
//    display.print("AC Plug IOT");
//    display.display();
  while (1) {
    display.setCursor(0,0);
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.print("IP: ");
    display.println(WiFi.localIP());
    display.print("ADC:");
    display.println(Asc712_ADC);
    display.print("Replay1:");
    display.println(Relay1);
    display.print("Replay2:");
    display.println(Relay2);
    display.display();
    vTaskDelay(2000);
  }
}
