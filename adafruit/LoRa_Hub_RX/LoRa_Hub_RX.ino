#include <SPI.h>
#include <RH_RF95.h>

#define RFM95_CS 8 // Unique for Feather LoRa M0
#define RFM95_RST 4
#define RFM95_INT 3

#define RF95_FREQ 915.0 // Must be the same for RX-TX
RH_RF95 rf95(RFM95_CS, RFM95_INT);

#define LED 13 // On-board LED

String AP = "UWNet"; // Need to register MAC address first (UWNet Only)
String PASS = "";
String API = "I3N8WV2Z2SV5C3EW"; // ThingSpeak.com Write API
String HOST = "api.thingspeak.com";
String PORT_NET = "80";
String field = "field1";
int countTrueCommand;
int countTimeCommand;
boolean found = false;
int valSensor = 1;

void setup()
{
  radio_init();
  Serial.begin(115200);
  delay(100);
  //WiFi
  Serial1.begin(115200);
  sendCommand("AT", 5, "OK");
  sendCommand("AT+CWMODE=1", 5, "OK");
  sendCommand("AT+CWJAP=\"" + AP + "\",\"" + PASS + "\"", 20, "OK");
}

void loop()
{
  digitalWrite(LED, LOW);
  if (rf95.available())
  {
    digitalWrite(LED, HIGH);
    delay(100); // to make LED visible  
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    if (rf95.recv(buf, &len))
    {
      Serial.print((char*) buf);
      //Serial.print(" RSSI: "); // received signal strength indicator (the less negative the better)
      //Serial.println(rf95.lastRssi(), DEC);
    }
    else
      Serial.println("Receive failed");

    // Posting to channel
    String getData = "GET /update?api_key=" + API + "&" + field + "=x" + String((char*) buf); // Added x at the beginning so that it is considered string in Matlab table.
    sendCommand("AT+CIPMUX=1", 5, "OK");
    sendCommand("AT+CIPSTART=0,\"TCP\",\"" + HOST + "\"," + PORT_NET, 15, "OK");
    sendCommand("AT+CIPSEND=0," + String(getData.length() + 4), 4, ">");
    digitalWrite(LED, HIGH);
    Serial1.println(getData);
    delay(1500);
    digitalWrite(LED, LOW);
    countTrueCommand++;
    sendCommand("AT+CIPCLOSE=0", 5, "OK");
  }
}

// (ESP8266) send command and wait for expected reply before timeout
void sendCommand(String command, int maxTime, char readReplay[]) {
  Serial.print(countTrueCommand);
  Serial.print(". at command => ");
  Serial.print(command);
  Serial.print(" ");
  while (countTimeCommand < (maxTime * 1))
  {
    Serial1.println(command);//at+cipsend
    if (Serial1.find(readReplay)) //ok
    {
      found = true;
      break;
    }
    countTimeCommand++;
  }
  if (found == true)
  {
    Serial.println("OYI");
    countTrueCommand++;
    countTimeCommand = 0;
  }
  if (found == false)
  {
    Serial.println("Fail");
    countTrueCommand = 0;
    countTimeCommand = 0;
  }
  found = false;
}

void radio_init(){
  pinMode(LED, OUTPUT);
  pinMode(RFM95_RST, OUTPUT);  
  digitalWrite(RFM95_RST, HIGH);
  // manual reset
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
  Serial.print("Set Freq to: ");
  Serial.println(RF95_FREQ);
  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on
  // The default transmitter power is 13dBm, using PA_BOOST.
  // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then
  // you can set transmitter powers from 5 to 23 dBm:
  rf95.setTxPower(23, false);
}

