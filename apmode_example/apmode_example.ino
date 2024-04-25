#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>

const char* ssidap = "nodemcu-server";
const char* passap = "";

ESP8266WebServer server(80);
String ssid, pass, devid, content;

void setup() {
  Serial.begin(115200);
  delay(100);
  Serial.println("AP Mode. Please connect to http://192.168.4.1 to configure");
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssidap, passap);
  Serial.println(WiFi.softAPIP());
  createWebServer();
  server.begin();
  pinMode(16, OUTPUT);
}

void createWebServer() {
  server.on("/", []() {
    content = "<html><head></head><body><h2>Welcome to NodeMCU Server</h2></body></htm>";
    server.send(200, "text/html", content);
  });
  server.on("/settings", []() {
    content = "<html><head><style>.button {background-color: #3CBC8D;";
    content += "color: white;padding: 5px 10px;text-align: center;text-decoration: none;";
    content += "display: inline-block;font-size: 14px;margin: 4px 2px;cursor: pointer;}";
    content += "input[type=text],[type=password]{width: 100%;padding: 5px 10px;";
    content += "margin: 5px 0;box-sizing: border-box;border: none;";
    content += "background-color: #3CBC8D;color: white;}</style></head><body>";
    content += "<h1>WIFI MANAGER</h1><br>";
    content += "<h3>Current Settings</h3>";
    content += "<table><tr><td><label> Device ID</label></td><td><label>" + devid + "</label></td></tr>";
    content += "<tr><td><label> WiFi SSID</label></td><td><label>" + ssid + "</label></td></tr>";
    content += "<tr><td><label> WiFi Pasword</label></td><td><label>" + pass + "</label></td></tr></table><br><br>";
    content += "<form method='get' action='setting'>";
    content += "<h3>New WiFi Settings</h3>";
    content += "<table><tr><td><label>WiFi SSID</label></td><td><input type='text' name = 'ssid' lenght=32 ></td></tr>";
    content += "<tr><td><label> WiFi Password</label></td><td><input type='password' name = 'password' lenght=32></td></tr>";
    content += "<tr><td><label>Device ID</label></td><td><input type='text' name = 'deviceid' lenght=32 ></td></tr>";
    content += "<tr><td></td><td><input class=button type='submit'></td></tr></table></form>";
    content += "</body></html>";
    server.send(200, "text/html", content);
  });
  server.on("/on", []() {
    digitalWrite(16, LOW);
    content = "<html><head></head><body><h2>LED ON</h2></body></htm>";
    server.send(200, "text/html", content);
  });

  server.on("/off", []() {
    digitalWrite(16, HIGH);
    content = "<html><head></head><body><h2>LED OFF</h2></body></htm>";
    server.send(200, "text/html", content);
  });
  server.on("/dht", []() {
    String temp = "27";
    content = "<html><head></head><body><h2>TEMP:" + temp + "celcius</h2></body></htm>";
    server.send(200, "text/html", content);
  });
}

void loop() {
  server.handleClient();
}
