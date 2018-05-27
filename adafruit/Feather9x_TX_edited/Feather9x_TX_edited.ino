#include <SPI.h>
#include <RH_RF95.h>

#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 3
#define RF95_FREQ 915.0
RH_RF95 rf95(RFM95_CS, RFM95_INT);

void setup()
{
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);

  Serial.begin(9600);
  while (!Serial) {
    delay(1);
  }
  delay(100);

  Serial.println("Feather LoRa TX Test!");

  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  while (!rf95.init()) {
    Serial.println("LoRa radio init failed");
    while (1);
  }
  Serial.println("LoRa radio init OK!");

  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    while (1);
  }
  Serial.print("Set Freq to: ");
  Serial.println(RF95_FREQ);
  rf95.setTxPower(23, false);
}

int16_t packetnum = 0;

void loop()
{
  if (Serial.available()) {
    char radiopacket[20];
    memset(&radiopacket[0], 0, sizeof(radiopacket)); // clear memory
    int i = 0;
    //uint8_t ch = Serial.read();
    while (Serial.available() > 0) // This loop reads all characters
    {
      radiopacket[i] =  Serial.read();
      i++;
    }
    delay(1000);

    //char radiopacket[20] = {ch,' ','#'};
    itoa(packetnum++, radiopacket+i, 10); //+ length of data //ltoa (long val, char *s, int radix)
    Serial.print("Sending data:");
    Serial.println(radiopacket);
    radiopacket[19] = 0;

    delay(10);
    rf95.send((uint8_t *)radiopacket, 20);

    delay(10);
    rf95.waitPacketSent();
    Serial.println("Sent successfully!");
  }
  //==============================================Waiting for a reply part====================================

  //  uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
  //  uint8_t len = sizeof(buf);
  //
  //  Serial.println("Waiting for reply...");
  //  if (rf95.waitAvailableTimeout(1000))
  //  {
  //    // Should be a reply message for us now
  //    if (rf95.recv(buf, &len))
  //   {
  //      Serial.print("Got reply: ");
  //      Serial.println((char*)buf);
  //      Serial.print("RSSI: ");
  //      Serial.println(rf95.lastRssi(), DEC);
  //    }
  //    else
  //    {
  //      Serial.println("Receive failed");
  //    }
  //  }
  //  else
  //  {
  //    Serial.println("No reply, is there a listener around?");
  //  }

}
