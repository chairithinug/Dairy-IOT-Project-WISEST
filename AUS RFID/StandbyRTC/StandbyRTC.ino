/*
  Sleep RTC Alarm for Arduino Zero

  Demonstrates the use an alarm to wake up an Arduino zero from Standby mode

  This example code is in the public domain

  http://arduino.cc/en/Tutorial/SleepRTCAlarm

  created by Arturo Guadalupi
  17 Nov 2015
  modified
  01 Mar 2016

  NOTE:
  If you use this sketch with a MKR1000 you will see no output on the serial monitor.
  This happens because the USB clock is stopped so it the USB connection is stopped too.
  **To see again the USB port you have to double tap on the reset button!**
*/

#include <RTCZero.h>

/* Create an rtc object */
RTCZero rtc;

/* Change these values to set the current initial time */
const byte seconds = 0;
const byte minutes = 00;
const byte hours = 17;

/* Change these values to set the current initial date */
const byte day = 17;
const byte month = 11;
const byte year = 15;

boolean alarmrang = false;

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  rtc.begin();

  rtc.setTime(hours, minutes, seconds);
  rtc.setDate(day, month, year);

  rtc.setAlarmDate(17, 11, 15);
  rtc.setAlarmTime(17, 00, 10);
  rtc.enableAlarm(rtc.MATCH_YYMMDDHHMMSS);

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
    
    alarmrang = false;
    rtc.setTime(hours, minutes, seconds);
    rtc.setDate(day, month, year);
    
    rtc.setAlarmDate(17, 11, 15);
    rtc.setAlarmTime(17, 00, 10);
    rtc.enableAlarm(rtc.MATCH_YYMMDDHHMMSS);
    
    rtc.attachInterrupt(alarmMatch);
    
  }
  rtc.standbyMode();    // Sleep until next alarm match
}

void alarmMatch()
{
  alarmrang = true;
}
