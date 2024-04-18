#include <dht.h>  //import dht library first
#define D4 2      //DHT11 Sensor connected to D4

dht DHT;          //DHT object from dht class

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
  int chk = DHT.read11(D4); 
  Serial.print("Temperature=");
  Serial.println(DHT.temperature);
  Serial.print("Humidity=");
  Serial.println(DHT.humidity);
  delay(1000);
}
