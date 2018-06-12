#include <SoftwareSerial.h>
SoftwareSerial Rfidserial(10, 11);

#define RST_POW 2 // Reset transistor to keep reading
#define RST_READER 8 // Reset reader always HIGH
#define PACKET_LENGTH 30

byte data_read[PACKET_LENGTH];

void setup() {
  pinMode(RST_POW, OUTPUT);
  pinMode(RST_READER, OUTPUT);

  Serial.begin(9600);
  Rfidserial.begin(9600);

  while (!Serial) {
    delay(1);
  }

  digitalWrite(RST_POW, LOW);
  digitalWrite(RST_READER, HIGH);
}

byte index = 0;

void loop() {
  digitalWrite(RST_POW, LOW);
  if (Rfidserial.available() > 0) {
    data_read[index] = Rfidserial.read();
    index++;
  }
  if (index >= 30) {
    transmit(data_read);
    Rfidserial.end();
    index = 0;
    digitalWrite(RST_POW, HIGH);
    delay(100);
    digitalWrite(RST_POW, LOW);
    Rfidserial.begin(9600);
    delay(100);
  }
}

void transmit(byte data_read[]) {
  for (byte i = 0; i < PACKET_LENGTH; i++) {
    Serial.print(data_read[i], HEX);
    delay(1);
  }
  Serial.println("");
  Serial.flush();
}

