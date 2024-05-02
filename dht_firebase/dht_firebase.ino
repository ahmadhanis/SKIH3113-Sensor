#include <ESP8266WiFi.h>
#include <dht.h>
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

#define D4 2  //DHT11 Sensor connected to D4
bool signupOK = false;
dht DHT;
const char* ssid = "UUMWiFi_Guest";
const char* pass = "";
int hum = 0, temp = 0;
//Define Firebase Data object
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
int count = 0;

void setup() {
  Serial.begin(115200);
  delay(100);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

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
  getDHT();
  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 15000 || sendDataPrevMillis == 0)) {
    sendDataPrevMillis = millis();
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
  }
}

void getDHT() {
  int chk = DHT.read11(D4);
  Serial.print("Temperature=");
  Serial.println(DHT.temperature);
  hum = DHT.humidity;
  temp = DHT.temperature;
  Serial.print("Humidity=");
  Serial.println(DHT.humidity);
  delay(5000);
}
