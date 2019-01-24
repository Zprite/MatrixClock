#include <functions.h>
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include <string.h>

void setup(void)
{
  Serial.begin(9600);
  pinMode(BUTTON_PIN1,INPUT_PULLUP);
  pinMode(BUTTON_PIN2,INPUT_PULLUP);
  pinMode(BUZZER_PIN,OUTPUT);
  P.begin(MAX_ZONES);
  P.setInvert(false);
  for (uint8_t i=0; i<MAX_ZONES; i++) // Set Zones for display
  {
    P.setZone(i, ZONE_SIZE*i, (ZONE_SIZE*(i+1))-1);
  }
}

void loop(void)
{
  static int clock_mode=0;
  Serial.println(clock_mode);
  // Handle button input
  int button1_state = button_control(BUTTON_PIN1);
  int button2_state = button_control(BUTTON_PIN2);
  unsigned long buzz_millis;
  if (button1_state== HOLD){
    clock_mode++;
    buzz_millis=millis();
    digitalWrite(BUZZER_PIN,HIGH);
  } 
  if(millis()-buzz_millis > 300)  
    digitalWrite(BUZZER_PIN,LOW);

  clock_mode %= nMODES;
  select_mode(clock_mode,button1_state,button2_state);
  // Update display whenever a change occurs
  if(!strcmp(currTime,prevTime)){
    display();
  }
  strcpy(prevTime,currTime);
}