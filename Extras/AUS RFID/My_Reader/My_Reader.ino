#include <SoftwareSerial.h>
SoftwareSerial mySerial(8, 9); // RX, TX
void setup()
{
  Serial.begin(9600);
  while (!Serial)
    delay(1);
  mySerial.begin(9600);
  mySerial.write("SD2\r");
  mySerial.write("ST2\r");
  delay(1000);
  while (mySerial.available())
    Serial.print(mySerial.read(),HEX);
  Serial.println("Complete");
  mySerial.flush();
}

bool id = false;

void loop()
{
  while (mySerial.available()){
    if (!id){
      mySerial.write("RAT\r");
    }
    int c = mySerial.read();
    Serial.write(c);
//    Serial.write(" ");
    id = true;
    delay(1);
  }
  if (id){
    Serial.write("\r\n");
  }
  id = false;
}
