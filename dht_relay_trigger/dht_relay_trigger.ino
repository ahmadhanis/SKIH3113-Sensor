/* 
Coder: Ahmad Hanis
Email: slumberjer@gmail.com
written: 9/5/2024
last update: 14/5/2024
Microcontroller: ESP8266
Components: Relay, DHT11

Description:
This code allow for the device to automatically turn on and off relay based on temp or humidity.
You can connect to device AP mode DHT-TRIGAP then open a browser (http://192.168.4.1). The page return 
a page where user can set trigger temp and humidity values, auto or manual mode, set relay on and off 
manually, and provide table of 500 temp/hum for 500 of 1 minute interval each.

TODO
1. load and save status mode in eeprom. 
2. load and save AP Name in eeprom
*/

#include <EEPROM.h>
#include <dht.h>
#include <ESP8266WebServer.h>

#define D2 4   // DHT pin (3.3v)
#define D5 14  // Relay pin (3.3v)

dht DHT;
ESP8266WebServer server(80);

int hum = 0, temp = 0;
String trhum = "", trtemp = "";
int swdelay = 10;
String relay = "Off", prevRelay = "Off", content;
String operation = "";
int counter = 0;
int arrindex = 0;
const int arraySize = 100;
int tempData[arraySize];
int humData[arraySize];
unsigned long previousMillis = 0;
int c = 0;
const char* ssidap = "DHT-TRIGAP";
const char* passap = "";

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(200);
  pinMode(D5, OUTPUT);
  readData();
  delay(100);
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssidap, passap);
  Serial.println(WiFi.softAPIP());
  delay(100);
  webServer();
  server.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  server.handleClient();
  delay(100);
  getDHT();
  if (counter == 50) {
    if (arrindex < arraySize) {
      tempData[arrindex] = temp;
      humData[arrindex] = hum;
      //Serial.println(arrindex);
      arrindex++;
    } else {
      arrindex = 0;
      clearArray();
    }
    counter = 0;
  }
  counter++;
  if (operation == "Auto") {
    if (trhum != "" || trtemp != "") {
      //Serial.println("Set");
      if ((hum >= trhum.toInt()) || (temp > trtemp.toInt())) {
        if (relay == "Off" && prevRelay == "Off") {
          relayOp(1);
          prevRelay = "On";
          relay = "On";
        }
      } else {
        if (relay == "On" && prevRelay == "On") {
          relayOp(0);
          prevRelay = "Off";
          relay = "Off";
        }
      }
    } else {
      //Serial.println("Not Set");
      if ((hum >= 70) || (temp > 30)) {
        if (relay == "Off" && prevRelay == "Off") {
          relayOp(1);
          prevRelay = "On";
          relay = "On";
        }
      } else {
        if (relay == "On" && prevRelay == "On") {
          relayOp(0);
          prevRelay = "Off";
          relay = "Off";
        }
      }
    }
  }
  if (operation == "Manual") {
    if (relay == "On" && prevRelay == "Off") {
      relayOp(1);
      prevRelay = "On";
      relay = "On";
    }
    if (relay == "Off" && prevRelay == "On") {
      relayOp(0);
      prevRelay = "Off";
      relay = "Off";
    }
  }
  //delay op
  unsigned long currentMillis = millis();
  const unsigned long interval = swdelay * 60000;
  if (operation == "Delay") {
    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;
      Serial.println(String(swdelay) + " min have passed | " + relay);
      if (relay == "Off") {
        relayOp(1);
        relay = "On";
      } else {
        relayOp(0);
        relay = "Off";
      }
      c = 0;
    } else {
      c++;
      Serial.println(String(currentMillis - previousMillis) + "<" + String(interval) + " Not yet: " + String(c));
    }
  }
}

void clearArray() {
  for (int i = 0; i < arraySize; i++) {
    Serial.print(humData[i]);
    Serial.print(",");
  }
  for (int i = 0; i < arraySize; i++) {
    tempData[i] = 0;  // Set each element to zero
    humData[i] = 0;
  }
}


