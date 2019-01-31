
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include <string.h>
#include <Wire.h>
#include "RTClib.h"

#define BUZZER_PIN 4
#define BUTTON_PIN1 3
#define BUTTON_PIN2 2
#define HOLD 2
#define PUSH 1
#define HOLD_TIME 300

#define nMODES 2
#define STOPWATCH 2
#define MINS_SECS 1
#define HRS_MINS 0

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4
#define MAX_ZONES   2
#define ZONE_SIZE (MAX_DEVICES/MAX_ZONES)

#define CLK_PIN   13
#define DATA_PIN  11
#define CS_PIN    10
#define SPEED_TIME  20
#define PAUSE_TIME  0

extern MD_Parola P = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);
extern RTC_DS3231 rtc;
// Turn on debug statements to the serial output
extern const int DEBUG = 0;
// Global variablesd
 extern char char_top[3]={'0','0','\0'};
 extern char char_bottom[3]={'0','0','\0'};

 