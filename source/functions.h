#include <Arduino.h>
#include <global.h>
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include <string.h>

void display(){
  static uint8_t  curZone = 0;
  strncpy (buff_h,currTime,2);    // Extract hours and minutes from currTime into buffer
  strncpy (buff_m,&currTime[2],2);
  P.displayZoneText (1,buff_h, PA_CENTER, SPEED_TIME, PAUSE_TIME, PA_PRINT ,PA_NO_EFFECT);
  P.displayZoneText (0,buff_m, PA_CENTER, SPEED_TIME, PAUSE_TIME, PA_PRINT ,PA_NO_EFFECT);
  while (!P.getZoneStatus (curZone))
    P.displayAnimate(); // Refresh display
}


int button_control (uint8_t buttonPin){ //Button needs to use a pull-up resistor!
    bool input = digitalRead (buttonPin);
    bool held = 0;
    if (!input){
      unsigned long prevMillis=millis();
      
      while (!digitalRead(buttonPin)) {
          if ( millis()-prevMillis > HOLD_TIME) held= 1;
      }
      if (held) 
        return 2; 

      else{
        static unsigned long exit_millis;
        if (millis()-exit_millis > 50){ // To remove accidental "double" inputs
          exit_millis=millis();
          return 1; // If pressed
        }
      }
    }
  return 0; // If no input
}


void clock_core (unsigned long old_millis,unsigned long DIV_MS, uint8_t MOD, uint16_t h, uint16_t m){
  m += (millis() - old_millis)/DIV_MS; 
  h += (m/60);
  m %= 60;
  h %= MOD;
  sprintf (currTime,"%02d%02d",h,m); // Convert integer to string
}


void clock (int button1, int button2){ 
  static uint8_t m,h;
  if (button1 == 1){ // Manually set minutes
    m++;
  }
  else if (button2==1){ //Maually set hours
    h++;
  }

  clock_core(0,60000,24,h,m);
}


void stopwatch (int button1, int button2){
  static bool stop=1;
  static unsigned long stop_millis=0;
  static unsigned long start_millis=0;
  static unsigned long diff;
  if (button1 == 2)
    sprintf(currTime,"%02d%02d",00,00); //  Set time to 0
  if (button2 == 1){
    if (stop) 
      diff= stop_millis;
    else{
      stop=1;
      stop_millis=millis();
      diff= stop_millis;
    }

    clock_core(diff,1000,100,0,0);
  }

  if (button1==1){ // Each time user presses stop/start button
    stop = !stop;
    if (stop) 
      stop_millis=millis(); // Store time when stopped

    if (!stop){
      start_millis=millis(); // Store time when started
      diff += start_millis-stop_millis; // Subtracts total stopped time from millis() to display correct time
    } 
  }
  if (!stop) //  Update time whenever clock is started
    clock_core(diff,1000,100,0,0);
}


bool timer_core(unsigned long start_millis, int h, int m,bool reset){
  static int m_count=0;
  static int h_count=0;
  if (reset){ // Reset static variables
    m_count=0;
    h_count=0;
  }  

  for (uint8_t i=0;i<m_count;i++){
    m+=60;
  }
  for (uint8_t i=0;i<h_count;i++){
    h--;
  }

  m -= (millis()-start_millis)/1000; 
  if (m<0){
    h_count++;
    m_count++;
  }

  sprintf(currTime,"%02d%02d",h,m); // Convert integer to string
  if (strcmp("0000",currTime)==0){ // Return 1 when timer reaches 0
    m_count=0;
    h_count=0;
    return 1;
  }
  return 0;
}


void timer(int button1, int button2){
  static uint8_t h = 0;
  static uint8_t m = 10;
  static bool start = 0;
  static bool reset=0;
  Serial.println("RESET:");
  Serial.print(reset);
  static unsigned long start_millis=0;
  if (start==0)  
    sprintf(currTime,"%02d%02d",h,m);

  if (start==1){
    if (timer_core(start_millis,h,m,reset)==1){
      display();
      while(button_control(BUTTON_PIN1)!=1){
        digitalWrite(BUZZER_PIN,HIGH);
      }
      start=0;
    } 
    reset = 0;
  }

  if (button1==PUSH && start==0){
    start = 1;
    start_millis=millis();
  }
  else if (button1==PUSH && start==1){
    start = 0;
    reset=1;
  }

  if (button2==PUSH && start==0)
    h++;
  if (button2==HOLD && start==0)
    m++;
}


void select_mode(int mode, int button1, int button2){
  switch (mode){
    case 0: 
      Serial.println("Mode: CLOCK ");
      clock(button1,button2);
      break;
    case 1: 
      Serial.println("Mode: STOPWATCH");
      stopwatch(button1,button2); 
      break;
    case 2:
      Serial.println("Mode: TIMER");
      timer(button1,button2);
      break;
  }
}