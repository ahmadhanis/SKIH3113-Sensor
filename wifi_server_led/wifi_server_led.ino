#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

const char* ssid = "UUMWiFi_Guest";
const char* password = "";
ESP8266WebServer server(80);

#define D0 16

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(100);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  server.on("/on", ledOn);
  server.on("/off", ledOff);
  server.begin();
  Serial.println("Http server started");
  pinMode(D0, OUTPUT);
}

void ledOn() {
  Serial.println("Client requested");
  digitalWrite(D0, LOW);
  server.send(200, "text/html", "<h1> Switch On</h1>");
}

void ledOff() {
  Serial.println("Client requested");
  digitalWrite(D0, HIGH);
  server.send(200, "text/html", "<h1> Switch Off</h1>");
}

void loop() {
  server.handleClient();
}
