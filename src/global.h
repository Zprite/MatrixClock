
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include <string.h>
#include <Wire.h>
#include "RTClib.h"

#define BUZZER_PIN 4
#define INTENSITY_PIN 9
#define BUTTON_PIN1 3
#define BUTTON_PIN2 2
#define CLK_PIN   13
#define DATA_PIN  11
#define CS_PIN    10

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4
#define MAX_ZONES   2
#define ZONE_SIZE (MAX_DEVICES/MAX_ZONES)

#define nMODES 4        // Number of clock modes
#define STOPWATCH 2
#define MINS_SECS 1
#define HRS_MINS 0

#define HOLD 2
#define PUSH 1

#define HOLD_TIME 300
#define SCROLL_SPEED 30
#define SPEED_TIME  20
#define PAUSE_TIME  0

extern MD_MAX72XX mx = MD_MAX72XX(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);
extern MD_Parola P = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);
extern RTC_DS3231 rtc;
// Turn on debug statements to the serial output
extern const int DEBUG = 0;
// Global variables
 extern char char_top[3]={'0','0','\0'};
 extern char char_bottom[3]={'0','0','\0'};

//Functions
 void display();
 void display_text(char *input_string);
 int button_control (uint8_t buttonPin);
 void intesinty_control(uint8_t button);
 void print_hh_mm();
 void print_mm_ss();
 void print_date();
 void default_clock(uint8_t button1, uint8_t button2);
 bool set_time(uint8_t button1, uint8_t button2);
 void alarm(uint8_t button1, uint8_t button2, bool setmode);
 void stopwatch (uint8_t button1, uint8_t button2);
 bool timer_core(bool reset, unsigned long start_seconds, int h, int m);
 void set_timer(uint8_t button1, uint8_t button2);
 void select_mode();


