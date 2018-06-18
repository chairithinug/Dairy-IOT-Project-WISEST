#include <SPI.h>
#include <RH_RF95.h>

#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 3
#define RF95_FREQ 915.0
RH_RF95 rf95(RFM95_CS, RFM95_INT);

#define ID 1

#define RST_POW 6 // Reset transistor to keep reading
#define RST_READER 5 // Reset reader always HIGH
#define DATA_NUM 30 // Number of bytes receieved from sensor
#define LED 13

const int PACKET_LENGTH = 3 * DATA_NUM;
byte data_read, data_shift;
int shift_count = 0;
int index_byte = 0;
char packet[PACKET_LENGTH] = {};

void setup()
{
  // Setup radio channel communication
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);
  Serial.begin(115200);
  while (!Serial) {
    delay(1);
  }
  delay(100);
  Serial.println("Feather LoRa TX Test!");
  // Manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);
  while (!rf95.init()) {
    Serial.println("LoRa radio init failed");
    while (1);
  }
  Serial.println("LoRa radio init OK!");
  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    while (1);
  }
  Serial.print("Set Freq to: ");
  Serial.println(RF95_FREQ);
  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on
  // The default transmitter power is 13dBm, using PA_BOOST.
  // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then
  // you can set transmitter powers from 5 to 23 dBm:
  rf95.setTxPower(23, false);

  // Setup reader module
  pinMode(RST_POW, OUTPUT);
  pinMode(RST_READER, OUTPUT);
  pinMode(LED, OUTPUT);
  Serial1.begin(9600);

  digitalWrite(RST_POW, HIGH);
  delay(10);
  digitalWrite(RST_POW, LOW);
  delay(10);

  digitalWrite(RST_READER, HIGH);

  Serial1.flush();
}

void loop()
{
  // ReadSerial
  if (Serial1.available() > 0) {
    data_read = Serial1.read();
    itoa(data_read, packet + index_byte * 3, 10);
    index_byte = index_byte + 1;
  }
  if (index_byte >= DATA_NUM) {
    printPacket(packet, PACKET_LENGTH);

    digitalWrite(LED, LOW);
    Serial1.end();
    // Transmit
    Serial.print("Sending ");
    Serial.println(packet);

    Serial.println("Sending...");
    delay(10);
    rf95.send((uint8_t *)packet, PACKET_LENGTH);

    Serial.println("Waiting for packet to complete...");
    delay(10);
    rf95.waitPacketSent();
    
    memset(&packet[0], 0, sizeof(packet)); // clear memory
    index_byte = 0;
    digitalWrite(RST_POW, HIGH);
    delay(250);
    digitalWrite(RST_POW, LOW);
    Serial1.begin(9600);
    Serial1.flush();
    delay(250);
    digitalWrite(LED, HIGH);
  }

  // Now wait for a reply
  //  waitReply();
}

void readSerial() {

}

void transmit(char* packet, int PACKET_LENGTH) {

}

void waitReply() {
  uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);

  Serial.println("Waiting for reply...");
  if (rf95.waitAvailableTimeout(1000))
  {
    // Should be a reply message for us now
    if (rf95.recv(buf, &len))
    {
      Serial.print("Got reply: ");
      Serial.println((char*)buf);
      Serial.print("RSSI: ");
      Serial.println(rf95.lastRssi(), DEC);
    }
    else
    {
      Serial.println("Receive failed");
    }
  }
  else
  {
    Serial.println("No reply, is there a listener around?");
  }
}

void printPacket(char packet[], int packet_len) {
  for (int i = 0 ; i < packet_len; i++) {
    Serial.print(packet[i]);
  }
  Serial.println("");
}

