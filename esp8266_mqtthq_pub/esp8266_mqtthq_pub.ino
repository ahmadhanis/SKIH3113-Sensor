#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// WiFi credentials
const char* ssid = "UUMWiFi_Guest";
const char* password = "";

// MQTT broker details
const char* mqtt_server = "public.mqtthq.com";
const int mqtt_port = 1883;
const char* mqtt_topic = "uum-esp8266/101/led";

// Pin definitions
const int buttonPin = 0;  // Flash button typically connected to GPIO0 INPUT
const int ledPin = 2;     // Built-in LED typically connected to GPIO2 OUTPUT

// Variables to hold the button state
int lastButtonState = HIGH;  // Assume the button is not pressed
int ledState = LOW;          // LED initially off

// Initialize WiFi and MQTT client
WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  // put your setup code here, to run once:
  // Initialize Serial monitor
  Serial.begin(115200);

  // Set the button pin as input with internal pullup resistor
  pinMode(buttonPin, INPUT_PULLUP);

  // Set the LED pin as output
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, ledState);  // Initialize the LED state

  // Connect to WiFi
  setup_wifi();
}

void loop() {
  // Ensure the client remains connected to the MQTT broker
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  // Read the button state user input FLASH button
  int buttonState = digitalRead(buttonPin);
  if (buttonState == LOW && lastButtonState == HIGH) {
    delay(50);  // Debounce delay
    if (digitalRead(buttonPin) == LOW) {
      ledState = !ledState;
      digitalWrite(ledPin, ledState);

      // Publish the LED state to the MQTT topic
      String stateMessage = (ledState == HIGH) ? "ON" : "OFF";
      char* message = stringToChar(stateMessage);

      client.publish(mqtt_topic, message);
      Serial.println("LED State: " + stateMessage);

      // Free the allocated memory
      delete[] message;
    }
  }
}

// Function to convert String to char*
char* stringToChar(String str) {
  // Allocate memory for the char array (+1 for the null terminator)
  char* charArray = new char[str.length() + 1];
  // Copy the contents of the String to the char array
  str.toCharArray(charArray, str.length() + 1);
  return charArray;
}

void reconnect() {
  // put your main code here, to run repeatedly:
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup_wifi() {
  // Start connecting to WiFi
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  // Wait until connected
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}
