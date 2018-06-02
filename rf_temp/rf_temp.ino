#include <SoftwareSerial.h>
SoftwareSerial rfidserial(10, 11);

#define rst_pow 2 //restart the reader to keep reading

const byte compare = 128;
byte data_read, data_shift;
byte shift_count = 0;

byte index = 0;
void setup() {
  pinMode(rst_pow, OUTPUT);
  Serial.begin(9600);
  rfidserial.begin(9600);
  
  while (!Serial) {
    delay(1);
  }
  
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
    Serial.println("");
    rfidserial.end();
    index = 0;
    digitalWrite(rst_pow, HIGH);
    delay(95);
    digitalWrite(rst_pow, LOW);
    rfidserial.begin(9600);
    delay(95);
  }
  if (rfidserial.overflow())
  {
    rfidserial.end();
    delay(100);
    rfidserial.begin(9600);
  }
}

void padding(uint8_t data) { // adding leading zeros so the data is 8-bit wide
  shift_count = 0;
  data_shift = data;
  while (data_shift < compare && shift_count < 8)
  {
    Serial.print(0, BIN);
    data_shift = data_shift << 1;
    shift_count = shift_count + 1;
    delay(1);

  }
  Serial.print(data, BIN);
  Serial.flush(); // add to test its functionality
}


