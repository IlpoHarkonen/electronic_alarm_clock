#include <LiquidCrystal.h>
#include <Wire.h>
#define DS3231_I2C_ADDRESS 0x68


LiquidCrystal lcd(12, 11, 5, 4, 3, 2);


//Pins for the left and right buttons and their pressed boolean values and also the buzzer pin
int buzzer_pin = 0;
int LB_pin = 0;//digital pin for the left button
int RB_pin = 0;//digital pin for the right button
boolean LB_pressed = false;
boolean RB_pressed = false;
unsigned int check_interval = 100;//Time between button press checks (ms)
unsigned int time_to_hold = 2000;//Time that is needed to keep the buttons pushed to access menu (ms)
unsigned int intervals = time_to_hold / check_interval;
int max_idle_time = 60000;//Maximum time the user can be in a menu, after which the clock returns back to the basic screen (ms)
int max_idle_var = max_idle_time;
int loop_interval = 500;//The time waited between loops. Helps reduce unnecessary calculation and recognize single user inputs. (ms)


//Time values
//RTC last reported time values
int current_hour = 0;
int current_minute = 0;
//User-inputted differences used in setting time or alarm
int time_modifier_hour = 0;
int time_modifier_minute = 0;


//Booleans for blinking hours or minutes while setting them.
boolean alarm_on = false;
boolean hours_chosen = false;
boolean blinking = true;

int alarm_hour = 0;
int alarm_minute = 0;


//Different states the clock can be in. Starts out in basic.
enum state {
  basic,//displays the time and may be readied to wake up
  choose_set_time,//shows the option to go into set_time function
  choose_set_alarm,//shows the option to go into set alarm function
  menu_set_time,//lets the user set time
  menu_set_alarm//lets the user set alarm
};

enum state menu_state = basic;



void setup() {
  Serial.begin(9600);
  //Initialize the push buttons and the buzzer
  pinMode(LB_pin, INPUT);
  pinMode(RB_pin, INPUT);
  //pinMode(buzzer_pin, OUTPUT);
  //initialize the custom alarm icon
  initialize_alarm_symbol();
  //Start up the LCD display
  lcd.begin(16, 2);
  Wire.begin();
  //DS3231 RTC clock
  setDS3231time(1,37,13,1,2,3,16);
}




