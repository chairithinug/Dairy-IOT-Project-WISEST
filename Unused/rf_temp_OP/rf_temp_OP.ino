#include <SoftwareSerial.h>
SoftwareSerial rfidserial(10, 11);

#define rst_pow 2 //restart the reader to keep reading

const byte compare = 128;
byte data_read, data_shift;

byte index = 0;
void setup() {
  pinMode(rst_pow, OUTPUT);
  Serial.begin(9600);
  rfidserial.begin(9600);
  digitalWrite(rst_pow, LOW);
  digitalWrite(8, HIGH);
}

void loop() {
  digitalWrite(rst_pow, LOW);
  if (rfidserial.available() > 0) {
    data_read = rfidserial.read();
    padding(data_read);
    index++;
  }
  if (index >= 30) {
    Serial.println();
    rfidserial.end();
    digitalWrite(rst_pow, HIGH);
    index = 0;
    delay(200);
    digitalWrite(rst_pow, LOW);
    rfidserial.begin(9600);
    delay(200);
  }
  if (rfidserial.overflow())
  {
    rfidserial.end();
    delay(100);
    rfidserial.begin(9600);
  }
}

void padding(byte data) { // adding leading zeros so the data is 8-bit wide
  data_shift = data;
  while (data_shift < compare)
  {
    Serial.print(0, BIN);
    data_shift = data_shift << 1;
  }
  Serial.print(data, BIN);
  Serial.flush(); // add to test its functionality
  data_shift = 0;
}


