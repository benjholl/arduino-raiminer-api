
/*******************************************************************
 *  An example of getting mining info from raiminer.com            *
 *                                                                 *
 *  Written by Benjamin Holl                                       *
 *******************************************************************/
 
#include <RaiMinerApi.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>


//------- Replace the following! ------
const char ssid[] = "SSID";       // your network SSID (name)
const char password[] = "PASSWORD";  // your network key
const char xrb_adress[] = "xrb_ADRESS"; // xrb adress used for mining


WiFiClient client;
RaiMinerApi rai(client, xrb_adress);

unsigned long update_rate = 30000; 
unsigned long update_duetime = 0;



void setup() {
  Serial.begin(115200);

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


void printStat(bool autoWithdraw) {
  Serial.println("---------------------------------");

  // autoWithdraw is optional, rai.GetStats() will not withdraw automatically and just request the stats
  MinerStats stats = rai.GetStats(autoWithdraw);
  
  if (stats.error == "") {
    Serial.print("Total Mined: ");
    Serial.print(stats.xrb_mined);
    Serial.println(" uxrb ");
    
    Serial.print("Withdrawn: ");
    Serial.print(stats.xrb_withdrawn);
    Serial.println(" uxrb ");
    
    Serial.print("Balance: ");
    Serial.print(stats.xrb_balance);
    Serial.print(" uxrb (");
    Serial.print(stats.balance);
    Serial.println(" Hashes)");
  } else {
    Serial.print("Error getting data: ");
    Serial.println(stats.error);
  }
}


void loop() {
  unsigned long timeNow = millis();
  if ((timeNow > update_duetime))  {
    printStat(false); // false: no withdraws; true: one automatic withdraw attempt every 61 minutes
    update_duetime = timeNow + update_rate;
  }
}
