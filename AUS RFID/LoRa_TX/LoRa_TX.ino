#include <Adafruit_SleepyDog.h>

#include <SPI.h>
#include <RH_RF95.h>

#define RFM95_CS 8 // Unique for Feather LoRa M0
#define RFM95_RST 4
#define RFM95_INT 3

#define RF95_FREQ 915.0
RH_RF95 rf95(RFM95_CS, RFM95_INT);

#define PERIOD 1// how long roughly (second) between two reading

#define LED 13 // On-board LED
#define VBATPIN A7 // Pin 9 (analog 7)

const int PACKET_LENGTH = 50;
const int HALF_PERIOD = PERIOD * 1000 / 2;
int index_packet = 0;
char packet[PACKET_LENGTH] = {};
boolean id = true;

void setup()
{
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);
//  while (!Serial)
//    delay(1);
  Serial.begin(115200);
  radio_init();

  rf95.sleep();
  
  Serial1.begin(9600);
//  Serial1.write("SD2\r"); // Set default mode to Animal Tag
  memset(&packet[0], 0, sizeof(packet)); // initialize memory with null
  
  Serial.println("Ready");
//  Serial1.write("SRA\r"); // Enable the reader to read as opposed to SRD
}

void loop()
{ 
//  Serial.print("id = ");
//  Serial.println(id);
  if (Serial1.available()) {
    digitalWrite(LED, HIGH);
    int c = Serial1.read();
    if (c == '\r') {
      if (id) {
        Serial.write(";");
//        Serial.println("Send RAT");
        Serial1.write("RAT\r"); // Read more detailed data
        delay(50);
      }
      else {
        Serial.write("\r\n");
      }
      id = !id;
    }
    else {
      Serial.write(c);
//      packet[index_packet++] = c;
    }
    digitalWrite(LED, LOW);
  }
  else{
    Serial1.write("SRD\r");
    delay(10);
    int count_out = 0;
    while (Serial1.available() && count_out < 4){
      int a = Serial1.read(); // Clear response of SRD command
      count_out++;
    }
//    Serial.println("Sleeping");
    delay(HALF_PERIOD);
    Serial1.write("SRA\r");
    delay(10);
    count_out = 0;
    while (Serial1.available() && count_out < 4){
      int b = Serial1.read(); // Clear response of SRA command
      count_out++;
    }
//    Serial.println("Wake up!");
    delay(HALF_PERIOD);
//    Serial.println();
  }
}

void radio_init() {
  pinMode(RFM95_RST, OUTPUT);
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
  rf95.setTxPower(5, false);
}

void battery_level() {
  float vbat = analogRead(VBATPIN);
  vbat *= 2;    // we divided by 2, so multiply back
  vbat *= 3.3;  // Multiply by 3.3V, our reference voltage
  vbat /= 1024; // convert to voltage
  Serial.print("Battery: ");
  Serial.println(vbat);
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
