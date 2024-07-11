#include <WiFi.h>
#include <WebServer.h>
#include <WiFiClient.h>
#include <ESPmDNS.h>
#include <TridentTD_LineNotify.h>//line
#include <IOXhop_FirebaseESP32.h>
#include "RC522G4.h"
#include <ESP32Servo.h>
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"

// Set these to run example.
#define FIREBASE_HOST "https://esp32banking-default-rtdb.asia-southeast1.firebasedatabase.app/"
#define FIREBASE_AUTH "YBzM3S0N83Q0TqMq3cZJNGvIHh3WsyDNaMGbuCl9"
//#define WIFI_SSID "Mimick"
//#define WIFI_PASSWORD "11111111"
#define LINE_TOKEN "cJmbpSXjEwhuwrWYSPkT98bN2yTcgbYGWDjI0uKHpJL"//line

#define RST_PIN 27
#define SS_PIN 5

const char *ssid = "Mimick";
const char *password = "11111111";
WebServer server(80);
const int pinIr5b = 17;
const int pinIr10b = 16;

int IRvalue5b = 0;
int IRvalue10b = 0;

// int coinCount = 0;

const int pinIrStart = 17;
int IRvalueStart = 0;

int n5 = 1;
int n10 = 1;

RC522G4 rc522g4(SS_PIN, RST_PIN);

Servo servo1;
Servo servo2;

bool doorIsOpen = true;

void openDoor() {
  servo1.writeMicroseconds(1000);
  Serial.print("Servo1 หมุน 1000 ");
  servo2.writeMicroseconds(2000);
  Serial.print("Servo2 หมุน 2000 ");
  delay(1000);  // เปิดประตูโดยหมุน Servo2 ให้อยู่ที่ 0 องศา
  servo1.writeMicroseconds(1500);
  // Serial.print("Servo1 หมุน 1500 ");
  servo2.writeMicroseconds(1500);
  // Serial.print("Servo2 หมุน 1500 ");
  Serial.print("door boolean After End void openDoor: ");
  doorIsOpen = !doorIsOpen;
}

void closeDoor() {
  servo1.writeMicroseconds(2000);
  Serial.print("Servo1 หมุน 2000 ");
  servo2.writeMicroseconds(1000);
  Serial.print("Servo2 หมุน 1000 ");
  delay(1000);
  servo1.writeMicroseconds(1500);
  // //Serial.print("Servo2 หมุน 1500 Closed ");
  servo2.writeMicroseconds(1500);
  // Serial.print("Servo2 หมุน 1500 closed");
  Serial.print("door boolean After End void closeDoor: ");
  doorIsOpen = !doorIsOpen;
}

void irSensor() {

  IRvalue5b = digitalRead(pinIr5b);
  IRvalue10b = digitalRead(pinIr10b);
  IRvalueStart = digitalRead(pinIrStart);
  
  if (IRvalue5b == 0) {
    Firebase.setInt("5 Bath/Balance", n5*5);
    if (Firebase.failed()) {  // ทำการตรวจสอบ การส่งค่าไปยัง Firebase ว่า Error ไหม
      Serial.print("setting /number failed:"); 
      Serial.println(Firebase.error());   
      return; 
    }
    Serial.println("It was a 5p coin that was detected");
    // coinDetected();
    amount();
    n5++;
  }

  if (IRvalue10b == 0) {
    Firebase.setInt("10 Bath/Balance", n10*10);
    if (Firebase.failed()) {  // ทำการตรวจสอบ การส่งค่าไปยัง Firebase ว่า Error ไหม
      Serial.print("setting /number failed:"); 
      Serial.println(Firebase.error());
      return; 
    }
    Serial.println("It was a 10p coin that was detected");
    // coinDetected();
    amount();
    n10++;
  }
}

// void coinDetected() {
//     coinCount ++;
// }

