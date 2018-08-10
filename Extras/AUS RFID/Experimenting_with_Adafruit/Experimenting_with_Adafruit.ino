#include <SPI.h>

#define LED 13 // On-board LED
#define PERIOD 1 // Read every x second(s)

const int HALF_PERIOD = PERIOD * 1000 / 2; // in ms

void setup()
{
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);
  while (!Serial)
    delay(1);
  Serial.begin(115200);
  Serial1.begin(9600);
//  Serial1.write("SD2\r"); // Set default mode to Animal Tag
  Serial.println("Ready");
//  Serial1.write("SRA\r"); // Enable the reader to read as opposed to SRD
}

bool id = true;

void loop()
{ 
//  Serial.print("id = ");
//  Serial.println(id);
  if (Serial1.available()) {
    digitalWrite(LED, HIGH);
    byte c = Serial1.read();
    if (c == '\r') {
      if (id) {
        Serial.write(";");
//        Serial.println("Send RAT");
        Serial1.write("RAT\r"); // Read more detailed data
        delay(50);
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
  else{
    Serial1.write("SRD\r");
    delay(10);
    int count_out = 0;
    while (Serial1.available() && count_out < 4){
      byte a = Serial1.read();
      count_out++;
    }
//    Serial.println("Sleeping");
    delay(HALF_PERIOD);
    Serial1.write("SRA\r");
    delay(10);
    count_out = 0;
    while (Serial1.available() && count_out < 4){
      byte b = Serial1.read();
      count_out++;
    }
//    Serial.println("Wake up!");
    delay(HALF_PERIOD);
//    Serial.println();
  }
}
