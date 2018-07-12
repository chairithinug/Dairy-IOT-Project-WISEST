#include <RTCZero.h>
RTCZero rtc;
#define PERIOD 10

boolean alarmrang = false;

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  
  rtc.begin();
  
  rtc.setSeconds(0);
  
  rtc.setAlarmSeconds(PERIOD);
  rtc.enableAlarm(rtc.MATCH_SS);
  
  rtc.attachInterrupt(alarmMatch);
  
  rtc.standbyMode();
}

void loop()
{
  if (alarmrang)
  {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(1000);
    digitalWrite(LED_BUILTIN, LOW);
    rtc.setSeconds(0);
    alarmrang = false;
  }
  rtc.standbyMode();    // Sleep until next alarm match
}

void alarmMatch()
{
  alarmrang = true; // this cannot have any delay inside. Recommend sending some flag like this.
}