void amount(){
  
  int bathValue = 0;
  int bath5Value = 0;
  int bath10Value = 0;

  bath5Value = Firebase.getInt("5 Bath/Balance");
  bath10Value = Firebase.getInt("10 Bath/Balance");

  if (Firebase.failed()) {
    Serial.print("Error reading from Firebase: ");
    Serial.println(Firebase.error());
    return;
  }

  bathValue = bath5Value + bath10Value;
  Firebase.setInt("Sum/Balance",bathValue);

  String messageValue = "มีเงินทั้งหมด: " + String(bathValue) + " ฿";
  String messageValue5 = "แบ่งเป็น 5฿ " + String(bath5Value) +" ฿";
  String messageValue10 = "แบ่งเป็น 10฿ " + String(bath10Value) +" ฿";

  Serial.print("5 Bath Balance: ");
  Serial.println(bath5Value);
  Serial.print("10 Bath Balance: ");
  Serial.println(bath10Value);
  Serial.print("Amount: ");
  Serial.println(bathValue);//line
  LINE.notify(messageValue);//line
  LINE.notify(messageValue5);//line
  LINE.notify(messageValue10);//line
  LINE.notifySticker(6136,10551387);
}

void removeDatabase(){
  Firebase.remove("5 Bath/Balance");
  Firebase.remove("10 Bath/Balance");
  Firebase.remove("Sum/Balance");
  if (Firebase.failed()) {  // ทำการตรวจสอบ การส่งค่าไปยัง Firebase ว่า Error ไหม
    Serial.print("setting /number failed:"); 
    Serial.println(Firebase.error());
    return; 
  }
  LINE.notify("เจ้านายได้ถอนแล้วค่ะ");
  LINE.notifySticker(6325,10979905);
}

void handle_OnConnect() {
  int value= 0;
  int value5=0;
  int value10=0;
  value = Firebase.getInt("Sum/Balance");
  value5 = Firebase.getInt("5 Bath/Balance");
  value10 = Firebase.getInt("10 Bath/Balance");
  server.send(200, "text/html", SendHTML(value,value5,value10));
}
String SendHTML(int value,int value5,int value10) {
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr += "<head><meta http-equiv=\"refresh\"content=\"1\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr += "<title>LED Control</title>\n";
  ptr += "<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr += "body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;} h3 {color: #444444;margin-bottom: 50px;}\n";
  ptr += ".button {display: block;width: 80px;background-color: #3498db;border: none;color: white;padding: 13px 30px;text-decoration: none;font-size: 25px;margin: 0px auto 35px;cursor: pointer;border-radius: 4px;}\n";
  ptr += ".button-on {background-color: #3498db;}\n";
  ptr += ".button-on:active {background-color: #2980b9;}\n";
  ptr += ".button-off {background-color: #34495e;}\n";
  ptr += ".button-off:active {background-color: #2c3e50;}\n";
  ptr += "p {font-size: 14px;color: #888;margin-bottom: 10px;}\n";
  ptr += "</style>\n";
  ptr += "</head>\n";
  ptr += "<body>\n";
  ptr += "<h1>Banking</h1>\n";
  
  ptr +="<h1>Amount: " +String(value) +"</h1>\n";
  ptr +="<h1>5 Bath: " +String(value5) +"</h1>\n";
  ptr +="<h1>10 Bath: " +String(value10) +"</h1>\n";

  ptr += "</body>\n";
  ptr += "</html>\n";
  return ptr;
}

void setup() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable detector
  Serial.begin(115200);
  Serial.println(LINE.getVersion());
  // connect to wifi.
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp32")) {
    Serial.println("MDNS responder started");
   }
  server.on("/", handle_OnConnect);

  server.begin();
  Serial.println("HTTP server started");

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

  rc522g4.begin();
  servo1.attach(26);
  servo2.attach(25);
  pinMode(pinIr10b,INPUT);
  pinMode(pinIrStart,INPUT);
  LINE.setToken(LINE_TOKEN);//line
}

void loop() {
  irSensor();
  //rc522g4.readUID();
  if (rc522g4.checkCardUID(0xA1, 0xC0, 0xD2, 0xCF)) {
    Serial.println("Read Success");
    if (doorIsOpen == true) {
      openDoor();
    } else if(doorIsOpen == false) {
      closeDoor();
      removeDatabase();
    }
  }
  server.handleClient();
}









