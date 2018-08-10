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

void loop()
{
  Serial1.write("RAT\r");
  Serial.println("Send RAT");
  delay(50);
  int c = Serial1.read();
  if (c != '?') {
    digitalWrite(LED, HIGH);
    int c = Serial1.read();
    if (c == '\r') {
      Serial.write("\r\n");
    }
    else {
      Serial.write(c);
    }
    digitalWrite(LED, LOW);
  }
  else {
  // Disable
  Serial1.write("SRD\r");
  delay(10);
  int count_out = 0;
  while (Serial1.available() && count_out < 4) {
    int a = Serial1.read();
    count_out++;
  }
  Serial.println("Sleeping");
  delay(5000);

  // Enable
  Serial1.write("SRA\r");
  delay(10);
  count_out = 0;
  while (Serial1.available() && count_out < 4) {
    int b = Serial1.read();
    count_out++;
  }
  Serial.println("Wake up!");
  delay(5000);
  Serial.println();
  }
}
