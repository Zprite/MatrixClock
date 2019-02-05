#include <functions.h>
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include <Wire.h>
#include "RTClib.h"

void setup(void)
{
  Serial.begin(9600);
  pinMode(INTENSITY_PIN,INPUT_PULLUP);
  pinMode(BUTTON_PIN1,INPUT_PULLUP);
  pinMode(BUTTON_PIN2,INPUT_PULLUP);
  pinMode(BUZZER_PIN,OUTPUT);
  
  // Set up display
  P.begin(MAX_ZONES);
  P.setInvert(false);
  for (uint8_t i=0; i<MAX_ZONES; i++) // Set Zones for display
  {
    P.setZone(i, ZONE_SIZE*i, (ZONE_SIZE*(i+1))-1);
  }
  P.setIntensity(1,0);
  P.setIntensity(0,0);
  // Set up RTC
  // VCC -> +5V
  // GND -> GND
  // SDA -> SDA (A4)
  // SCL -> SCL (A5) 
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  if (rtc.lostPower()) {
    int buttonstate_1;
    int buttonstate_2;
    while(true){
     buttonstate_1 = button_control(BUTTON_PIN1);
     buttonstate_2 = button_control(BUTTON_PIN2);
     display();
      if(set_time(buttonstate_1,buttonstate_2)==1) // Exit loop when time is set
        break;
    }
  }
}

void loop(void)
{
  intesinty_control(INTENSITY_PIN);
  select_mode();
  display(); // Update display 
}