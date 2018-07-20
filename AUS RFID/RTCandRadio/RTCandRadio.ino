#include <SPI.h>
#include <RH_RF95.h>
#include <RTCZero.h>

#define RFM95_CS 8 // Unique for Feather LoRa M0
#define RFM95_RST 4
#define RFM95_INT 3

#define RF95_FREQ 915.0
RH_RF95 rf95(RFM95_CS, RFM95_INT);

RTCZero rtc;
#define PERIOD 5// how long roughly (ms) between two reading

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
  delay(1000);
  alarm_init();
}

int count_out = 0;

void loop()
{
  if (alarmrang) {
    char radiopacket[RADIO_PACKET_LENGTH] = "Hello, World!#";
    itoa(count_out++,radiopacket+14,10);
    transmit(radiopacket, RADIO_PACKET_LENGTH);
    digitalWrite(LED, HIGH);
    delay(1000);
    digitalWrite(LED, LOW);
    alarmrang = !alarmrang;
    rf95.sleep();
    rtc.setSeconds(0);
  }
  else // this else is not run because after rtc.setSeconds runs, MCU seems to turn off right away.
    rf95.sleep();
  rtc.standbyMode();    // Sleep until next alarm match
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
  alarmrang = true; // this cannot have any delay inside. Recommend sending some flag like this.
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
  rf95.setTxPower(5, false);
}

void transmit(char radiopacket[], int RADIO_PACKET_LENGTH) {
  rf95.send((uint8_t *)radiopacket, RADIO_PACKET_LENGTH);
  rf95.waitPacketSent();
}
