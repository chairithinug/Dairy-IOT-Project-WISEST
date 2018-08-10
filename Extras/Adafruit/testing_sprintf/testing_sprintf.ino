#include <SPI.h>

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
  char buff[10];
  int i = 16;
  sprintf(buff, "%x", i);
  for (int i = 0; i < sizeof(buff); i++)
  {
    Serial.print(buff[i]);
  }
  Serial.println("");
}