void webServer() {
  server.on("/", []() {
    String content = "<html><head><meta name='viewport' content='width=device-width, initial-scale=1.0'>";
    content += "<style>";
    content += ".w3-container { max-width: 600px; margin: auto; padding: 20px; }";
    content += ".w3-input { width: 100%; padding: 10px; }";
    content += ".w3-button { background-color: #4CAF50; color: white; padding: 10px 20px; text-align: center; text-decoration: none; display: inline-block; font-size: 16px; margin: 4px 2px; cursor: pointer; border: none; border-radius: 4px; }";
    content += ".w3-table { width: 100%; border-collapse: collapse; }";
    content += ".w3-table th, .w3-table td { border: 1px solid #ddd; padding: 8px; text-align: left; }";
    content += ".w3-table th { background-color: #4CAF50; color: white; }";
    content += "th, td { border: 1px solid black; padding: 8px; text-align: center; }";
    content += "@media screen and (max-width: 600px) { .w3-container { max-width: 100%; } }";
    content += "</style></head><body>";
    content += "<div class='w3-container'>";
    content += "<h2>ESP-DHT AUTO-TRIG</h2><hr>";
    content += "<h3>Settings</h3>";
    content += "<form method='get' action='settings'>";
    content += "<table class='w3-table'>";
    content += "<tr><th>Parameter</th><th>Value</th><th>Operation</th></tr>";
    content += "<tr><td><label>AP Name</label></td><td>" + String(ssidap) + " </td><td><input class='w3-input' name='apname' type='text' value=" + String(ssidap) + "></td></tr>";
    content += "<tr><td><label>AP Password</label></td><td>" + String(passap) + " </td><td><input class='w3-input' name='appass' type='password' value=" + String(passap) + "></td></tr>";
    content += "<tr><td><label>Operation Mode:</label></td><td>" + operation + " </td><td>";
    content += "<select class='w3-input' name='operation'>";
    content += "<option value='Auto'>Auto</option>";
    content += "<option value='Delay'>Delay</option>";
    content += "<option value='Manual'>Manual</option>";
    content += "</select></td></tr>";
    content += "<tr><td><label>Delay:</label></td><td>" + String(swdelay) + " minutes </td>";
    content += "<td><select class='w3-input' name='delay'>";
    for (int swdelayv = 5; swdelayv <= 50; swdelayv += 5) {
      if (swdelay == swdelayv) {
        content += "<option value='" + String(swdelay) + "' selected>" + String(swdelayv) + " minutes</option>";
      } else {
        content += "<option value='" + String(swdelayv) + "'>" + String(swdelayv) + " minutes</option>";
      }
    }
    content += "</select></td></tr>";
    content += "<tr><td><label>Current Temperature:</label></td><td>" + String(temp) + " C</td><td></td></tr>";
    content += "<tr><td><label>Current Humidity:</label></td><td>" + String(hum) + "%</td><td></td></tr>";
    content += "<tr><td><label>Trigger Temperature:</label></td><td>" + String(trtemp) + " C</td>";
    content += "<td><select class='w3-input' name='temperature'>";
    for (int stemp = 25; stemp <= 40; stemp++) {
      if (stemp == temp) {
        content += "<option value='" + String(stemp) + "' selected>" + String(stemp) + " C</option>";
      } else {
        content += "<option value='" + String(stemp) + "' >" + String(stemp) + " C</option>";
      }
    }
    content += "</select></td></tr>";
    content += "<tr><td><label>Trigger Humidity:</label></td><td>" + String(trhum) + "%</td><td>";
    content += "<select class='w3-input' name='humidity'>";
    for (int shum = 40; shum <= 99; shum++) {
      if (shum == hum) {
        content += "<option value='" + String(shum) + "' selected>" + String(shum) + " %</option>";
      } else {
        content += "<option value='" + String(shum) + "'>" + String(shum) + " %</option>";
      }
    }
    content += "</td></tr>";
    content += "<tr><td colspan='2'><input class='w3-button' type='submit'></td><td></td></tr></table></form></div>";
    content += "<div class='' style='text-align: center'><a href='/'><button class='w3-button'>Refresh</button></a></div>";
    content += "<br><hr>";  // Add some space between forms
    content += "</body></html>";
    server.send(200, "text/html", content);
  });

  server.on("/settings", []() {
    //http://192.168.4.1/settings?apname=DHT-TRIGAP&appass=&operation=Auto&delay=10&temperature=25&humidity=40
    String trtemperature = "";
    String trhumidity = "";
    String operation = "";
    String swdelay = "";
    String napname = "";
    String nappass = "";

    trtemperature = server.arg("temperature");
    trhumidity = server.arg("humidity");
    operation = server.arg("operation");
    swdelay = server.arg("delay");
    napname = server.arg("apname");
    nappass = server.arg("appass");

    // Serial print all the received values
    Serial.println("Received values:");
    Serial.println("Temperature: " + trtemperature);
    Serial.println("Humidity: " + trhumidity);
    Serial.println("Operation: " + operation);
    Serial.println("Delay: " + swdelay);
    Serial.println("AP Name: " + napname);
    Serial.println("AP Password: " + nappass);

    writeData(trtemperature, trhumidity, operation, swdelay, napname, nappass);
    delay(200);
    readData();
    delay(500);
    server.sendHeader("Location", "/", true);  // Redirect to "/"
    server.send(302, "text/plain", "");
  });
  server.on("/delay", []() {
    prevRelay = "Off";
    relay = "On";
    swdelay = server.arg("delay").toInt();
    server.sendHeader("Location", "/", true);  // Redirect to "/"
    server.send(302, "text/plain", "");
  });
  server.on("/op", []() {
    operation = server.arg("operation");
    server.sendHeader("Location", "/", true);  // Redirect to "/"
    server.send(302, "text/plain", "");
  });

  server.on("/onoff", []() {
    if (operation == "Manual") {
      relay = server.arg("onoff");
      server.sendHeader("Location", "/", true);  // Redirect to "/"
      server.send(302, "text/plain", "");
    } else {
      server.send(200, "text/html", "<html><head><meta name='viewport' content='width=device-width, initial-scale=1.0'><style>.w3-button { background-color: #4CAF50; color: white; padding: 10px 20px; text-align: center; text-decoration: none; display: inline-block; font-size: 16px; margin: 4px 2px; cursor: pointer; border: none; border-radius: 4px; }</style><html></body><h2>Operation must be set to MANUAL mode</h2><a href='/'><button>Home</button></a></body><html>");
    }
  });
}

