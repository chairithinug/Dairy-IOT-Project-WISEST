#include <SPI.h>
#include <RH_RF95.h>

#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 3
#define RF95_FREQ 915.0
RH_RF95 rf95(RFM95_CS, RFM95_INT);

void setup() {
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);
  
  Serial.begin(115200);
  delay(100);
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
  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    while (1);
  }
  Serial.print("Set Freq to: ");
  Serial.println(RF95_FREQ);
  rf95.setTxPower(23, false);
  delay(7500);
}

int packetnum = 0;

void loop() {
  char radiopacket[25] = "This is TX1 sending#";
  itoa(packetnum++, radiopacket+20, 10);
  Serial.print("Sending ");
  Serial.println(radiopacket);
  rf95.send((uint8_t *)radiopacket, sizeof(radiopacket));
  delay(5000);
}
