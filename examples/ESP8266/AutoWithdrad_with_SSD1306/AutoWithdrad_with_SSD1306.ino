
/*******************************************************************
 *  An example of getting mining info from raiminer.com            *
 *                                                                 *
 *  Written by Benjamin Holl                                       *
 *******************************************************************/
 
#include <RaiMinerApi.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>

// SSD1306 OLED Libraries
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
Adafruit_SSD1306 display(D0);


//------- Replace the following! ------
const char ssid[] = "SSID";       // your network SSID (name)
const char password[] = "PASSWORD";  // your network key
const char xrb_adress[] = "xrb_ADRESS"; // xrb adress used for mining


WiFiClient client;
RaiMinerApi rai(client, xrb_adress);

unsigned long update_rate = 30000; 
unsigned long update_duetime = 0;


const unsigned char PROGMEM pick [] = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x7E, 0x00, 0x00, 0x00, 0x77, 0xC1, 0x80, 0x00, 0x1C, 0x7F, 0xE0,
0x00, 0x07, 0x03, 0xE0, 0x00, 0x01, 0x80, 0xC0, 0x00, 0x03, 0x81, 0xC0, 0x00, 0x07, 0xEC, 0x60,
0x00, 0x1C, 0xFF, 0x30, 0x00, 0x73, 0x81, 0xD8, 0x01, 0xCE, 0x00, 0x7C, 0x07, 0x38, 0x00, 0x18,
0x1C, 0xE0, 0x00, 0x00, 0x33, 0x80, 0x00, 0x00, 0x3E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};


void setup() {
  Serial.begin(115200);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
 

  // Set WiFi to station mode and disconnect from an AP if it was Previously
  // connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  // Attempt to connect to Wifi network:
  Serial.print("Connecting Wifi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  IPAddress ip = WiFi.localIP();
  Serial.println(ip);
}


void displayStat(bool autoWithdraw) {
  
  // autoWithdraw is optional, rai.GetStats() will not withdraw automatically and just request the stats
  MinerStats stats = rai.GetStats(autoWithdraw);  
  if (stats.error != "") return;
  
  char string[21];
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(2);      
  display.setCursor(15,2);  
  sprintf(string, "%d.%06d",stats.xrb_withdrawn/1000000,stats.xrb_withdrawn%1000000);
  display.println(string);    
  display.drawBitmap(15, 17,  pick, 32, 16, 1);
  display.setTextSize(1);    
  display.setCursor(53,17);  
  sprintf(string, "+%03d uxrb",stats.xrb_balance);
  display.println(string);
  display.setCursor(53,25);  
  if (stats.auto_withdraw_timer > 0)
     sprintf(string, "in %02d min",stats.auto_withdraw_timer/60);
  else
     sprintf(string, "pending");
  display.println(string);  
  display.display();
}


void loop() {
  unsigned long timeNow = millis();
  if ((timeNow > update_duetime))  {
    displayStat(true); // false: no withdraws; true: one automatic withdraw attempt every 61 minutes
    update_duetime = timeNow + update_rate;
  }
}