#include <SPI.h>
#include <RH_RF95.h>

#define RFM95_CS 8 // Unique for Feather LoRa M0
#define RFM95_RST 4
#define RFM95_INT 3

#define RF95_FREQ 915.0
RH_RF95 rf95(RFM95_CS, RFM95_INT);

#define PERIOD 5// How long roughly (ms) between two reading
#define SAMPLE_PER_WAKE 3

#define VBATPIN A7 // Pin 9 (analog 7)
#define NMOSPIN 6

const int PACKET_LENGTH = 48;
const int RADIO_PACKET_LENGTH = 18;

int avg = 0;
int index_data = 0;
char packet[PACKET_LENGTH] = {}; // Contains sensor data
char radiopacket[RADIO_PACKET_LENGTH] = {}; // Contains edited data that is going to be sent
int temperature[SAMPLE_PER_WAKE] = {};
char idpacket[16] = {};
uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
uint8_t len = sizeof(buf);

char OUT[2] = "1";
boolean id = true;
boolean sent = false;
boolean received = false;

void setup()
{
  pinMode(NMOSPIN, OUTPUT);
  radio_init();
  Serial.begin(9600);
  Serial1.begin(9600);
  clear_packet();
  clear_radiopacket();
  clear_temperaturepacket();
  rf95.sleep();
  reader_disable();
  delay(500);
}

void loop()
{
  sent = false;
  int sample_count = 0;
  received = false;
  clear_radiopacket();
  clear_temperaturepacket();
  clear_idpacket();
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
          if (index_data == 45) // 46th number
            c = B00110001; // 31 in HEX which is 1 is ASCII. This bit is always one!
          packet[index_data++] = c;
          Serial.write(c);
          Serial.print("(");
          Serial.print(index_data);
          Serial.print(") ");
        }
      }
      if (index_data >= PACKET_LENGTH)
      {
        sample_count++;
        received = true;
        temperature[sample] = extract_temperature(packet);
        Serial.println();
        Serial.print("Temperature is ");
        Serial.print(temperature[sample]);

        extract_id(idpacket, packet);
        Serial.print(" ID is ");
        Serial.println(idpacket);
        break;
      }
    }
    clear_packet();
    reader_disable();
    delay(500);
  }
  if (!received) { // if nothing is received after 3 samples
    Serial.println("Get nothing");
  }
  else
  {
    avg = average_temperature(temperature, sample_count);
    Serial.print("The average is ");
    Serial.println(avg);

    memcpy(radiopacket, idpacket, 15);
    itoa(avg, radiopacket + 15, 10);
    transmit(radiopacket, RADIO_PACKET_LENGTH);
    sent = true;
  }

  while (sent) { // if get anything, send and wait for a reply
    digitalWrite(LED_BUILTIN, HIGH);
    if (rf95.waitAvailableTimeout(2000)) {
      if (rf95.recv(buf, &len)) {
        Serial.print("Got reply: ");
        Serial.println((char*)buf);

        if (strncmp((char*) buf, (char*) OUT, 1) == 0)
          break;
      }
      else
        Serial.println("Receive failed");
    }
    int r = (rand() % 5);
    Serial.print("Retrying in ");
    Serial.println(r);
    delay(r * 1000); // wait for 0-5 sec and resend again
    transmit(radiopacket, RADIO_PACKET_LENGTH);
  }
  digitalWrite(LED_BUILTIN, LOW);

  rf95.sleep();
  delay(PERIOD * 1000);
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

int extract_temperature(char* sensor_data) {
  return strtol(sensor_data + 45, NULL, 16);
}

void extract_id(char* id, char* sensor_data) {
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

