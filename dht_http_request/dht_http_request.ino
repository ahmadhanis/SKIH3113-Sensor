#include <ESP8266WiFi.h>
#include <dht.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

#define D4 2  //DHT11 Sensor connected to D4
const char* ssid = "UUMWiFi_Guest";
const char* pass = "";
int hum = 0, temp = 0;
String relay = "Off", prevRelay = "On";
String timestamp = "", datestamp = "", hourstamp = "";
unsigned long sendDataPrevMillis = 0;
int count = 0;
String serverName = "http://slumberjer.com/iottraining/";

dht DHT;
WiFiUDP ntpUDP;
const long utcOffsetInSeconds = 28800;  //UTC 8 offset UTC(8+) * 60*60
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(100);
  WiFi.begin(ssid, pass);
  pinMode(12, OUTPUT);
  delay(100);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  timeClient.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  if (millis() - sendDataPrevMillis > 10000 || sendDataPrevMillis == 0) {
    count++;
    getDHT();
    sendDataPrevMillis = millis();
    // ntp get date/time/hour
    datestamp = getDate();
    timestamp = getTime();
    hourstamp = getHour();
    if (WiFi.status() == WL_CONNECTED) {
      WiFiClient client;
      HTTPClient http;
      String httpReqStr = serverName + "dht.php?temp=" + temp + "&hum=" + hum;
      http.begin(client, httpReqStr.c_str());
      int httpResponseCode = http.GET();
      if (httpResponseCode > 0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        String payload = http.getString();
        Serial.println(payload);
      } else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
      }
      // Free resources
      http.end();
    }
  }
}

void relayOp(int st) {
  if (st == 1) {
    digitalWrite(12, HIGH);
    Serial.println("RELAY ON");
    delay(200);
  }
  if (st == 0) {
    digitalWrite(12, LOW);
    Serial.println("RELAY OFF");
    delay(200);
  }
}

String getDate() {
  timeClient.update();
  time_t epochTime = timeClient.getEpochTime();
  struct tm* ptm = gmtime((time_t*)&epochTime);

  int monthDay = ptm->tm_mday;
  int currentMonth = ptm->tm_mon + 1;
  int currentYear = ptm->tm_year + 1900;

  String currentDate = String(monthDay) + "-" + String(currentMonth) + "-" + String(currentYear);
  Serial.print("Current date: ");
  Serial.println(currentDate);
  return currentDate;
}

String getTime() {
  timeClient.update();
  time_t epochTime = timeClient.getEpochTime();
  struct tm* ptm = gmtime((time_t*)&epochTime);
  String currentTime = String(timeClient.getHours()) + ":" + String(timeClient.getMinutes()) + ":" + String(timeClient.getSeconds());
  return currentTime;
}

String getHour() {
  timeClient.update();
  time_t epochTime = timeClient.getEpochTime();
  struct tm* ptm = gmtime((time_t*)&epochTime);
  String currentHour = String(timeClient.getHours());
  return currentHour;
}

void getDHT() {
  int chk = DHT.read11(D4);
  Serial.print("Temperature=");
  Serial.println(DHT.temperature);
  hum = DHT.humidity;
  temp = DHT.temperature;
  Serial.print("Humidity=");
  Serial.println(DHT.humidity);
  delay(200);
}
