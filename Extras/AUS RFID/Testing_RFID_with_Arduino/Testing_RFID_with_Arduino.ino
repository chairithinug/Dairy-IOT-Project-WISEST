#include <SoftwareSerial.h>
SoftwareSerial mySerial(8, 9); // RX, TX
void setup()
{
  Serial.begin(9600);
  while (!Serial)
    delay(1);
  mySerial.begin(9600);
  mySerial.write("SD2\r");
  delay(1000);
  Serial.println("Complete");
  mySerial.write("SRA\r");
}

bool id = true;

void loop()
{
  if (mySerial.available()) {
    int c = mySerial.read();
    if (c == '\r') {
      if (id) {
        Serial.write("; ");
        mySerial.write("RAT\r");
      }
      else {
        Serial.write("\r\n");
      }
      id = !id;
    }
    else {
      Serial.write(c);
    }
  }
}
