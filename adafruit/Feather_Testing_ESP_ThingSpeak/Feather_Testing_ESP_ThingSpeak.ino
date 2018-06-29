#include <SPI.h>
String AP = "iPhone";       // CHANGE ME
String PASS = "risktaker"; // CHANGE ME
String API = "I3N8WV2Z2SV5C3EW";   // CHANGE ME
String HOST = "api.thingspeak.com";
String PORT_NET = "80";
String field = "field1";
int countTrueCommand;
int countTimeCommand;
boolean found = false;
int valSensor = 1;

#define LED 5

void setup() {
  pinMode(LED, OUTPUT);
  
  Serial.begin(9600);
  Serial1.begin(115200);
  sendCommand("AT", 5, "OK");
  sendCommand("AT+CWMODE=1", 5, "OK");
  sendCommand("AT+CWJAP=\"" + AP + "\",\"" + PASS + "\"", 20, "OK");
}
void loop() {
  valSensor = getSensorData();
  String getData = "GET /update?api_key=" + API + "&" + field + "=" + String(valSensor);
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
int getSensorData() {
  return random(1000); // Replace with
}
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
