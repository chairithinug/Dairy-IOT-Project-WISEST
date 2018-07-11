#include <SPI.h>
#include <RH_RF95.h>

#define RFM95_CS 8 // Unique for Feather LoRa M0
#define RFM95_RST 4
#define RFM95_INT 3

#define RF95_FREQ 915.0
RH_RF95 rf95(RFM95_CS, RFM95_INT);

#define PERIOD 4000// how long roughly (ms) between two reading

#define LED 13 // On-board LED
#define VBATPIN A7 // Pin 9 (analog 7)

const int PACKET_LENGTH = 48;
const int HALF_PERIOD = PERIOD / 2;
int index_packet = 0;
char packet[PACKET_LENGTH] = {};
boolean id = true;
boolean done = false;

void setup()
{
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);
  Serial.begin(115200);

  Serial1.begin(9600);
  //  Serial1.write("SD2\r"); // Set default mode to Animal Tag
  memset(&packet[0], 0, sizeof(packet)); // initialize memory with null
  rf95.sleep();
  delay(1000);
  Serial.println("Ready");
  //  Serial1.write("SRA\r"); // Enable the reader to read as opposed to SRD
}

byte CRC = 0;

void loop()
{
  if (Serial1.available()) {
    digitalWrite(LED, HIGH);
    int c = Serial1.read();
    if (c == '\r') {
      if (id) {
        //        Serial.write(";");
        Serial.flush();
        //        Serial.println("Send RAT");
        Serial1.write("RAT\r"); // Read more detailed data
        Serial1.flush();
        delay(50);
      }
      id = !id;
    }
    else {
      if (index_packet == 45) // 46th number but index 45 before increment
      {
        c = B00110001; // 31 in HEX which is 1 is ASCII. This bit is always one!
      }
      Serial.write(c);
      Serial.flush();
      packet[index_packet++] = c;
      if ((index_packet >= 17 && index_packet != 20 && index_packet != 33 && index_packet != 35 && index_packet < 37) || index_packet >= 43) {

        CRC ^= c;
      }
      if (index_packet == 48)
      {
        Serial.print(" Checksum: ");
        Serial.print(CRC);
        CRC = 0;
      }
    }
    if (index_packet >= PACKET_LENGTH)
    {
      Serial.println(" Transmitted");
      index_packet = 0;
      memset(&packet[0], 0, sizeof(packet)); // clear memory with null
    }
    digitalWrite(LED, LOW);
  }
  else {
    //    Serial.println("Sleeping");
    rf95.sleep();
    Serial1.write("SRD\r");
    Serial1.flush();
    delay(10);
    int count_out = 0;
    while (Serial1.available() && count_out < 3) {
      int a = Serial1.read(); // Clear response of SRD command
      count_out++;
    }
    delay(PERIOD);
    //    Serial.println("Wake up!");
    Serial1.write("SRA\r");
    Serial1.flush();
    delay(10);
    count_out = 0;
    while (Serial1.available() && count_out < 3) {
      int b = Serial1.read(); // Clear response of SRA command
      count_out++;
    }
    delay(PERIOD / 32); // Must be at least 100 ms
    //    Serial.println();
  }
}
