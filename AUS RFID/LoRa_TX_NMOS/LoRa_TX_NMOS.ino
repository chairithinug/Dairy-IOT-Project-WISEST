#include <SPI.h>
#include <RH_RF95.h>

#define RFM95_CS 8 // Unique for Feather LoRa M0
#define RFM95_RST 4
#define RFM95_INT 3

#define RF95_FREQ 915.0
RH_RF95 rf95(RFM95_CS, RFM95_INT);

#define PERIOD 5// How long roughly (ms) between two reading

#define VBATPIN A7 // Pin 9 (analog 7)
#define NMOSPIN 6

const int PACKET_LENGTH = 49;
const int RADIO_PACKET_LENGTH = 28;
int index_data = 0;
int index_radio = 0;
char packet[PACKET_LENGTH] = {}; // Contains raw data
char radiopacket[RADIO_PACKET_LENGTH] = {}; // Contains edited data that is going to be sent
boolean id = true;
boolean sent = false;

void setup()
{
  pinMode(NMOSPIN, OUTPUT);
  radio_init();
  Serial.begin(9600);
  Serial1.begin(9600);
  memset(&packet[0], 0, sizeof(packet)); // initialize memory with null
  memset(&radiopacket[0], 0, sizeof(radiopacket)); // initialize memory with null
  rf95.sleep();
  reader_disable();
  delay(500);
}

void loop()
{
  // Enable reader
  reader_enable();
  delay(500);
  sent = false;
  while (Serial1.available()) {
    int c = Serial1.read();
    if (c == '\r') {
      if (id) { // Get the first part of data (the data received from the sensor without RAT)
        Serial1.write("RAT\r"); // Read more detailed data
        Serial1.flush();
        delay(50);
      }
      id = !id;
    }
    else {
      packet[index_data++] = c; // This array contains all raw data
      if (index_data == 47) // 46th number
        c = B00110001; // 31 in HEX which is 1 is ASCII. This bit is always one!
      if ((index_data >= 18 && index_data != 21 && index_data != 34 && index_data != 36 && index_data < 38) || index_data >= 44) {
        radiopacket[index_radio++] = c; // This array contains only data (Country,ID,Animal,Data,Temp) going to be sent
        Serial.write(c);
      }
    }
    if (index_data >= PACKET_LENGTH)
    {
      itoa(battery_level(), radiopacket + 23, 10);
      transmit(radiopacket, RADIO_PACKET_LENGTH);
      sent = true;
      index_data = 0;
      index_radio = 0;
      memset(&radiopacket[0], 0, sizeof(radiopacket)); // Clear memory with null
      memset(&packet[0], 0, sizeof(packet)); // Clear memory with null

      // Turn off radio and reader
      rf95.sleep();
      reader_disable();
    }
  }
  if (!sent) {
    char radiopacket[] = "xxxxxxxxxxxxxxxxxxxxxxx";
    itoa(battery_level(), radiopacket + 23, 10);
    transmit(radiopacket, RADIO_PACKET_LENGTH);
    index_data = 0;
    index_radio = 0;
    memset(&radiopacket[0], 0, sizeof(radiopacket)); // Clear memory with null
    memset(&packet[0], 0, sizeof(packet)); // Clear memory with null
  }
  
  rf95.sleep();
  reader_disable();
  delay(5000);
}

void radio_init() {
  pinMode(RFM95_RST, OUTPUT);
  // Manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);
  while (!rf95.init())
    while (1);
  if (!rf95.setFrequency(RF95_FREQ))
    while (1);
  rf95.setTxPower(5, false); // Between 5 and 23 5 for low power
}

void reader_enable() {
  digitalWrite(NMOSPIN, HIGH);
}

void reader_disable() {
  digitalWrite(NMOSPIN, LOW);
}

void transmit(char sendpacket[], int LENGTH) {
  rf95.send((uint8_t *)sendpacket, LENGTH);
  rf95.waitPacketSent();
}

int battery_level() {
  float vbat = analogRead(VBATPIN);
  vbat *= 2;    // we divided by 2, so multiply back
  vbat *= 3.3;  // Multiply by 3.3V, our reference voltage
  vbat /= 1024; // convert to voltage
  vbat *= 10000; // remove decimal point to int
  return (int) vbat;
}
