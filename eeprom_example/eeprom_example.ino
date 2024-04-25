#include <EEPROM.h>
String ssid = "UUMWiFi_Guest";
String pass = "Abc123";

void setup() {
  EEPROM.begin(512);
  Serial.begin(115200);
  delay(100);
  Serial.println("WRITE DATA TO EEPROM");
  writeData();
  delay(500);
  Serial.println("READING DATA FROM EEPROM");
  delay(500);
  readData();
}

void loop() {
}

void writeData() {
  Serial.println("Writing to EEPROM");
  for (int i = 0; i < 20; i++) {
    EEPROM.write(i, ssid[i]);
  }
  for (int i = 20; i < 40; i++) {
   EEPROM.write(i, pass[i - 20]);
  }
  EEPROM.commit();
  Serial.println("Write successfull");
}

void readData() {  //Read from EEPROM memory
  String essid, epass;

  EEPROM.begin(512);
  Serial.println("Reading From EEPROM..");
  for (int i = 0; i < 20; i++) {  //Reading for SSID max for 20 characters long
    if (char(EEPROM.read(i)) > 0) {
      essid += char(EEPROM.read(i));
      // Serial.println(char(EEPROM.read(i)));
    }
  }
  Serial.println("Reading Wifi ssid: " + ssid);
  for (int i = 20; i < 40; i++) {  //Reading for WiFi password max for 40 characters long
    if (char(EEPROM.read(i)) > 0) {
      epass += char(EEPROM.read(i));
      // Serial.println(char(EEPROM.read(i)));
    }
  }
  Serial.println("WiFi ssid from EEPROM:" + essid);
  Serial.println("WiFi password EEPROM:" + epass);
  Serial.println("Reading successfull.....");

  EEPROM.end();
}
