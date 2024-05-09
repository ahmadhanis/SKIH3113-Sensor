//Coded by : Ahmad Hanis
//Date:9/5/2024
//Description: DHT Firebase with data logger

#include <ESP8266WiFi.h>
#include <dht.h>
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"
#include <NTPClient.h>
#include <WiFiUdp.h>

#define D4 2  //DHT11 Sensor connected to D4

bool signupOK = false;
const char* ssid = "UUMWiFi_Guest";
const char* pass = "";
int hum = 0, temp = 0;
String relay = "Off", prevRelay = "On";
String timestamp = "", datestamp = "", hourstamp = "";
unsigned long sendDataPrevMillis = 0;
int count = 0;

dht DHT;
//Define Firebase Data object
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
FirebaseJson json;

WiFiUDP ntpUDP;
const long utcOffsetInSeconds = 28800;  //UTC 8 offset UTC(8+) * 60*60
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);


void setup() {
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
  config.api_key = "AIzaSyBobpVpnt_0fTy-YcKqp_S8p5JEpa0CB-w";
  config.database_url = "https://my-iot-projects-d1d93-default-rtdb.asia-southeast1.firebasedatabase.app/";

  /* Sign up */
  if (Firebase.signUp(&config, &auth, "", "")) {
    signupOK = true;
  } else {
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback;  //see addons/TokenHelper.h

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop() {

  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 5000 || sendDataPrevMillis == 0)) {
    count++;
    getDHT();
    sendDataPrevMillis = millis();
    // ntp get date/time/hour
    datestamp = getDate();
    timestamp = getTime();
    hourstamp = getHour();

    //update operation
    if (Firebase.RTDB.setInt(&fbdo, "101/humidity", hum)) {
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    } else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }

    if (Firebase.RTDB.setInt(&fbdo, "101/temp", temp)) {
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    } else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }

    if (Firebase.RTDB.setString(&fbdo, "101/time", datestamp)) {
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    } else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }

    //relay operation
    if (Firebase.RTDB.getString(&fbdo, "101/relay")) {
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
      relay = fbdo.stringData();
      Serial.println(relay);
      Serial.println(prevRelay);
      if (fbdo.dataType() == "string") {
        if (relay == "Off" && prevRelay == "On") {
          relayOp(0);
          prevRelay = "Off";
        }
        if (relay == "On" && prevRelay == "Off") {
          relayOp(1);
          prevRelay = "On";
        }
      }

      //data logger every 10 counter ++ (about 1 minute)
      String tempPath = "/temperature";
      String humPath = "/humidity";
      String timePath = "/timestamp";
      json.set(tempPath.c_str(), String(temp));
      json.set(humPath.c_str(), String(hum));
      if (count == 10) {
        if (Firebase.RTDB.setJSON(&fbdo, "101/logger/" + datestamp + "/" + hourstamp + "/" + timestamp, &json)) {
          Serial.println("PASSED");
          Serial.println("PATH: " + fbdo.dataPath());
          Serial.println("TYPE: " + fbdo.dataType());
        } else {
          Serial.println("FAILED");
          Serial.println("REASON: " + fbdo.errorReason());
        }
        count = 0;
      }
    } else {
      Serial.println(fbdo.errorReason());
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
