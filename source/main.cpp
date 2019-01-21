#include <functions.h>
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include <string.h>

MD_Parola P = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);

void display(){
  static uint8_t  curZone = 0;
  strncpy(buff_h,currTime,2);    // Extract hours and minutes from currTime into buffer
  strncpy(buff_m,&currTime[2],2);
  P.displayZoneText(1,buff_h, PA_CENTER, SPEED_TIME, PAUSE_TIME, PA_PRINT ,PA_NO_EFFECT);
  P.displayZoneText(0,buff_m, PA_CENTER, SPEED_TIME, PAUSE_TIME, PA_PRINT ,PA_NO_EFFECT);

  while (!P.getZoneStatus(curZone))
    P.displayAnimate(); // Refresh display
}

void setup(void)
{
  Serial.begin(9600);
  pinMode(BUTTON_PIN1,INPUT_PULLUP);
  pinMode(BUTTON_PIN2,INPUT_PULLUP);
  P.begin(MAX_ZONES);
  P.setInvert(false);
  for (uint8_t i=0; i<MAX_ZONES; i++)
  {
    P.setZone(i, ZONE_SIZE*i, (ZONE_SIZE*(i+1))-1);
  }
}

void loop(void)
{
  // Handle serial input
   static int clock_mode=0;
   Serial.println(clock_mode);
  //clock(timeIn);
  // Handle button input
  int button1_state = button_control(BUTTON_PIN1);
  int button2_state = button_control(BUTTON_PIN2);
  if (button1_state== HOLD) clock_mode++;
  clock_mode %= nMODES;
  select_mode(clock_mode,button1_state,button2_state);

  // Handle display
  if(!strcmp(currTime,prevTime)){
    display();
  }
  strcpy(prevTime,currTime);
}