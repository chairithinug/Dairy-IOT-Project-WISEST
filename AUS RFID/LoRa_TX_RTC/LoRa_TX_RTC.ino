#include <SPI.h>
#include <RH_RF95.h>
#include <RTCZero.h>

#define RFM95_CS 8 // Unique for Feather LoRa M0
#define RFM95_RST 4
#define RFM95_INT 3

#define RF95_FREQ 915.0
RH_RF95 rf95(RFM95_CS, RFM95_INT);

RTCZero rtc;
#define PERIOD 30// How long roughly (sec) between two reading

//#define LED 13 // On-board LED
#define VBATPIN A7 // Pin 9 (analog 7)
#define NMOSPIN 6

const int PACKET_LENGTH = 48;
const int RADIO_PACKET_LENGTH = 28;
int index_data;
int index_radio;
char packet[PACKET_LENGTH] = {}; // Contains raw data
char radiopacket[RADIO_PACKET_LENGTH] = {}; // Contains edited data that is going to be sent
boolean id = true;
boolean alarmrang = false;
boolean sent = false;

void setup()
{
  pinMode(NMOSPIN, OUTPUT);
  reader_disable();
  radio_init();
  Serial1.begin(9600);
  clear_packets_n_indices();
  rf95.sleep();
  alarm_init();
}

void loop()
{
  if (alarmrang) {
    reader_enable();
    delay(1000);
    sent = false;
    while (Serial1.available()) {
      int c = Serial1.read();
      if (c == '\r') {
        if (id) // Get the first part of data (the data received from the sensor without RAT)
          get_sensor_data();
        id = !id;
      }
      else {
        if (c != '\0') {
          packet[index_data++] = c; // This array contains all raw data
          if (index_data == 46)
            c = B00110001; // 31 in HEX which is 1 is ASCII. This bit is always one!
          if ((index_data >= 17 && index_data != 20 && index_data != 33 && index_data != 35 && index_data < 37) || index_data >= 43)
            radiopacket[index_radio++] = c; // This array contains only data (Country,ID,Animal,Data,Temp) going to be sent
        }
        if (index_data >= PACKET_LENGTH && !sent) {
          reader_disable();
          itoa(battery_level(), radiopacket + 23, 10); // Attach battery level at the end of string stream
          transmit(radiopacket, RADIO_PACKET_LENGTH);
          sent = true;
          break;
        }
      }
    }
    if (!sent) {
      reader_disable();
      char radiopacket[] = "xxxxxxxxxxxxxxxxxxxxxxx";
      itoa(battery_level(), radiopacket + 23, 10);
      transmit(radiopacket, RADIO_PACKET_LENGTH);
    }
    clear_packets_n_indices();
    go_sleep();
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

void alarmMatch() {
  alarmrang = true; // This cannot have any delay inside. Recommend sending some flag.
}

void reader_enable() {
  digitalWrite(NMOSPIN, HIGH);
}

void reader_disable() {
  digitalWrite(NMOSPIN, LOW);
}

void get_sensor_data() {
  Serial1.write("RAT\r"); // Read more detailed data
  Serial1.flush();
  delay(50);
}

void clear_packets_n_indices() {
  index_data = 0;
  index_radio = 0;
  memset(&radiopacket[0], 0, sizeof(radiopacket)); // Clear memory with null
  memset(&packet[0], 0, sizeof(packet)); // Clear memory with null
}

void transmit(char radiopacket[], int RADIO_PACKET_LENGTH) {
  rf95.send((uint8_t *)radiopacket, RADIO_PACKET_LENGTH);
  rf95.waitPacketSent();
}

void go_sleep() {
  reader_disable();
  rf95.sleep();
  alarmrang = false;
  rtc.setSeconds(0); // End of the iteration. Go back to sleep
}

int battery_level() {
  float vbat = analogRead(VBATPIN);
  vbat *= 2;    // we divided by 2, so multiply back
  vbat *= 3.3;  // Multiply by 3.3V, our reference voltage
  vbat /= 1024; // convert to voltage
  vbat *= 10000; // remove decimal point to int
  return (int) vbat;
}
