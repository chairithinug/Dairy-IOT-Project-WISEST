#include <SoftwareSerial.h>
SoftwareSerial mySerial(8, 9); // RX, TX

void setup()
{
  // Open connection to host at 38400bps
  Serial.begin(9600);

  // Open connection to RFID module at 9600bps
  mySerial.begin(9600);

  /*Setup reader for FDX-B RFID */
  mySerial.write("ST2\r");
  delay(100);
  while(mySerial.available()) {
    Serial.print(mySerial.read());
  }
  Serial.println("Setup Complete");
}

enum state {WAIT, SEND_RAT, PRINT_RESPONSE};

state currentState = WAIT;
unsigned int timeout = 0;

int rfidCount = 0;

char rfidData[100];

int countryCodeCount = 0;
char countryCode[20];

int nationalCount = 0 ;
char nationalIdentityCode[30];

int crcCount = 0;
char crc[20];

int myBodyBitStatus = 0;
int dataBitStatus = 0;

int temperatureCount = 0;
char temperature[20];
char temperatureBytes[3];


void loop()
{
  char returnedByte;
  switch (currentState) {
    case WAIT:
      //if there is data print it out, otherwise code has finished being returned
      if (mySerial.available()) {
        returnedByte = mySerial.read();
        //Serial.write(returnedByte);
        if (returnedByte == '\r') {
          //end of data, next step
          currentState = SEND_RAT;
        }
      }
      break;

    case SEND_RAT:
      mySerial.write("RAT\r");
      currentState = PRINT_RESPONSE;
      break;

    case PRINT_RESPONSE:
      if (mySerial.available()) {
        returnedByte = mySerial.read();

        if (returnedByte == '_') {
          //When we receive underscore we know that the next part of the data is next.
          rfidCount ++;
        }
        else if (rfidCount == 0) {
          //First part of the data is the country code
          //countryCode.concat(returnedByte);
          countryCode[countryCodeCount] = returnedByte;
          countryCodeCount ++;
        }
        else if (rfidCount == 1) {
          //Second part of the data is the National Identity Code
          //nationalIdentityCode.concat(returnedByte);
          nationalIdentityCode[nationalCount] = returnedByte;
          nationalCount ++;
        }
        else if (rfidCount == 2) {
          //third part is the myBody bit status
          if (returnedByte) {
            myBodyBitStatus = 1;
          }
          else {
            myBodyBitStatus = 0;
          }
        }
        else if (rfidCount == 3) {
          //Fourth part is the data bit status
          if (returnedByte) {
            dataBitStatus = 1;
          }
          else {
            dataBitStatus = 0;
          }
        }
        else if (rfidCount == 4) {
          //Fifth part is the CRC
          //crc.concat(returnedByte);
          crc[crcCount] = returnedByte;
          crcCount ++;
        }

        else if (rfidCount == 5) {
          //Sixth part is the temperature
          //temperature.concat(returnedByte);
          temperature[temperatureCount] = returnedByte;
          temperatureCount ++;
        }

        else if (returnedByte == '\r') {
          currentState = WAIT;
          rfidCount = 0; //reset counter
          //Print everything
          Serial.print("Country Code: ");
          Serial.println(countryCode);
          Serial.print("National Identity Code: ");
          Serial.println(nationalIdentityCode);
          Serial.print("myBody Bit Status: ");
          if (myBodyBitStatus == 1) {
            Serial.println("True");
          }
          else {
            Serial.println("False");
          }
          Serial.print("Data Bit Status: ");
          if (dataBitStatus == 1) {
            Serial.println("True");
          }
          else {
            Serial.println("False");
          }
          Serial.print("Temperature: ");

          //Convert temp hex to dec

          //Just use the last two bytes, ignore the 1
          for(int i = 0; i < 2; i++){
            temperatureBytes[i] = temperature[i + 4];
          }
          
          char * ptr;
          long decTemp = strtol(temperatureBytes, &ptr, 16);
          
          Serial.print(decTemp);
          Serial.println('C');
          Serial.println(' ');

          //Reset variables here
          countryCodeCount = 0;
          nationalCount = 0;
          crcCount = 0;
          myBodyBitStatus = 0;
          dataBitStatus = 0;
          temperatureCount = 0;
        }

      }
      break;
  }


  if (Serial.available())
    mySerial.write(Serial.read());

}


