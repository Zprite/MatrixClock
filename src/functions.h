#include <Arduino.h>
#include <global.h>
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include <string.h>

void display(){
  static uint8_t  curZone = 0;
  P.displayZoneText (1,char_top, PA_CENTER, SPEED_TIME, PAUSE_TIME, PA_PRINT ,PA_NO_EFFECT);
  P.displayZoneText (0,char_bottom, PA_CENTER, SPEED_TIME, PAUSE_TIME, PA_PRINT ,PA_NO_EFFECT);
  while (!P.getZoneStatus (curZone))
    P.displayAnimate(); // Refresh display
}

void setup_rtc(){

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  if (rtc.lostPower()) {
     Serial.println("RTC lost power, lets set the time!");
    // following line sets the RTC to the date & time this sketch was compiled
     //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }
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


bool set_time(int button1, int button2){
  static uint8_t month,day,h,m;
  static bool change_date=1,set;
  if (button1==PUSH){ // Set time with button 1
    if(!change_date) // Set clock when date and time has been set
      set=1;
    change_date = !change_date;
    if(set){
      rtc.adjust(DateTime(2019, day, month, h, m, 0)); // Write time to RTC
      digitalWrite(BUZZER_PIN , HIGH);
      delay(100);
      digitalWrite(BUZZER_PIN , LOW);
      month=0;
      day=0;
      h=0;
      m=0;
      return 1;
    }
  } 
  else if (button2==HOLD){  // Get input
    if (change_date){
      day++;
      day%=32;
      if(day==0) day++;
    }
    else{
      h++;
      h %= 24;
    }
  } 

  else if (button2==PUSH){ // Get input
    if (change_date){
      month++;
      month%=13;
      if (month == 0) month++;
    }
    else{
      m++;
      m %= 60;
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
  sprintf (char_top,"%02d",now.day());
  sprintf (char_bottom,"%02d",now.month());
}

void stopwatch (int button1, int button2){
  
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

void set_timer(int button1, int button2){
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

void alarm(int button1, int button2,bool set_mode){
  DateTime now = rtc.now();
  static uint8_t h = 0;
  static uint8_t m = 0;
  static bool start = 0;

  if (set_mode == 1){
    sprintf (char_top,"%02d",h); // Print to display
    sprintf (char_bottom,"%02d",m);

    if (button1==PUSH) // Set / disable alarm with button 1
      start=!start;
    else if (button2==PUSH && start == 0){  // Get button input to set alarm
      h++;
      h %=24;
    } 
    else if (button2==HOLD && start == 0){
      m++;
      m %= 60;
    }
    
  } 
  
  if (start==1){
    if(set_mode == 1)digitalWrite(BUZZER_PIN,HIGH); // CHANGE TO LED PIN LATER!!!!!!!!
    if (h== now.hour() && m == now.minute()){ // Check time
      print_hh_mm();
      display();
      while(button_control(BUTTON_PIN1)!=1){ // Sound buzzer until user input is recieved
        digitalWrite(BUZZER_PIN,HIGH);
      }
      start=0;
    } 
  }
}

void default_clock(int button1, int button2){
  static uint8_t mode;
  static bool set_date = 0;
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


void select_mode(){
  DateTime now = rtc.now();
  int button1_state = button_control(BUTTON_PIN1);
  int button2_state = button_control(BUTTON_PIN2);
  static unsigned long buzz_millis;
  static int clock_mode=0;

  if (button1_state== HOLD){
    clock_mode++;
    clock_mode %= nMODES;
    buzz_millis=millis();
    digitalWrite(BUZZER_PIN,HIGH);
  } 
  if(millis()-buzz_millis > 300)  
    digitalWrite(BUZZER_PIN,LOW);

  switch (clock_mode){
    case 0: 
      alarm(button1_state,button2_state,0); // Check alarm
      default_clock(button1_state,button2_state);
      break;
    case 1: 
      alarm(button1_state,button2_state,1); // Use alarm in set mode.
      break;
    case 2:
      set_timer(button1_state,button2_state);
      break;
    case 3:
      alarm(button1_state,button2_state,0); // Check alarm
      stopwatch(button1_state,button2_state);
      break;
  }
}