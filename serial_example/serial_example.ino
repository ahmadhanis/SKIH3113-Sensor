void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(100);
  Serial.println("Hello from the world of ESP8266");
  Serial.println("Printing the value of i");
  for (int i=0; i < 100; i++){
    Serial.print("The value of i is");
    Serial.println(i);
    delay(200);
  }
}

void loop() {
  // put your main code here, to run repeatedly:

}
