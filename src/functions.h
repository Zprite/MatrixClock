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
    Serial.println("M: ");
    Serial.println(m);
    Serial.println("S: ");
    Serial.println(s);
    sprintf (char_top,"%02d",m); // Print to display
    sprintf (char_bottom,"%02d",s);
}

void select_mode(){

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
      Serial.println("Mode: HH MM");
      stopwatch(button1_state,button2_state);
     // print_hh_mm();
      break;
    case 1: 
      Serial.println("Mode: MM SS");
      print_mm_ss();
      break;
    case 2:
      Serial.println("Mode: DATE");
      print_date();
      break;
  }
}