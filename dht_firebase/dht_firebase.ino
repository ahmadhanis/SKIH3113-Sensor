#include <ESP8266WiFi.h>
#include <dht.h>
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"
#include <NTPClient.h>
#include <WiFiUdp.h>


#define D4 2  //DHT11 Sensor connected to D4
bool signupOK = false;
dht DHT;
const char* ssid = "UUMWiFi_Guest";
const char* pass = "";
int hum = 0, temp = 0;
String relay = "Off", prevRelay = "On";

//Define Firebase Data object
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");
int timestamp;

unsigned long sendDataPrevMillis = 0;
int count = 0;

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

  /* Assign the api key (required) */
  config.api_key = "AIzaSyBobpVpnt_0fTy-YcKqp_S8p5JEpa0CB-w";

  /* Assign the RTDB URL (required) */
  config.database_url = "https://my-iot-projects-d1d93-default-rtdb.asia-southeast1.firebasedatabase.app/";

  /* Sign up */
  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("ok");
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
    getDHT();
    sendDataPrevMillis = millis();
    timestamp = getTime();
    Serial.print("time: ");
    Serial.println(timestamp);
    // Write an Int number on the database path test/int
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

    if (Firebase.RTDB.setInt(&fbdo, "101/time", timestamp)) {
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    } else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }

    if (Firebase.RTDB.getString(&fbdo, "101/relay")) {
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
      relay = fbdo .stringData();
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

unsigned long getTime() {
  timeClient.update();
  unsigned long now = timeClient.getEpochTime();
  return now;
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
