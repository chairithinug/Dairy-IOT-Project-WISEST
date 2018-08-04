int c = 0;

void setup() {
  Serial1.begin(9600);
  Serial.begin(9600);
}

void loop() {
  Serial1.println(c);
  c++;
  delay(500);
  int a = Serial1.read();
  Serial.println(a);

}
