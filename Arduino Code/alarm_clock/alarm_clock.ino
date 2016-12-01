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
unsigned int check_interval = 100;//time between button press checks (ms)
unsigned int time_to_hold = 2000;//time that is needed to keep the buttons pushed to access menu (ms)
unsigned int intervals = time_to_hold / check_interval;


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
  pinMode(buzzer_pin, OUTPUT);
  //initialize the custom alarm icon
  initialize_alarm_symbol();
  //Start up the LCD display
  lcd.begin(16, 2);
  Wire.begin();
  //DS3231 RTC clock
  setDS3231time(1,37,13,1,2,3,16);
}




void loop() {
  //Print the correct state to LCD
  if (menu_state == basic) {
    //displays time and the possible alarm symbol
  }
  else if (menu_state == choose_set_time) {
    //displays the option to go into set time mode
  }
  else if (menu_state == choose_set_alarm) {
    //displays the option to go into set alarm mode
  }
  else if (menu_state == menu_set_time) {
    //displays the set time functional menu
  }
  else if (menu_state == menu_set_alarm) {
    //displays the set alarm functional menu
  }




  //Move between states
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
    //Set alarm and move to basic display MORE LINES
    menu_state = basic;
  }
  else if (menu_state == menu_set_time && buttons_held == true) {
    //Set time and move to basic display MORE LINES
    menu_state = basic;
  }



  //Add delay to the loop to avoid gaining too many consecutive user inputs from a single button push
  //and also determine how often the display is updated.
  delay(500);
}



void set_alarm() {
 
}


void play_alarm_sound() {
  tone(buzzer_pin,4000,500);
  //delay(500);
  noTone(buzzer_pin);
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

