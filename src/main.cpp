#include "global.h "
#include "Font_Data.h"
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include <Wire.h>
#include "RTClib.h"

void setup(void)
{
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
    //Serial.println("Couldn't find RTC");
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


void display(){
  P.setFont(0, nullptr);
  P.setFont(1, nullptr);
  static uint8_t  curZone = 0;
  P.displayZoneText (1,char_top, PA_CENTER, SPEED_TIME, PAUSE_TIME, PA_PRINT ,PA_NO_EFFECT);
  P.displayZoneText (0,char_bottom, PA_CENTER, SPEED_TIME, PAUSE_TIME, PA_PRINT ,PA_NO_EFFECT);
  while (!P.getZoneStatus (curZone))
    P.displayAnimate(); // Refresh display
}

void display_text(char *input_string){        
  P.displayClear(1);
  P.displayClear(0);
  P.setFont(0, BigFontLower);
  P.setFont(1, BigFontUpper);
  P.displayZoneText(1, input_string, PA_LEFT, SCROLL_SPEED, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
  P.displayZoneText(0, input_string, PA_RIGHT, SCROLL_SPEED, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
  static uint8_t  curZone = 1;
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
      static unsigned long exit_millis;
      if (held) {
        if (millis()-exit_millis > 50){ // To remove accidental "double" inputs
          exit_millis=millis();
          return 2; // If pressed
        }
      }

      else{
        if (millis()-exit_millis > 50){ // To remove accidental "double" inputs
          exit_millis=millis();
          return 1; // If pressed
        }
      }
    }
  return 0; // If no input
}


void intesinty_control(uint8_t button){
	static uint8_t intensity=3;
  P.setIntensity(intensity);
	if(button_control(button)==1){
    intensity+=3;
    intensity %= 12;
    P.setIntensity(intensity);
  }
}


bool set_time(uint8_t button1, uint8_t button2){
  static uint8_t month,day,h,m;
  static bool change_date=1,print_datemsg=1,print_timemsg=1,set;
 
  if(print_datemsg==1){ // Print descriptive message to display
      display_text("SET DATE");
      print_datemsg=0;
    }

  if (button1==PUSH){ // Set time with button 1
    if(!change_date) // Set clock when date and time has been set
      set=1;
    change_date = !change_date;
    if(print_timemsg==1 && change_date ==0){ // Print descriptive message to display
      display_text("SET TIME");
      print_timemsg=0;
    }
    if(set){
      rtc.adjust(DateTime(2019, month, day, h, m, 0)); // Write time to RTC
      digitalWrite(BUZZER_PIN , HIGH);
      delay(100);
      digitalWrite(BUZZER_PIN , LOW);
      month=0;
      day=0;
      h=0;
      m=0;
      print_datemsg=1;
      print_timemsg=1;
      set =0;
      display_text("TIME SET!");
      return 1;
    }
  } 
  else if (button2==PUSH){  // Get input
    if (change_date){
      day++;
      day%=32;
      if(day == 0) day++;
    }
    else{
      m++;
      m %=60;
    }
  } 

  else if (button2==HOLD){ // Get input
    if (change_date){
      month++;
      month%=13;
      if (month == 0) month++;
    }
    else{
      h++;
      h %= 24;
    }
  }

  if(change_date){
    sprintf (char_top,"%02d",month); // Print date to display
    sprintf (char_bottom,"%02d",day);
  }
  else{
    sprintf (char_top,"%02d",h); // Print time display
    sprintf (char_bottom,"%02d",m);
  }
 return 0;
}

void print_hh_mm(){
  DateTime now = rtc.now();
  sprintf (char_top,"%02d",now.hour()); 
  sprintf (char_bottom,"%02d",now.minute());
}

void print_mm_ss(){
  DateTime now = rtc.now();
  sprintf (char_top,"%02d",now.minute()); 
  sprintf (char_bottom,"%02d",now.second());
}

void print_date(){
  DateTime now = rtc.now();
  sprintf (char_top,"%02d",now.month());
  sprintf (char_bottom,"%02d",now.day());
}

void default_clock(uint8_t button1, uint8_t button2){
  static uint8_t mode;
  static bool set_date;

  if(button1 == PUSH){
    mode++;
    mode %= 3;
  }
  else if (button2 == HOLD)
    set_date=1;
  
  if(set_date==1){
    if(set_time(button1,button2)==1){
      set_date = 0; // Exit function
      mode=0; // Reset mode
    }
  }
  else if (set_date==0){
    switch(mode){
      case 0: 
        print_hh_mm();
        break;
      case 1:
        print_mm_ss();
        break;
      case 2:
        print_date();
        break;
    }    
  }
}

void alarm(uint8_t button1, uint8_t button2, bool setmode){
  DateTime now = rtc.now();
  static uint8_t h = 0;
  static uint8_t m = 0;
  static bool start = 0, displaymsg=1;

  if (setmode == 1){
    sprintf (char_top,"%02d",h); // Print to display
    sprintf (char_bottom,"%02d",m);

    if (button1==PUSH) // Set / disable alarm with button 1
      start=!start;
    else if (button2==PUSH && start == 0){
      m++;
      m %= 60;
    }
      else if (button2==HOLD && start == 0){  // Get button input to set alarm
      h++;
      h %=24;
    } 
  } 
  
  if (start==1){
    if(setmode==1 && displaymsg==1){
      display_text("ALARM SET!");
      displaymsg=0;
    }
    if(setmode == 1)digitalWrite(BUZZER_PIN,HIGH); // CHANGE TO LED PIN LATER, AND REMOVE IF STATEMENT
    if (h== now.hour() && m == now.minute()){ // Check time
      print_hh_mm();
      display();
      while(button_control(BUTTON_PIN1)!=1){ // Sound buzzer until user input is recieved
        digitalWrite(BUZZER_PIN,HIGH);
      }
      start=0;
      displaymsg=1;
    } 
  }
}


void stopwatch (uint8_t button1, uint8_t button2){
  
  static uint32_t m,s;
  static bool stop=1;
  static unsigned long stop_secs=0;
  static unsigned long start_secs=0;
  static unsigned long diff;
  DateTime now = rtc.now();
  
  if (button2 == 1){
    if (!stop){
      stop=1;
      stop_secs=now.secondstime();
    }
    diff= stop_secs;
    m=0; // Update display variables
    s=0;
  }

  if (button1==1){ // Each time user presses stop/start button
    stop = !stop;
    if (stop) 
      stop_secs=now.secondstime(); // Store time when stopped

    if (!stop){
      start_secs=now.secondstime(); // Store time when started
      diff += start_secs-stop_secs; // Subtracts total stopped time from millis() to display correct time
    } 
  }
  if (!stop) {
    s = (now.secondstime()-diff);  // Second counter = Seconds since 1/1/2000 - accumulative difference
    m = (s/60);
    s %= 60;
    m %= 100; // Count up to 100 minutes
  }
    sprintf (char_top,"%02d",m); // Print to display
    sprintf (char_bottom,"%02d",s);
} 

bool timer_core(bool reset, unsigned long start_seconds, int h, int m){
  DateTime now = rtc.now();
  static int h_count=0;
  if (reset){ // Reset static variables
    h_count=0;
  }  

  for (uint8_t i=0;i<h_count;i++){
    m+=60;
    h--;
  }

  m -= (now.secondstime()-start_seconds); 
  if (m<0){
    h_count++;
    m=0; // To prevent display from briefly showing -1
  }

  sprintf (char_top,"%02d",h); // Print to display
  sprintf (char_bottom,"%02d",m);

  if (h==0 && m==0){ // Return 1 when timer reaches 0
    h_count=0;
    return 1;
  }
  return 0;
}

void set_timer(uint8_t button1, uint8_t button2){
  DateTime now = rtc.now();
  static uint8_t h = 0;
  static uint8_t m = 10;
  static bool start = 0;
  static bool reset=0;
  static unsigned long start_seconds=0;
  if (start==0)  
    sprintf (char_top,"%02d",h); // Print to display
    sprintf (char_bottom,"%02d",m);

  if (start==1){
    if (timer_core(reset,start_seconds,h,m)==1){ // Run core timer function while checking if timer has reached 00.00
      while(button_control(BUTTON_PIN1)!=1){
        display();
        digitalWrite(BUZZER_PIN,HIGH);
      }
      start=0;
    } 
    reset = 0;
  }

  if (button1==PUSH && start==0){
    start = 1;
    start_seconds=now.secondstime();
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

void select_mode(){
  int button1_state = button_control(BUTTON_PIN1);
  int button2_state = button_control(BUTTON_PIN2);
  static unsigned long buzz_millis;
  static int clock_mode=0;
  static bool switched_mode;

  if (button1_state== HOLD){ // Change mode
    clock_mode++;
    clock_mode %= nMODES;
    switched_mode=1;
    buzz_millis=millis();
    digitalWrite(BUZZER_PIN,HIGH);
  } 
  if(millis()-buzz_millis > 300) 
    digitalWrite(BUZZER_PIN,LOW);

  switch (clock_mode){
    case 0: 
      if(switched_mode==1){  // Display descriptive text when switching mode
        switched_mode=0;
        display_text("CLOCK"); 
      }
      alarm(button1_state,button2_state,0); // Use alarm in check mode
      default_clock(button1_state,button2_state);
      break;
    case 1: 
      if(switched_mode==1){  // Display descriptive text when switching mode
        switched_mode=0;
        display_text("ALARM"); 
      }
      alarm(button1_state,button2_state,1); // Use alarm in set mode.
      break;
    case 2:
      if(switched_mode==1){ // Display descriptive text when switching mode
        switched_mode=0;
        display_text("TIMER"); 
      }
      alarm(button1_state,button2_state,0); // Use alarm in check mode
      set_timer(button1_state,button2_state);
      break;
    case 3: 
      if(switched_mode==1){  // Display descriptive text when switching mode
        switched_mode=0;
        display_text("STOPWATCH"); 
      }
      alarm(button1_state,button2_state,0); // Use alarm in check mode
      stopwatch(button1_state,button2_state);
      break;
  }
}