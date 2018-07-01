#include <SPI.h>
#include <RH_RF95.h>

#define RFM95_CS 8 // Unique for Feather LoRa M0
#define RFM95_RST 4
#define RFM95_INT 3

#define RF95_FREQ 915.0
RH_RF95 rf95(RFM95_CS, RFM95_INT);

#define RST_POW 6 // Reset transistor to keep reading
#define RST_READER 5 // Reset reader always HIGH
#define DATA_NUM 30 // Number of bytes receieved from sensor
#define LED 13
#define PERIOD 20// how long roughly (second) between two reading

const int PACKET_LENGTH = 2 * DATA_NUM;
const int HALF_PERIOD = PERIOD * 1000 / 2;

byte data_read, data_shift;
int shift_count = 0;
int index_byte = 0;
char packet[PACKET_LENGTH] = {};

void setup()
{
  Serial.begin(115200);
  delay(100);
  radio_init();

  Serial1.begin(9600);
  reader_init();
  Serial1.flush();
  memset(&packet[0], 0, sizeof(packet)); // initialize memory with null
}

void loop()
{
  digitalWrite(LED, LOW);
  if (Serial1.available() > 0) {
    data_read = Serial1.read();
    if ((data_read == 2 && index_byte == 0) || index_byte != 0) // for the first 2
    {
      sprintf(packet + index_byte * 2, "%0.2x", data_read);
      index_byte = index_byte + 1;
    }
    else
      index_byte = 0;
  }
  if (index_byte >= DATA_NUM) {
    printPacket(packet, PACKET_LENGTH);
    digitalWrite(LED, HIGH);
    Serial1.end();
    transmit(packet, PACKET_LENGTH);
    memset(&packet[0], 0, sizeof(packet)); // clear memory with null
    index_byte = 0;
    digitalWrite(RST_POW, HIGH);
    delay(HALF_PERIOD);
    digitalWrite(RST_POW, LOW);
    Serial1.begin(9600);
    Serial1.flush();
    delay(HALF_PERIOD);
    digitalWrite(LED, LOW);
  }
}

void radio_init() {
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);
  Serial.println("Feather LoRa TX Test!");
  // Manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);
  while (!rf95.init()) {
    Serial.println("LoRa failed");
    while (1);
  }
  Serial.println("LoRa OK!");
  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    while (1);
  }
  Serial.print("Freq: ");
  Serial.println(RF95_FREQ);
  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on
  // The default transmitter power is 13dBm, using PA_BOOST.
  // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then
  // you can set transmitter powers from 5 to 23 dBm:
  rf95.setTxPower(23, false);
}

void reader_init() {
  pinMode(RST_POW, OUTPUT);
  pinMode(RST_READER, OUTPUT);
  pinMode(LED, OUTPUT);
  digitalWrite(RST_POW, HIGH);
  delay(10);
  digitalWrite(RST_POW, LOW);
  delay(10);
  digitalWrite(RST_READER, HIGH);
}

void transmit(char packet[], int PACKET_LENGTH) {
  Serial.print("Sending ");
  Serial.println(packet);
  delay(10);
  rf95.send((uint8_t *)packet, PACKET_LENGTH);
  delay(10);
  rf95.waitPacketSent();
}

void printPacket(char packet[], int packet_len) {
  for (int i = 0 ; i < packet_len; i++) {
    Serial.print(packet[i]);
    Serial.print("(");
    Serial.print(i);
    Serial.print(") ");
  }
  Serial.println("");
}

