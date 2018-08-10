#include <SPI.h>
#include <RH_RF95.h>

#define RFM95_CS 8 // Unique for Feather LoRa M0
#define RFM95_RST 4
#define RFM95_INT 3

#define RF95_FREQ 915.0
RH_RF95 rf95(RFM95_CS, RFM95_INT);

#define PERIOD 20000// how long roughly (ms) between two reading

#define LED 13 // On-board LED
#define VBATPIN A7 // Pin 9 (analog 7)
//#define RST_POW 6 // Reset transistor to keep reading

const int PACKET_LENGTH = 48;
const int RADIO_PACKET_LENGTH = 23;
const int HALF_PERIOD = PERIOD / 2;
int index_data = 0;
int index_radio = 0;
char packet[PACKET_LENGTH] = {}; // Contains raw data
char radiopacket[RADIO_PACKET_LENGTH] = {}; // Contains edited data that is going to be sent
boolean id = true;

void setup()
{
  //  pinMode(RST_POW, OUTPUT);
  //  digitalWrite(RST_POW, HIGH);
  //  delay(10);
  //  digitalWrite(RST_POW, LOW);
  //  delay(10);

  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);
  Serial.begin(115200);
  radio_init();
  Serial1.begin(9600);
  //  Serial1.write("SD2\r"); // Set default mode to Animal Tag
  memset(&packet[0], 0, sizeof(packet)); // initialize memory with null
  rf95.sleep();
  delay(1000);
  Serial.println("Ready");
  //  Serial1.write("SRA\r"); // Enable the reader to read as opposed to SRD
}

void loop()
{
  if (Serial1.available()) {
    digitalWrite(LED, HIGH);
    int c = Serial1.read();
    if (c != '9' && index_data == 0){ // Do not start if the first number is not 9
      index_data == 0;
      while(Serial1.available()) // Must be tested!
      {
        int a = Serial1.read(); // Clear all the following bit
        delay(1);
      }
    }
    if (c == '\r') {
      if (id) { // Get the first part of data (the data received from the sensor without RAT)
        //        Serial.write(";");
        Serial.flush();
        //        Serial.println("Send RAT");
        Serial1.write("RAT\r"); // Read more detailed data
        Serial1.flush();
        delay(50);
      }
      id = !id;
    }
    else {
      packet[index_data++] = c; // This array contains all raw data
      if (index_data == 46) // 46th number
      {
        c = B00110001; // 31 in HEX which is 1 is ASCII. This bit is always one!
      }
      if ((index_data >= 17 && index_data != 20 && index_data != 33 && index_data != 35 && index_data < 37) || index_data >= 43) {
      Serial.write(c);
      Serial.flush();
      radiopacket[index_radio++] = c; // This array contains only data going to be sent
      }
    }
    if (index_data >= PACKET_LENGTH)
    {
      Serial.println(" Transmitted");
      transmit(radiopacket, RADIO_PACKET_LENGTH);
      index_data = 0;
      index_radio = 0;
      memset(&radiopacket[0], 0, sizeof(radiopacket)); // clear memory with null
      memset(&packet[0], 0, sizeof(packet)); // clear memory with null
      //      digitalWrite(RST_POW, HIGH);
      //      rf95.sleep();
      //      delay(2*PERIOD);
      //      digitalWrite(RST_POW, LOW);
    }
    digitalWrite(LED, LOW);
  }
  else {
    //    Serial.println("Sleeping");

    //      digitalWrite(RST_POW, HIGH);
    rf95.sleep();
    //      delay(2*PERIOD);
    //      digitalWrite(RST_POW, LOW);
    //      delay(2*PERIOD);

    Serial1.write("SRD\r");
    Serial1.flush();
    delay(10);
    int count_out = 0;
    while (Serial1.available() && count_out < 3) {
      int a = Serial1.read(); // Clear response of SRD command
      count_out++;
    }
    delay(PERIOD);
    //    Serial.println("Wake up!");
    Serial1.write("SRA\r");
    Serial1.flush();
    delay(10);
    count_out = 0;
    while (Serial1.available() && count_out < 3) {
      int b = Serial1.read(); // Clear response of SRA command
      count_out++;
    }
    delay(PERIOD / 32); // Must be at least 100 ms

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
    Serial.flush();
    while (1);
  }
  Serial.println("LoRa OK!");
  Serial.flush();
  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    Serial.flush();
    while (1);
  }
  Serial.print("Freq: ");
  Serial.println(RF95_FREQ);
  Serial.flush();
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
  Serial.flush();
}

void transmit(char radiopacket[], int RADIO_PACKET_LENGTH) {
  //  Serial.println("Sending");
  delay(10);
  rf95.send((uint8_t *)radiopacket, RADIO_PACKET_LENGTH);
  delay(10);
  rf95.waitPacketSent();
  //  Serial.println("End Sending");
  Serial.flush();
}
