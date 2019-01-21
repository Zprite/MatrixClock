
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include <string.h>

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

// Turn on debug statements to the serial output
extern const int DEBUG = 0;
// Global variables
 extern bool t_has_changed=0;
 extern char buff_h [3]={'0','0','\0'};
 extern char buff_m[3]={'0','0','\0'};
 extern char input_buffer_h[3]={'0','0','\0'};
 extern char input_buffer_m[3]={'0','0','\0'};
 extern char oldTime_h[3]={'0','0','\0'};
 extern char input_buffer[5]={'0','0','0','0','\0'};
 extern char currTime[5]={'0','0','0','0','\0'};
 extern char prevTime[5]={'0','0','0','0','\0'};
 extern char startTime[5]={'0','0','0','0','\0'};
 