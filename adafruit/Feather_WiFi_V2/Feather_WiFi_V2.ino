#include <SPI.h>
#include <RH_RF95.h>
#include "ESP8266WiFi.h"

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

const int PACKET_LENGTH = 2 * DATA_NUM;
byte data_read, data_shift;
int shift_count = 0;
int index_byte = 0;
char packet[PACKET_LENGTH] = {};

const char WEBSITE[] = "api.pushingbox.com"; //pushingbox API server
const String devid = "<YOUR_Devid_HERE>"; //device ID from Pushingbox

const char* MY_SSID = "<YOUR_WIFI_NAME_HERE";
const char* MY_PWD =  "<YOUR_WIFI_PASSWORD_HERE>";

void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}
