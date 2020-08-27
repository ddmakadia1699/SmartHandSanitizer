/*
   Component List :
   1. Pump : https://www.ebay.com/c/15012640609#oid253232507165
   2. IR Sensor : https://robotantra.com/robotantra-ir-proximity-sensor-for-line-follower-and-obstacle-sensing-robots.html
   3. RFID Reader/Writer : https://bm-es.com/product/rfid-reader-writer-rc522/?gclid=EAIaIQobChMIksmsz_up6gIVkn0rCh2YEQ8mEAYYBSABEgJcovD_BwE
   4. MLX90614 : https://www.adafruit.com/product/1748
*/

#include <Adafruit_MLX90614.h>
#include <NTPClient.h>
#include <Servo.h>
#include <WiFiUdp.h>
#include <SPI.h>
#include <MFRC522.h>
#include <WiFiNINA.h>
#include "Firebase_Arduino_WiFiNINA.h"


const long utcOffsetInSeconds = 19800;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

#define FIREBASE_HOST "hs300620-81959.firebaseio.com"
#define FIREBASE_AUTH "4UV274Ppbz10pIGhz6AJevIDCSBoVbvgIfVgv7Wy"

const char *host = "api.pushingbox.com";

#define SS_PIN 10
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.

#define red 0     // Red led Connected on pin D1 
#define green 1   // Green led Connected on pin D1
#define buzzer 7  
#define pwPin 3   // PWM Pin for maxsonar sensor
#define door 2    // Servo motor connected on pin D2 to open the door 
#define potPin A2 // Potentiometer Connected on Analog Pin A2
#define pump 4    // DC Senitizer Pump connected on pin D4 

long pulse, inches; //variables needed to store values of Sonar sensor 
Servo servo;   // create servo object to control a servo
boolean a = true;
int val=0;

char ssid[] = "DDIK Makadia";        // your network SSID
char pass[] = "kidd123456789";    // your network password
int status = WL_IDLE_STATUS;     // the Wifi radio's status

Adafruit_MLX90614 mlx = Adafruit_MLX90614();

int noOfEm = 1;
String UserID[1] = {"BD 31 15 2B"};
String path = "/Test";
FirebaseData firebaseData;

void setup()
{
  Serial.begin(115200);   // Initiate a serial communication
  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522
  mlx.begin();     // Initiate MLX90614
  servo.attach(door); // attaches the servo on pin 2
  Serial.println("Approximate your card to the reader...");
  Serial.println();
  timeClient.begin();
  pinMode(red, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(buzzer,OUTPUT);
  pinMode(pump,OUTPUT);

  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    while (true);
  }
  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);
    delay(300);
  }
  Serial.print("You're connected to the network");
  printCurrentNet();
  printWifiData();

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH, ssid, pass);
  Firebase.reconnectWiFi(true);
}

void loop()
{
  if ( ! mfrc522.PICC_IsNewCardPresent())
  {
    return;
  }
  if ( ! mfrc522.PICC_ReadCardSerial())
  {
    return;
  }
  Serial.print("UID tag :");
  String content = "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++)
  {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  content.toUpperCase();
  for (int i = 0; i < noOfEm; i++) {
    if (content.substring(1) == UserID[i]) //change here the UID of the cards that you want to give access
    {
      digitalWrite(red, LOW);
      digitalWrite(green, HIGH);
      servo.write(180);
      digitalWrite(buzzer,HIGH);
      delay(1000);
      digitalWrite(buzzer,LOW);
      servo.write(0);
      Serial.print("Ambient = ");
      Serial.print(mlx.readAmbientTempC());
      Serial.print("*C\tTarget= ");
      Serial.print(mlx.readObjectTempC());
      Serial.println("*C");
      timeClient.update();
      int day1 = timeClient.getDay(); //0 means sunday 
      int hours = timeClient.getHours();
      int minutes = timeClient.getMinutes();
      int seconds = timeClient.getSeconds();
      String time1 = String(day1) + " " + String(hours) + ":" + String(minutes) + ":" + String(seconds);
      Serial.println(time1);
      Firebase.setInt(firebaseData, path + "/" + (i + 1) + "/" + time1 , mlx.readObjectTempC());
      WiFiClient client;
      const int httpPort = 80;
      if (!client.connect(host, httpPort)) {
        Serial.println("connection failed");
        return;
      }
      String url = "/pushingbox?";
      url += "devid=";
      url += "v5435C84E0614CC7";
      url += "&Data=" + i;
      Serial.println(url);
      client.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: close\r\n\r\n");
      unsigned long timeout = millis();
      while (client.available() == 0) {
        if (millis() - timeout > 4000) {
          client.stop();
          return;
        }
      }
      while (client.available()) {
        String line = client.readStringUntil('\r');
        Serial.print("Data Sent!");
      }
    }
    else {
      digitalWrite(red, HIGH);
      digitalWrite(green, LOW);
      digitalWrite(buzzer,HIGH);
      Serial.println(" Access denied");
    }
  }
  
  pinMode(pwPin, INPUT);  //Used to read the pulse that is being sent by the MaxSonar device.
  pulse = pulseIn(pwPin, HIGH);
  inches = pulse / 147;
  Serial.print(inches);
  Serial.println("in, ");
  //delay(500);
  val = analogRead(potPin); // Potentiometer Reading Value

  if (inches < 4)
  {
    while (a == true)
    {
      digitalWrite(pump, HIGH);
      delay(val);
      digitalWrite(pump,LOW);
      a = false;
      break;
    }
    while (a == false)
    {
      digitalWrite(pump,LOW);
      delay(1500);
      a = true;
      break;
    }
    
  }
}

void printWifiData() {
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  byte mac[6];
  WiFi.macAddress(mac);
  Serial.print("MAC address: ");
  printMacAddress(mac);
}
void printCurrentNet() {
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
  byte bssid[6];
  WiFi.BSSID(bssid);
  Serial.print("BSSID: ");
  printMacAddress(bssid);
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.println(rssi);
}
void printMacAddress(byte mac[]) {
  for (int i = 5; i >= 0; i--) {
    if (mac[i] < 16) {
      Serial.print("0");
    }
    Serial.print(mac[i], HEX);
    if (i > 0) {
      Serial.print(":");
    }
  }
  Serial.println();
}
