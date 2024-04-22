#include <ESP8266WiFi.h>
const char* ssid = "NodeMCU-AP";
const char* pass = "";

void setup(){
  Serial.begin(115200);
  delay(100);
  Serial.println();
  Serial.print("Configuring ");
  Serial.println(ssid);
  Serial.println(" access point...");
  WiFi.softAP(ssid,pass);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("IP address: ");
  Serial.print(myIP);
}

void loop() {
  // put your main code here, to run repeatedly:

}
