const int LC1pin = 2;
float LC1value = 0;

void setup() {
  Serial.begin(9600);
}

void loop() {
  LC1value = analogRead(LC1pin);
  Serial.print("Raw Value : ");
  Serial.println(LC1value);
  delay(5000);
}
