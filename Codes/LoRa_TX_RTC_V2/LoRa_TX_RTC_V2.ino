#include <SPI.h>
#include <RH_RF95.h> // LoRa library
#include <RTCZero.h> // Timer and standby library

#define RFM95_CS 8 // Unique for Feather LoRa M0
#define RFM95_RST 4
#define RFM95_INT 3

#define RF95_FREQ 915.0
RH_RF95 rf95(RFM95_CS, RFM95_INT);

RTCZero rtc;
#define PERIOD 30// How long roughly (sec) between two reading
#define SAMPLE_PER_WAKE 3

#define NMOSPIN 6

const int PACKET_LENGTH = 48; // Does not include a null terminator
const int RADIO_PACKET_LENGTH = 28;

int avg;
int index_data;
int sample_count;

char packet[PACKET_LENGTH] = {}; // Contains one reading
char radiopacket[RADIO_PACKET_LENGTH] = {}; // Contains data waiting to be sent
int temperature[SAMPLE_PER_WAKE] = {}; // Contains
char idpacket[16] = {};

// Incoming reply data
uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
uint8_t len = sizeof(buf);

char OUT[2] = "1";
boolean id = true;
boolean alarmrang = false;
boolean sent = false;
boolean received = false;

void setup()
{
  device_init();
}

