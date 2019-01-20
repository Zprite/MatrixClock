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
            if( millis()-prevMillis > 500) held= 1;
        }
        if (held) return 2; 
        return 1; // If pressed
    }
    return 0; // If no input
}
  void clock_core(unsigned long old_millis,unsigned long DIV_MS, uint8_t MOD){
    uint16_t h,m;
    m += (millis() - old_millis)/DIV_MS; 
    h += (m/60);
    m %= 60;
    h %= MOD;
    sprintf(currTime,"%02d%02d",h,m); // Convert integer to string
  }
  
void stopwatch (int input){
  static bool stop=1;
  static unsigned long stop_millis=0;
  static unsigned long start_millis=0;
  static unsigned long diff;
  if(input==1){
    stop = !stop;
    if(stop) stop_millis=millis();
    if(!stop){
      start_millis=millis();
      diff += start_millis-stop_millis;
    } 
  }
  if(!stop){
    clock_core(diff,1000,100);
  }
}

uint16_t h,m;

void clock(String timeIn){ 
  
  static unsigned long old_millis=0;
  if (t_has_changed==1){ // If serial input is recieved
    timeIn.toCharArray(input_buffer,sizeof(input_buffer));
    input_buffer[4]=NULL;
    strncpy(input_buffer_h,input_buffer,2);
    old_millis=millis();
    t_has_changed=0;
    h=0;
    Serial.print("timeIN (char): ");
    Serial.println(input_buffer);
  }
  sscanf(input_buffer_h,"%d",&h);
  sscanf(&input_buffer[2],"%d",&m);

  clock_core(old_millis,60000,24);
}
