#include <SPI.h>
#include <RH_RF95.h>

#define RFM95_CS 8 // Unique for Feather LoRa M0
#define RFM95_RST 4
#define RFM95_INT 3

#define RF95_FREQ 915.0 // Must be the same for RX-TX
RH_RF95 rf95(RFM95_CS, RFM95_INT);

#define LED 13 // On-board LED
#define NUMBER_OF_DEVICES 2 //TODO
#define DATA_LENGTH 18
#define ID_LENGTH 15

// Use String for the sake of ease
String AP = "UWNet"; // Need to register MAC address first (UWNet Only)
String PASS = "";
String API = "I3N8WV2Z2SV5C3EW"; // ThingSpeak.com Write API
String HOST = "api.thingspeak.com";
String PORT_NET = "80";
String field1 = "field1";

int count_command = 0;
int command_time = 0;
boolean found = false;
uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
uint8_t ongoing_data[DATA_LENGTH * NUMBER_OF_DEVICES + 1];

char list_ID[NUMBER_OF_DEVICES * ID_LENGTH];
char ID[ID_LENGTH];
int logged_devices = 0; // When a device has valid id and is registered the first time of that iteration
int bad_devices = 0; // When a device does not have a valid id, or a device fails
char OUT[2] = "1"; // A response to a sender
int comm_count = 0; // Used as a timeout.

void setup()
{
  device_init();
}

void loop()
{
  if (rf95.available()) {
    transmit(OUT, sizeof(OUT)); // signal that it received data
    digitalWrite(LED, HIGH);
    uint8_t len = sizeof(buf);
    if (rf95.recv(buf, &len))
      Serial.println((char*) buf);
    else
      Serial.println("Receive failed");

    strncpy(ID, (char*) buf, 15); // copy the ID to be compared
    Serial.print("This ID is ");
    Serial.println(ID);

    if (!is_repeated_ID(ID, logged_devices)) {
      Serial.print(ID);
      Serial.println(" logged");
      Serial.println();
      memcpy(list_ID + logged_devices * ID_LENGTH, ID, ID_LENGTH); // Add this ID to a list

      memcpy(ongoing_data + logged_devices * DATA_LENGTH, buf, DATA_LENGTH); // Attach this data to a package
      logged_devices++;
    }

    comm_count++;
    memset(&ID[0], 0, sizeof(ID)); // Clear memory with null
    delay(100);
    digitalWrite(LED, LOW);
  }
  if (logged_devices + bad_devices >= NUMBER_OF_DEVICES || comm_count > 2 * NUMBER_OF_DEVICES) { //comm_count is a timeout condition
    Serial.print("sending data from ");
    Serial.print(logged_devices);
    Serial.print(" out of ");
    Serial.print(NUMBER_OF_DEVICES);
    Serial.println(" devices");
    Serial.print((char*) ongoing_data);
    Serial.println();
    post2API();
    logged_devices = 0;
    comm_count = 0;
    memset(&ongoing_data[0], 0, sizeof(ongoing_data)); // Clear memory with null
    memset(&list_ID[0], 0, sizeof(list_ID));
  }
}

/////////////////////////////////////////////////////////////////////////////////
// Setup the program
/////////////////////////////////////////////////////////////////////////////////

void device_init() {
  radio_init();
  Serial.begin(115200);
  Serial1.begin(115200);
  wifi_init();
  memset(&buf[0], 0, RH_RF95_MAX_MESSAGE_LEN); // Clear memory with null
  memset(&ongoing_data[0], 0, sizeof(ongoing_data));
  memset(&list_ID[0], 0, sizeof(list_ID));
  memset(&ID[0], 0, sizeof(ID));
  digitalWrite(LED, LOW);
}

/////////////////////////////////////////////////////////////////////////////////
// Setup LoRa communication
/////////////////////////////////////////////////////////////////////////////////

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

/////////////////////////////////////////////////////////////////////////////////
// Setup WiFi module
/////////////////////////////////////////////////////////////////////////////////

void wifi_init() {
  send_command("AT", 5, "OK"); // Check ESP8266
  send_command("AT+CWMODE=1", 5, "OK"); // Set station mode
  send_command("AT+CWJAP=\"" + AP + "\",\"" + PASS + "\"", 20, "OK"); // Connect to WiFi
}

/////////////////////////////////////////////////////////////////////////////////
// (ESP8266) send command and wait for expected reply before timeout
// Parameters
//            command: AT command to ESP8266
//            max_time: a timeout
//            read_reply: an array of expected data from ESP8266
/////////////////////////////////////////////////////////////////////////////////

void send_command(String command, int max_time, char read_reply[]) {
  Serial.print(count_command);
  Serial.print(". ");
  Serial.print(command);
  Serial.print(" ");
  while (command_time < (max_time * 1))
  {
    Serial1.println(command); // AT+CIPSEND
    Serial.write(Serial1.read());
    if (Serial1.find(read_reply)) // OK
    {
      found = true;
      break;
    }
    command_time++;
  }
  if (found == true)
  {
    Serial.println("OK");
    count_command++;
    command_time = 0;
  }
  if (found == false)
  {
    Serial.println(" Fail");
    count_command = 0;
    command_time = 0;
  }
  found = false;
}

/////////////////////////////////////////////////////////////////////////////////
// Request an update to Thingspeak
/////////////////////////////////////////////////////////////////////////////////

void post2API() {
  String getData = "GET /update?api_key=" + API + "&" + field1 + "=x" + String((char*) ongoing_data);// Add x to be considered string in Matlab.
  send_command("AT+CIPMUX=1", 20, "OK"); // Multiple connection
  send_command("AT+CIPSTART=0,\"TCP\",\"" + HOST + "\"," + PORT_NET, 15, "OK"); // Establish TCP
  send_command("AT+CIPSEND=0," + String(getData.length() + 50), 4, ">"); // Set data length
  Serial1.println(getData);
  send_command("AT+CIPCLOSE=0", 5, "OK"); // Close TCP
  Serial.println();
}

/////////////////////////////////////////////////////////////////////////////////
// Check if this ID has been logged already
// Parameters
//            ID: a pointer to the ID of a new device
//            logged_devices: a number of devices that have been logged previously
/////////////////////////////////////////////////////////////////////////////////

boolean is_repeated_ID(char* ID, int logged_devices) {
  for (int i = 0; i < logged_devices; i++) {
    Serial.print("comparing ");
    Serial.print(ID);
    Serial.print(" and ");
    Serial.println((list_ID + i * ID_LENGTH));
    if (strncmp((char*) (list_ID + i * ID_LENGTH), ID, ID_LENGTH) == 0) {
      Serial.println("This ID has already logged");
      Serial.println();
      return true;
    }
  }
  return false;
}

/////////////////////////////////////////////////////////////////////////////////
// Send data to the hub using LoRa
// Parameters
//            sendpacket: a pointer of a data array
//            LENGTH: a number of characters to be sent
/////////////////////////////////////////////////////////////////////////////////

void transmit(char sendpacket[], int LENGTH) {
  Serial.println("Replying someone");
  rf95.send((uint8_t *)sendpacket, LENGTH);
  rf95.waitPacketSent();
}
