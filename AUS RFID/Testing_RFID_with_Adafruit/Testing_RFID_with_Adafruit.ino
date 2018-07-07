#include <SPI.h>

#define LED 13 // On-board LED

void setup()
{
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);
  while (!Serial)
    delay(1);
  Serial.begin(9600);
  Serial1.begin(9600);
//  Serial1.write("SD2\r"); // Set default mode to Animal Tag
  delay(1000);
  Serial.println("Complete");
//  Serial1.write("SRA\r"); // Enable the reader to read as opposed to SRD
}

bool id = true;

void loop()
{
  if (Serial1.available()) {
    digitalWrite(LED, HIGH);
    int c = Serial1.read();
    if (c == '\r') {
      if (id) {
        Serial.write("; ");
        Serial1.write("RAT\r"); // Read more detailed data
      }
      else {
        Serial.write("\r\n");
      }
      id = !id;
    }
    else {
      Serial.write(c);
    }
    digitalWrite(LED, LOW);
  }
}