void loop() {
  //Fetch time variables from DS3231
  update_time();
  
  //1. Print the correct state to LCD
  //2. Check if both buttons are held or one button is pushed to move between states accordingly
  //3. Check for individual button presses that indicate other user input and act accordingly
  //4. Check if alarm sound should be played or stopped from playing
  //5. If no keypresses have occured in a sufficient time, return to the basic clock state
  
  //1. Print the correct state to LCD
  if (menu_state == basic) {
    //displays time and the possible alarm symbol
    display_basic();
  }
  else if (menu_state == choose_set_time) {
    //displays the option to go into set time mode
    display_choose_set_time();
  }
  else if (menu_state == choose_set_alarm) {
    //displays the option to go into set alarm mode
    display_choose_set_alarm();
  }
  else if (menu_state == menu_set_time) {
    //displays the set time functional menu
    display_menu_set_time();
  }
  else if (menu_state == menu_set_alarm) {
    //displays the set alarm functional menu
    display_menu_set_alarm();
  }


  //2. Move between states
  boolean buttons_held = check_if_buttons_held();
  if (menu_state == basic && buttons_held == true) {
    //Move from default display to menu
    menu_state = choose_set_alarm;
  }
  else if (menu_state == choose_set_alarm && (check_if_pushed(LB_pin) == true | check_if_pushed(RB_pin) == true)) {
    //Move between menu options
    menu_state = choose_set_time;
  }
  else if (menu_state == choose_set_time && (check_if_pushed(LB_pin) == true | check_if_pushed(RB_pin) == true)) {
    //Move between menu options
    menu_state = choose_set_alarm;
  }
  else if (menu_state == choose_set_alarm && buttons_held == true) {
    //Go to set alarm functional menu
    menu_state = menu_set_alarm;
  }
  else if (menu_state == choose_set_time && buttons_held == true) {
    //Go to set time functional menu
    menu_state = menu_set_time;
  }
  else if (menu_state == menu_set_alarm && buttons_held == true) {
    //Set alarm and move to basic display
    menu_state = basic;
    
    //Update alarm time and switch alarm on
    set_alarm();
    
    //Clear temporal modifiers
    time_modifier_hour = 0;
    time_modifier_minute = 0;
    hours_chosen = false;
  }
  else if (menu_state == menu_set_time && buttons_held == true) {
    //Set time and move to basic display
    menu_state = basic;

    //Update DS3231 clock internal time
    set_time(current_hour + time_modifier_hour, current_minute + time_modifier_minute);

    
    //Clear the temporal modifiers
    time_modifier_hour = 0;
    time_modifier_minute = 0;
    hours_chosen = false;
  }


  //3. Check for individual keypresses and act accordingly
  //This part concerns only the set_alarm and set_time menus.
  LB_pressed = check_if_pushed(LB_pin);
  RB_pressed = check_if_pushed(RB_pin);
  
  //Changing the alarm or clock time temporal variables
  if (menu_state == menu_set_alarm | menu_state == menu_set_time) {
    //Increase or decrese hours
    if (hours_chosen == true) {
      if (LB_pressed == true) {
        time_modifier_hour -= 1;
      }
      else if (RB_pressed == true) {
        time_modifier_hour += 1;
      }
    }
    
    //Increase or decrease minutes
    else {
      if (LB_pressed == true) {
        time_modifier_minute -= 1;
      }
      else if (RB_pressed == true) {
        time_modifier_minute += 1;
      }
    }
  }

  //4. Check if alarm sound should be played or set off
  if (current_hour == alarm_hour && current_minute == alarm_minute && alarm_on = true) {
    play_alarm_sound();
    if (LB_pressed == true | RB_pressed == true) {
      alarm_on = false;
    }
  }

  //5. Return to basic clock screen if the user has been idle for too long
  // Also resets all temporal modifiers and turns off the alarm.
  if (menu_state != basic && max_idle_var > 0) {
    max_idle_time -= loop_interval;
  }
  if (max_idle_time < 0) {
    max_idle_var = max_idle_time;
    menu_state = basic;
    time_modifier_hour = 0;
    time_modifier_minute = 0;
    alarm_on = false; 
  }
  


  //Add delay to the loop to avoid gaining too many consecutive user inputs from a single button push
  //and also determine how often the display is updated.
  delay(loop_interval);
}



void set_alarm() {
  //Updates the alarm variables and brings the to 0-23h and 0-59m. Also switches the alarm on 
  alarm_on = true;
  alarm_hour = alarm_hour + time_modifier_hour;
  alarm_minute = alarm_minute + time_modifier_minute;

  while (alarm_hour > 23) {
    alarm_hour = alarm_hour - 24;
  }
  while (alarm_hour < 0) {
    alarm_hour = alarm_hour - 24;
  }
  while (alarm_minute > 59) {
    alarm_minute = alarm_minute - 60; 
  }
  while (alarm_minute < 0) {
    alarm_minute = alarm_minute + 60;
  }
}


void play_alarm_sound() {
  //Plays the alarm sound k times
  int k = 2;
  while (k > 0) {
    tone(buzzer_pin,4000,500);
    noTone(buzzer_pin);
    k -= 1;
  }
}



boolean check_if_buttons_held() {
  //We check if buttons are pressed for the specified time to hold and return the corresponding boolean value
  LB_pressed = check_if_pushed(LB_pin);
  RB_pressed = check_if_pushed(RB_pin);
  unsigned int i = 0;
  if (LB_pressed == true && RB_pressed == true) {
    while (i < intervals && LB_pressed == true && RB_pressed == true) {
      delay(check_interval);
      LB_pressed = check_if_pushed(LB_pin);
      RB_pressed = check_if_pushed(RB_pin);
      i += 1;
    }
  }
  //If button presses are confirmed we move to the menu
  if (i == intervals) {
    return true;
  }
  else {
    return false;
  }
}




boolean check_if_pushed(int button_pin) {
  //check if a button using a given pin is pressed or not and returns the corresponding boolean value
  int button_state = digitalRead(button_pin);
  if (button_state == 1) {
    return true;
  }
  else {
    return false;
  }
}