void getDHT() {
  int chk = DHT.read11(D2);
  hum = DHT.humidity;
  temp = DHT.temperature;
  // Serial.print("Humidity=");
  // Serial.println(DHT.humidity);
  delay(200);
}

void relayOp(int st) {
  if (st == 1) {
    digitalWrite(D5, LOW);
    Serial.println("RELAY ON");
    delay(200);
  }
  if (st == 0) {
    digitalWrite(D5, HIGH);
    Serial.println("RELAY OFF");
    delay(200);
  }
}

void readData() {
  EEPROM.begin(512);
  Serial.println("Read Data");

  // Read 'trtemperature' from EEPROM
  for (int i = 0; i < 20; i++) {
    char character = char(EEPROM.read(i));
    if (character != '\0') {
      trtemp += character;
    }
  }

  // Read 'trhumidity' from EEPROM
  for (int i = 20; i < 40; i++) {
    char character = char(EEPROM.read(i));
    if (character != '\0') {
      trhum += character;
    }
  }

  // Read 'operation' from EEPROM
  for (int i = 40; i < 60; i++) {
    char character = char(EEPROM.read(i));
    if (character != '\0') {
      operation += character;
    }
  }

  // Read 'swdelay' from EEPROM
  for (int i = 60; i < 80; i++) {
    char character = char(EEPROM.read(i));
    if (character != '\0') {
      swdelay += character;
    }
  }

  // Read 'napname' from EEPROM
  for (int i = 80; i < 100; i++) {
    char character = char(EEPROM.read(i));
    if (character != '\0') {
      ssidap += character;
    }
  }

  // Read 'nappass' from EEPROM
  for (int i = 100; i < 120; i++) {
    char character = char(EEPROM.read(i));
    if (character != '\0') {
      passap += character;
    }
  }

  // Serial output for debugging
  Serial.println("Trig Temp: " + trtemp);
  Serial.println("Trig Hum: " + trhum);
  Serial.println("Operation: " + operation);
  Serial.println("Switch Delay: " + swdelay);
  Serial.println("Network SSID: " + String(ssidap));
  Serial.println("Network Password: " + String(passap));

  EEPROM.end();
}


void writeData(String a, String b, String c, String d, String e, String f) {
  // Serial output for debugging
  clearData();  // Clear current EEPROM memory
  EEPROM.begin(512);
  Serial.println("Write Data...");

  Serial.println("Writing " + a);
  for (int i = 0; i < 20; i++) {   // Writing for stat max for 20 characters long
    EEPROM.write(i, a.charAt(i));  // Accessing characters using charAt() method
    Serial.println(a.charAt(i));
  }

  Serial.println("Writing " + b);
  for (int i = 0; i < 20; i++) {        // Writing for SSID max for 20 characters long
    EEPROM.write(20 + i, b.charAt(i));  // Accessing characters using charAt() method
    Serial.println(b.charAt(i));
  }

  Serial.println("Writing " + c);
  for (int i = 0; i < 20; i++) {
    EEPROM.write(40 + i, c.charAt(i));
    Serial.println(c.charAt(i));
  }

  Serial.println("Writing " + d);
  for (int i = 0; i < 20; i++) {
    EEPROM.write(60 + i, d.charAt(i));
    Serial.println(d.charAt(i));
  }

  Serial.println("Writing " + e);
  for (int i = 0; i < 20; i++) {
    EEPROM.write(80 + i, e.charAt(i));
    Serial.println(e.charAt(i));
  }

  Serial.println("Writing " + f);
  for (int i = 0; i < 20; i++) {
    EEPROM.write(100 + i, f.charAt(i));
    Serial.println(f.charAt(i));
  }

  EEPROM.commit();
  EEPROM.end();
  Serial.println("Write Successful");
}


void clearData() {
  EEPROM.begin(512);
  Serial.println("Clearing EEPROM ");
  for (int i = 0; i < 512; i++) {
    Serial.print(".");
    EEPROM.write(i, 0);
  }
  EEPROM.commit();
  EEPROM.end();
}