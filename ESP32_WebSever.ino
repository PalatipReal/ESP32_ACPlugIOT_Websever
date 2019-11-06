//************ Include WiFi WiFiClient WebServer ************
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>

//************ Include FireBaseEsp32 And Define Host,Auth ************
#include <FirebaseESP32.h>
#define FIREBASE_HOST "https://embedapp-9157e.firebaseio.com" //Do not include https:// in FIREBASE_HOST
#define FIREBASE_AUTH "P2aAecFYcDqsdHowVHZv1wJUsJivlpBSh82DN9ll"

//************ Include Index.h For Web Page ************
#define LED 2
#include "index.h"  //Web page header file

//************ Include PCF8591 For Read Analog From Acs712 ************
#include "PCF8591.h"
#define PCF8591_I2C_ADDRESS 0x48
PCF8591 pcf8591(PCF8591_I2C_ADDRESS);

//************ Start WebServer Port80 And Firebase ************
WebServer server(80);
FirebaseData firebaseData1;
FirebaseData firebaseData2;
bool LED1status1 = LOW;
bool LED2status = LOW;

//************ Enter your SSID and PASSWORD ************
//const char* ssid       = "iot";
//const char* password   = "C0mputinG";
//const char* ssid       = "iot-lab";
//const char* password   = "computing";
const char* ssid       = "Renat";
const char* password   = "nattapong2539nat";

//===============================================================
// This routine is executed when you open its IP in browser
//===============================================================
void handleRoot() {
 String s = MAIN_page; //Read HTML contents
 server.send(200, "text/html", s); //Send web page
}
 
void handleADC() {
 int a = pcf8591.analogRead(AIN0);
 String adcValue = String(a);
 
 server.send(200, "text/plane", adcValue); //Send ADC value only to client ajax request
}
void handleLED1() {
 String ledState1 = "OFF";
 String t_state = server.arg("LEDstate1"); //Refer  xhttp.open("GET", "setLED?LEDstate="+led, true);
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
 Firebase.setString(firebaseData2,"/Relay/Relay1", t_state); // send Data to Firebase
 server.send(200, "text/plane", ledState1); //Send web page
}
void handleLED2() {
 String ledState2 = "OFF";
 String t_state = server.arg("LEDstate2"); //Refer  xhttp.open("GET", "setLED?LEDstate="+led, true);
 Serial.print("Relay 2 :");
 Serial.println(t_state);
 Serial.println("");
 if(t_state == "1")
 {
  digitalWrite(LED,LOW); //LED ON
  ledState2 = "ON"; //Feedback parameter
 }
 else
 {
  digitalWrite(LED,HIGH); //LED OFF
  ledState2 = "OFF"; //Feedback parameter  
 }
 Firebase.setString(firebaseData2,"/Relay/Relay2", t_state); // send Data to Firebase
 server.send(200, "text/plane", ledState2); //Send web page
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

  server.on("/", handleRoot);      //This is display page
  server.on("/readADC", handleADC);//To get update of ADC Value only
  server.on("/setLED1", handleLED1);
  server.on("/setLED2", handleLED2);
  server.begin();                  //Start server
  Serial.println("HTTP server started");

}

//===============================================================
// This routine is executed when you open its IP in browser
//===============================================================
void loop(void){
  server.handleClient();
  delay(1);
}
