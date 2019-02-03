#include <functions.h>
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include <Wire.h>
#include "RTClib.h"

void setup(void)
{
  Serial.begin(9600);
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

  // Set up RTC
  // VCC -> +5V
  // GND -> GND
  // SDA -> SDA (A4)
  // SCL -> SCL (A5) 
   setup_rtc();
}

void loop(void)
{
  select_mode();
  display(); // Update display 
}