#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>

const char* ssidap = "nodemcu-server";
const char* passap = "";

ESP8266WebServer server(80);
String content = "";

void setup() {
  Serial.begin(115200);
  delay(100);
  Serial.println("AP Mode. Please connect to http://192.168.4.1 to configure");
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssidap, passap);
  Serial.println(WiFi.softAPIP());
  createWebServer();
  server.begin();
}

void createWebServer() {
  server.on("/", []() {
    content = "<html><head></head><body><h2>Welcome to NodeMCU Server</h2></body></htm>";
    server.send(200, "text/html", content);
  });
  server.on("/settings", []() {
    content = "<html><head></head><body><h2>Settings</h2></body></htm>";
    server.send(200, "text/html", content);
  });
}

void loop() {
  server.handleClient();
}
