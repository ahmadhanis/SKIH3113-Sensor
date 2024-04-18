#define D3 0      

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(D3, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(D3, HIGH); //open
  Serial.println("HIGH");
  delay(5000);
  digitalWrite(D3, LOW);  //close
  Serial.println("LOW");
  delay(5000);
}
