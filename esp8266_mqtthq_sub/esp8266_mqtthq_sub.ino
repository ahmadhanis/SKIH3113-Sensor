#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// WiFi credentials
const char* ssid = "UUMWiFi_Guest";
const char* password = "";

// MQTT broker details
const char* mqtt_server = "public.mqtthq.com";
const int mqtt_port = 1883;
const char* mqtt_topic = "uum-esp8266/101/led";


//Pin definitions 
const int ledPin = 2;  // Built-in LED typically connected to GPIO2

// Initialize WiFi and MQTT client
WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  // Set the LED pin as output
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);  // Initialize the LED state to off

  // Connect to WiFi
  setup_wifi();

  // Set the MQTT server and callback
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(mqttCallback);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
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

void reconnect() {
  // Loop until reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      // Once connected, subscribe to the topic
      client.subscribe(mqtt_topic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  String message;

  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.println(message);

  // Control the LED based on the message received
  if (message == "ON") {
    digitalWrite(ledPin, HIGH);
    Serial.println("LED is ON");
  } else if (message == "OFF") {
    digitalWrite(ledPin, LOW);
    Serial.println("LED is OFF");
  }
}
