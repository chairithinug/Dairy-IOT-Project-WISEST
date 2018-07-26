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
String field1 = "field1";
String field2 = "field2";

int count_command = 0;
int command_time = 0;
boolean found = false;
uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];

void setup()
{
  radio_init();
  Serial.begin(115200);
  Serial1.begin(115200);
  wifi_init();
  memset(&buf[0], 0, RH_RF95_MAX_MESSAGE_LEN); // Clear memory with null
  digitalWrite(LED, LOW);
}

void loop()
{
  if (rf95.available())
  {
    digitalWrite(LED, HIGH);
    uint8_t len = sizeof(buf);
    if (rf95.recv(buf, &len))
      Serial.println((char*) buf);
    else
      Serial.println("Receive failed");
z
    post2API();
    delay(100);
    digitalWrite(LED, LOW);
  }
}

void radio_init() {
  pinMode(LED, OUTPUT);
  pinMode(RFM95_RST, OUTPUT);
  // manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);
  while (!rf95.init())
    while (1);
  if (!rf95.setFrequency(RF95_FREQ))
    while (1);
  rf95.setTxPower(23, false);
}

void wifi_init() {
  sendCommand("AT", 5, "OK"); // Check ESP8266
  sendCommand("AT+CWMODE=1", 5, "OK"); // Set station mode
  sendCommand("AT+CWJAP=\"" + AP + "\",\"" + PASS + "\"", 20, "OK"); // Connect to WiFi
}

// (ESP8266) send command and wait for expected reply before timeout
void sendCommand(String command, int maxTime, char readReplay[]) {
  Serial.print(count_command);
  Serial.print(". ");
  Serial.print(command);
  Serial.println(" ");
  while (command_time < (maxTime * 1))
  {
    Serial1.println(command); // AT+CIPSEND
    if (Serial1.find(readReplay)) // OK
    {
      found = true;
      break;
    }
    command_time++;
  }
  if (found == true)
  {
    count_command++;
    command_time = 0;
  }
  if (found == false)
  {
    Serial.println("Fail");
    count_command = 0;
    command_time = 0;
  }
  found = false;
}

void post2API() {
  String getData = "GET /update?api_key=" + API + "&" + field1 + "=x" + String((char*) buf) + "&" + field2 + "=" + rf95.lastRssi(); // Add x at the beginning so that it is considered string in Matlab table.
  sendCommand("AT+CIPMUX=1", 5, "OK"); // Multiple connection
  sendCommand("AT+CIPSTART=0,\"TCP\",\"" + HOST + "\"," + PORT_NET, 15, "OK"); // Establish TCP
  sendCommand("AT+CIPSEND=0," + String(getData.length() + 4), 4, ">"); // Set data length
  Serial1.println(getData);
  sendCommand("AT+CIPCLOSE=0", 5, "OK"); // Close TCP
}

