#include <SoftwareSerial.h>
SoftwareSerial rfidSerial(10, 11);

#define RST_POW 2 // Reset transistor to keep reading
#define RST_READER 8 // Reset reader always HIGH
#define COMPARE 128 // 1000_0000
#define BYTE_NUM_LENGTH 30 // Number of bytes receieved from sensor

byte data_read, data_shift;
byte shift_count = 0;
byte index = 0;

void setup() {
  pinMode(RST_POW, OUTPUT);
  pinMode(RST_READER, OUTPUT);
  Serial.begin(9600);
  rfidSerial.begin(9600);
  
  while (!Serial) {
    delay(1);
  }
  digitalWrite(RST_POW, LOW);
  digitalWrite(RST_READER, HIGH);
}

void loop() {
  digitalWrite(RST_POW, LOW);
  if (rfidSerial.available() > 0) {
    data_read = rfidSerial.read();
    padding(data_read);
    index++;
  }
  if (index >= BYTE_NUM_LENGTH) {{
    Serial.println("");
    Serial.flush(); // moved from padding function. The rest is intact.
    rfidSerial.end();
    index = 0;
    digitalWrite(RST_POW, HIGH);
    delay(95);
    digitalWrite(RST_POW, LOW);
    rfidSerial.begin(9600);
    delay(95);
  }
  if (rfidSerial.overflow())
  {
    rfidSerial.end();
    delay(100);
    rfidSerial.begin(9600);
  }
}

void padding(uint8_t data) { // adding leading zeros so the data is 8-bit wide
  shift_count = 0;
  data_shift = data;
  while (data_shift < COMPARE && shift_count < 8)
  {
    Serial.print(0, BIN);
    data_shift = data_shift << 1;
    shift_count = shift_count + 1;
    delay(1);
  }
  Serial.print(data, BIN);
}