void loop()
{
  if (alarmrang) {
    setup_variable();
    for (int sample = 0 ; sample < SAMPLE_PER_WAKE; sample++) {
      reader_enable();
      delay(500); // Wait for reader to turn on
      while (Serial1.available()) { // Get all data from reader in this iteration
        int c = Serial1.read();
        if (c == '\r') {
          if (id) { // Reader only read ID without data. Need a command for more data
            get_sensor_data();
          }
          id = !id;
        }
        else {
          if (c != '\0') { // If the data is not null
            if (index_data == 45) { // 46th number starting bit of temperature
              c = B00110001; // 31 in HEX which is 1 is ASCII. This bit is always one!
            }
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
      set_packet();
      reader_disable();
      delay(500);
    }
    if (received) { // If get anything during 3 iterations
      avg = average_temperature(temperature, sample_count);
      memcpy(radiopacket, idpacket, 15); // Attach the ID to be sent
      itoa(avg, radiopacket + 15, 10); // Convert Int to Char and attach the average to be sent
      transmit(radiopacket, RADIO_PACKET_LENGTH);
      sent = true;
    }

    // This is supposed to handle a collision (did not quite handle in real life)
    while (sent) { // If get anything, send and wait for a reply
      if (rf95.waitAvailableTimeout(2000)) { // Wait for 2 seconds for a reply
        if (rf95.recv(buf, &len)) {
          if (strncmp((char*) buf, (char*) OUT, 1) == 0) {  // If get the expected reply
            break;
          }
        }
      }
      retransmit(); // Keep sending until get a reply
    }

    go_sleep();
  }
  rtc.standbyMode(); // Sleep until next alarm match
}

/////////////////////////////////////////////////////////////////////////////////
// Setup the program
/////////////////////////////////////////////////////////////////////////////////

void device_init() {
  pinMode(NMOSPIN, OUTPUT);
  radio_init();
  Serial1.begin(9600);
  set_packet();
  set_radiopacket();
  set_temperaturepacket();
  rf95.sleep();
  reader_disable();
  delay(500);
  alarm_init();
}

/////////////////////////////////////////////////////////////////////////////////
// Setup LoRa communication
/////////////////////////////////////////////////////////////////////////////////

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

/////////////////////////////////////////////////////////////////////////////////
// Setup an alarm
/////////////////////////////////////////////////////////////////////////////////

void alarm_init() {
  rtc.begin();
  rtc.setSeconds(0);
  rtc.setAlarmSeconds(PERIOD);
  rtc.enableAlarm(rtc.MATCH_SS); // Repeat if match the same second. // 0-59
  rtc.attachInterrupt(alarmMatch);
  rtc.standbyMode();
}

/////////////////////////////////////////////////////////////////////////////////
// Called when the alarm goes off
/////////////////////////////////////////////////////////////////////////////////

void alarmMatch() {
  alarmrang = true; // This cannot have any delay inside, using a flag.
}

/////////////////////////////////////////////////////////////////////////////////
// Setup the program
/////////////////////////////////////////////////////////////////////////////////

void setup_variable() {
  sent = false;
  sample_count = 0;
  avg = 0;
  received = false;
  set_radiopacket();
  set_temperaturepacket();
  set_idpacket();
}

/////////////////////////////////////////////////////////////////////////////////
// Power a reader
/////////////////////////////////////////////////////////////////////////////////

void reader_enable() {
  digitalWrite(NMOSPIN, HIGH); // NMOS gate is open for the current's flow
}

/////////////////////////////////////////////////////////////////////////////////
// Turn off a reader
/////////////////////////////////////////////////////////////////////////////////

void reader_disable() {
  digitalWrite(NMOSPIN, LOW); // NMOS gate is closed
}

/////////////////////////////////////////////////////////////////////////////////
// Send a command to a reader to get detailed data
/////////////////////////////////////////////////////////////////////////////////

void get_sensor_data() {
  Serial1.write("RAT\r"); // Get temperature data
  Serial1.flush();
  delay(50);
}

/////////////////////////////////////////////////////////////////////////////////
// Send data to the hub using LoRa
// Parameters
//            sendpacket: a pointer of a data array
//            LENGTH: a number of characters to be sent
/////////////////////////////////////////////////////////////////////////////////

void transmit(char sendpacket[], int LENGTH) {
  rf95.send((uint8_t *)sendpacket, LENGTH);
  rf95.waitPacketSent();
}

/////////////////////////////////////////////////////////////////////////////////
// Randomize a number X (0-5), wait for X seconds, and tranmit again
/////////////////////////////////////////////////////////////////////////////////

void retransmit() {
  int r = (rand() % 5);
  delay(r * 1000); // Wait for 0-5 sec
  transmit(radiopacket, RADIO_PACKET_LENGTH); // Resend again
}

/////////////////////////////////////////////////////////////////////////////////
// Set a timer and disable devices to go into a sleep mode (Standby mode)
/////////////////////////////////////////////////////////////////////////////////

void go_sleep() {
  reader_disable();
  rf95.sleep();
  alarmrang = false;
  rtc.setSeconds(0); // End of the iteration. Go back to sleep
}

/////////////////////////////////////////////////////////////////////////////////
// Extract temperature from the data received from a reader
// Parameters
//            sensor_data: a pointer of a data array
/////////////////////////////////////////////////////////////////////////////////

int get_temperature(char* sensor_data) {
  return strtol(sensor_data + 45, NULL, 16); // Convert from ASCII Hex to Dec
}

/////////////////////////////////////////////////////////////////////////////////
// Extract ID from the data received from a reader
// Parameters
//            id: a pointer of a destination array to place ID
//            sensor_data: a pointer of a data array
/////////////////////////////////////////////////////////////////////////////////

void get_id(char* id, char* sensor_data) {
  memcpy(id, sensor_data, 3); // country code
  memcpy(id + 3, sensor_data + 4, 12); // ID
}

/////////////////////////////////////////////////////////////////////////////////
// Return the floor of the average of temperature
// Parameters
//            temp: a pointer of an temperature array from readings
//            divisor: a number of samples to average
/////////////////////////////////////////////////////////////////////////////////

int average_temperature(int* temp, int divisor) {
  int sum = 0;
  if (divisor != 0) {
    for (int i = 0; i < SAMPLE_PER_WAKE; i++) {
      sum += (int) temp[i];
    }
    return sum / divisor;
  }
  else {
    return 0;
  }
}

/////////////////////////////////////////////////////////////////////////////////
// Clear memory
/////////////////////////////////////////////////////////////////////////////////

void set_packet() {
  index_data = 0;
  memset(&packet[0], 0, sizeof(packet)); // Clear memory with null
}

void set_radiopacket() {
  memset(&radiopacket[0], 0, sizeof(radiopacket)); // Clear memory with null
}

void set_temperaturepacket() {
  memset(&temperature[0], 0, sizeof(temperature)); // Clear memory with null
}

void set_idpacket() {
  memset(&idpacket[0], 0, sizeof(idpacket)); // Clear memory with null
}
