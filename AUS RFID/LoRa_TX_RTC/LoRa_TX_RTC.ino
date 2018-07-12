#include <SPI.h>
#include <RH_RF95.h>
#include <RTCZero.h>

#define RFM95_CS 8 // Unique for Feather LoRa M0
#define RFM95_RST 4
#define RFM95_INT 3

#define RF95_FREQ 915.0
RH_RF95 rf95(RFM95_CS, RFM95_INT);

RTCZero rtc;
#define PERIOD 20// How long roughly (ms) between two reading

#define LED 13 // On-board LED

const int PACKET_LENGTH = 48;
const int RADIO_PACKET_LENGTH = 23;
int index_data = 0;
int index_radio = 0;
char packet[PACKET_LENGTH] = {}; // Contains raw data
char radiopacket[RADIO_PACKET_LENGTH] = {}; // Contains edited data that is going to be sent
boolean id = true;
boolean alarmrang = false;

void setup()
{
  Serial.begin(9600);
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);
  radio_init();
  Serial1.begin(9600);
  memset(&packet[0], 0, sizeof(packet)); // initialize memory with null
  memset(&radiopacket[0], 0, sizeof(radiopacket)); // initialize memory with null
  rf95.sleep();
  reader_disable();
  delay(1000);
  alarm_init();
}

int count_out = 0;

void loop()
{
  if (alarmrang) {
    // Enable reader
    reader_enable();
    delay(500);
    while (Serial1.available()) {
      digitalWrite(LED, HIGH);
      int c = Serial1.read();
      if (c != '9' && index_data == 0) { // Do not start if the first number is not 9
        index_data == 0;
        while (Serial1.available()) // Must be tested!
        {
          int a = Serial1.read(); // Clear all the following bit
          delay(1);
        }
      }
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
        if (index_data == 46) // 46th number
          c = B00110001; // 31 in HEX which is 1 is ASCII. This bit is always one!
        if ((index_data >= 17 && index_data != 20 && index_data != 33 && index_data != 35 && index_data < 37) || index_data >= 43)
          radiopacket[index_radio++] = c; // This array contains only data (Country,ID,Animal,Data,Temp) going to be sent
      }
      if (index_data >= PACKET_LENGTH)
      {
        transmit(radiopacket, RADIO_PACKET_LENGTH);
        index_data = 0;
        index_radio = 0;
        memset(&radiopacket[0], 0, sizeof(radiopacket)); // Clear memory with null
        memset(&packet[0], 0, sizeof(packet)); // Clear memory with null

        // Turn off radio and reader
        digitalWrite(LED, LOW);
        rf95.sleep();
        reader_disable();
        alarmrang = false;
        rtc.setSeconds(0); // Go back to sleep
      }
    }
    
    digitalWrite(LED, LOW);
    rf95.sleep();
    reader_disable();
    alarmrang = false;
    rtc.setSeconds(0);
  }
  rtc.standbyMode(); // Sleep until next alarm match
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

void alarm_init() {
  rtc.begin();
  rtc.setSeconds(0);
  rtc.setAlarmSeconds(PERIOD);
  rtc.enableAlarm(rtc.MATCH_SS);
  rtc.attachInterrupt(alarmMatch);
  rtc.standbyMode();
}

void alarmMatch()
{
  alarmrang = true; // This cannot have any delay inside. Recommend sending some flag.
}

void reader_enable() {
  Serial1.write("SRA\r");
  Serial1.flush();
  delay(10);
  int count_out = 0;
  while (Serial1.available() && count_out < 3) {
    int c = Serial1.read(); // Clear response of SRA command
    count_out++;
  }
}

void reader_disable() {
  Serial1.write("SRD\r");
  Serial1.flush();
  delay(10);
  int count_out = 0;
  while (Serial1.available() && count_out < 3) {
    int c = Serial1.read(); // Clear response of SRD command
    count_out++;
  }
}

void transmit(char radiopacket[], int RADIO_PACKET_LENGTH) {
  rf95.send((uint8_t *)radiopacket, RADIO_PACKET_LENGTH);
  rf95.waitPacketSent();
}
