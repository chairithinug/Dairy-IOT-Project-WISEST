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
#define SAMPLE_PER_WAKE 3

//#define LED 13 // On-board LED
#define VBATPIN A7 // Pin 9 (analog 7)
#define NMOSPIN 6

const int PACKET_LENGTH = 48;
const int RADIO_PACKET_LENGTH = 28;

int avg;
int index_data;
int sample_count;
char packet[PACKET_LENGTH] = {}; // Contains raw data
char radiopacket[RADIO_PACKET_LENGTH] = {}; // Contains edited data that is going to be sent
int temperature[SAMPLE_PER_WAKE] = {};
char idpacket[16] = {};
uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
uint8_t len = sizeof(buf);

char OUT[2] = "1";
boolean id = true;
boolean alarmrang = false;
boolean sent = false;
boolean received = false;

void setup()
{
  pinMode(NMOSPIN, OUTPUT);
  radio_init();
  Serial1.begin(9600);
  clear_packet();
  clear_radiopacket();
  clear_temperaturepacket();
  rf95.sleep();
  reader_disable();
  delay(500);
  alarm_init();
}

void loop()
{
  if (alarmrang) {
    setup_variable();
    for (int sample = 0 ; sample < SAMPLE_PER_WAKE; sample++) {
      reader_enable();
      delay(500); // wait for reader to turn on
      while (Serial1.available()) { // get all data from reader in this iteration
        int c = Serial1.read();
        if (c == '\r') {
          if (id) // Get the first part of data (the data received from the sensor without RAT)
            get_sensor_data();
          id = !id;
        }
        else {
          if (c != '\0') {
            if (index_data == 45) // 46th number starting bit of temperature
              c = B00110001; // 31 in HEX which is 1 is ASCII. This bit is always one!
            packet[index_data++] = c;
          }
        }
        if (index_data >= PACKET_LENGTH) {
          sample_count++;
          received = true;
          temperature[sample] = get_temperature(packet);
          get_id(idpacket, packet);
          break;
        }
      }
      clear_packet();
      reader_disable();
      delay(500);
    }
    if (received) {// if get anything during 3 iterations
      avg = average_temperature(temperature, sample_count);
      memcpy(radiopacket, idpacket, 15);
      itoa(avg, radiopacket + 15, 10);
      transmit(radiopacket, RADIO_PACKET_LENGTH);
      sent = true;
    }
    while (sent) { // if get anything, send and wait for a reply
      if (rf95.waitAvailableTimeout(2000)) {
        if (rf95.recv(buf, &len)) {
          if (strncmp((char*) buf, (char*) OUT, 1) == 0) // if get the expected reply
            break;
        }
      }
      retransmit(); // keep sending until get a reply
    }
    
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

void setup_variable() {
  sent = false;
  sample_count = 0;
  avg = 0;
  received = false;
  clear_radiopacket();
  clear_temperaturepacket();
  clear_idpacket();
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

void clear_packet() {
  index_data = 0;
  memset(&packet[0], 0, sizeof(packet)); // Clear memory with null
}

void clear_radiopacket() {
  memset(&radiopacket[0], 0, sizeof(radiopacket)); // Clear memory with null
}

void clear_temperaturepacket() {
  memset(&temperature[0], 0, sizeof(temperature)); // Clear memory with null
}

void clear_idpacket() {
  memset(&idpacket[0], 0, sizeof(idpacket)); // Clear memory with null
}

void transmit(char sendpacket[], int LENGTH) {
  rf95.send((uint8_t *)sendpacket, LENGTH);
  rf95.waitPacketSent();
}

void retransmit() {
  int r = (rand() % 5);
  delay(r * 1000); // wait for 0-5 sec and resend again
  transmit(radiopacket, RADIO_PACKET_LENGTH);
}

void go_sleep() {
  reader_disable();
  rf95.sleep();
  alarmrang = false;
  rtc.setSeconds(0); // End of the iteration. Go back to sleep
}

int get_temperature(char* sensor_data) {
  return strtol(sensor_data + 45, NULL, 16);
}

void get_id(char* id, char* sensor_data) {
  memcpy(id, sensor_data, 3);
  memcpy(id + 3, sensor_data + 4, 12);
}

int average_temperature(int* temp, int divisor) {
  int sum = 0;
  if (divisor != 0) {
    for (int i = 0; i < SAMPLE_PER_WAKE; i++) {
      sum += (int) temp[i];
    }
    return sum / divisor;
  }
  else
    return 0;
}
