#include <Arduino.h>
#include <global.h>
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include <string.h>

int button_control(uint8_t buttonPin){ //Button needs to use a pull-up resistor!
    bool input = digitalRead(buttonPin);
    bool held = 0;
    if (!input){
        unsigned long prevMillis=millis();
        while(!digitalRead(buttonPin)) {
            if( millis()-prevMillis > HOLD_TIME) held= 1;
        }
        if (held) return 2; 
        return 1; // If pressed
    }
    return 0; // If no input
}
  void clock_core(unsigned long old_millis,unsigned long DIV_MS, uint8_t MOD, uint16_t h, uint16_t m){
    m += (millis() - old_millis)/DIV_MS; 
    h += (m/60);
    m %= 60;
    h %= MOD;
    sprintf(currTime,"%02d%02d",h,m); // Convert integer to string
  }

void stopwatch (int button1, int button2){
  static bool stop=1;
  static unsigned long run_millis=0;
  static unsigned long stop_millis=0;
  static unsigned long start_millis=0;
  static unsigned long diff;
  if (button2 == 1){
    if(stop) diff= stop_millis;
    else{
      stop=1;
      stop_millis=millis();
      diff= stop_millis;
    } 
    clock_core(diff,1000,100,0,0);
  }
  if(button1==1){
    stop = !stop;
    if(stop) stop_millis=millis();
    if(!stop){
      start_millis=millis();
      diff += start_millis-stop_millis;
    } 
  }
  if(!stop){
    clock_core(diff,1000,100,0,0);
  }
}

void clock(int button2){ 
  static uint8_t m,h;
  if (button2 == 1){
    m++;
  }
  if (button2==2){
    h++;
  }
  clock_core(0,60000,24,h,m);
}

void select_mode(int mode, int button1, int button2){
  switch (mode){
    case 0: 
      Serial.println("Mode: CLOCK ");
      clock(button2);
      break;
    case 1: 
      Serial.println("Mode: STOPWATCH");
      stopwatch(button1,button2); 
      break;
  }
}