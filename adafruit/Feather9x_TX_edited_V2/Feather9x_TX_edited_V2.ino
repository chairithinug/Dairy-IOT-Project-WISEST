#include <SPI.h>
#include <SoftwareSerial.h>
#include <RH_RF95.h>

#define RFM95_CS 8 // specific for Feather M0 Lora
#define RFM95_RST 4
#define RFM95_INT 3 
#define RF95_FREQ 915.0
#define rst_pow 2 // reset transistor pin
#define rst_reader 8 // Do not use but set to HIGH
RH_RF95 rf95(RFM95_CS, RFM95_INT);
SoftwareSerial rfidserial(10,11);

void setup()
{
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);

  pinMode(rst_pow, OUTPUT);
  digitalWrite(rst_pow, LOW);
  
  pinMode(rst_reader, OUTPUT);
  digitalWrite(rst_reader , HIGH);
  
  Serial.begin(9600);
  rfidserial.begin(9600);
  while (!Serial) {
    delay(1);
  }
  
  delay(100);
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);
  
  while (!rf95.init()) {
    Serial.println("LoRa radio init failed");
    while (1);
  }
  
  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    while (1);
  }
  
  Serial.print("Freq: ");
  Serial.println(RF95_FREQ);
  rf95.setTxPower(23, false);
}

const byte compare = 128;
byte data_read, data_shift;
byte shift_count = 0;
byte index = 0;
char radiopacket[40]; // should use only 30 spots
//int16_t packetnum = 0;

void loop()
{
  digitalWrite(rst_pow, LOW);
  if (rfidserial.available() > 0) {
    radiopacket[index++] = rfidserial.read();
    // data_read = rfidserial.read();
    // padding(data_read);
  }
  if (index >= 30) {
    Serial.println("");
    rfidserial.end();
    index = 0;
    digitalWrite(rst_pow, HIGH);
    delay(95);
    digitalWrite(rst_pow, LOW);
    rfidserial.begin(9600);
    delay(95);
    
    delay(10);
    rf95.send((uint8_t *)radiopacket, 20);
    delay(10);
    rf95.waitPacketSent(); //flush
    memset(&radiopacket[0], 0, sizeof(radiopacket)); // clear memory
  }
  if (rfidserial.overflow())
  {
    rfidserial.end();
    delay(100);
    rfidserial.begin(9600);
  }
}

void padding(uint8_t data) { // adding leading zeros so the data is 8-bit wide
  shift_count = 0;
  data_shift = data;
  while (data_shift < compare && shift_count < 8)
  {
    Serial.print(0, BIN);
    data_shift = data_shift << 1;
    shift_count = shift_count + 1;
    delay(1);
  }
  Serial.print(data, BIN);
  Serial.flush(); // add to test its functionality
}

char 0pad2Array(uint8_t data) { // adding leading zeros so the data is 8-bit wide
  char paddedData[10]; // should use only 8 spots
  memset(&paddedData[0], 0, sizeof(paddedData)); // clear memory
  for (int j=0; j<8; j++)
  {
    paddedData[j] = '0';
    delay(1);
  }
  Serial.print(data, BIN);
  return paddedData[10];
}
