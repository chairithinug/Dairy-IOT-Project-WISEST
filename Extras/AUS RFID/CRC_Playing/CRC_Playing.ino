void setup() {
  Serial.begin(9600);

}

byte a = 0xA; //1010
byte b = 2; //0010


void loop() {
  Serial.print("Xor result of ");
  Serial.print(a);
  Serial.print(" and ");
  Serial.print(b);
  Serial.print(" is: ");
  Serial.println(a^b);
  delay(1000);
}
