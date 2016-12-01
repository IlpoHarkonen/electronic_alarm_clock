#include <LiquidCrystal.h>
#include <Wire.h>
#define DS3231_I2C_ADDRESS 0x68


LiquidCrystal lcd(12, 11, 5, 4, 3, 2);


//pins for the left and right buttons and their pressed boolean values and also the buzzer pin
int buzzer_pin = 0;
int LB_pin = 0;//digital pin for the left button
int RB_pin = 0;//digital pin for the right button
boolean LB_pressed = false;
boolean RB_pressed = false;
unsigned int check_interval = 100;//time between button press checks (ms)
unsigned int time_to_hold = 2000;//time that is needed to keep the buttons pushed to access menu (ms)
unsigned int intervals = time_to_hold / check_interval;


//time values
int current_hour = 0;//rtc last reported hour
int current_minute = 0;//rtc last reported minute
int time_modifier_hour = 0;
int time_modifier_minute = 0;


boolean alarm_on = false;
boolean hours_chosen = false;
boolean minutes_chosen = false;
boolean blinking = true;




//Bit art for the custom made alarm symbol. The symbol is in four pieces,
//as it takes 4 digits of display space on the LCD screen.
byte left_upper[8] = {B00000,B00000,B00000,B00010,B00110,B11110,B11110,B11110,};
byte left_lower[8] = {B11110,B11110,B11110,B00110,B00010,B00000,B00000,B00000,};
byte right_upper[8] = {B00000,B00010,B01001,B00101,B10101,B10101,B10101,B10101,};
byte right_lower[8] = {B10101,B10101,B10101,B10101,B00101,B01001,B00010,B00000,};




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
  lcd.createChar(0, left_upper);
  lcd.createChar(1, right_upper);
  lcd.createChar(2, left_lower);
  lcd.createChar(3, right_lower);
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




  delay(500);//add delay to the loop to avoid gaining too many consecutive user inputs from a single button push
}




void display_basic() {
  //Print out current time in HH:MM format. Also print the alarm symbol if alarm is set.
  lcd.clear();
  if (alarm_on == true) {display_alarm_icon();}
  
  //Place the cursor to the middle of the top row and print out the current time.
  lcd.setCursor(8,0);
  if(current_hour <10) {lcd.print('0');}
  lcd.print(current_hour);
  if(current_minute <10) {lcd.print('0');}
  lcd.print(current_minute);
}

//FIX THE MINUTES PART IN ACCORDING TO THE HOURS PART
void display_menu_set_time(){
  //Display the set time menu
  lcd.clear();
  lcd.setCursor(2,0);
  lcd.print('Set time:');

  //IF EDITING HOURS
  //Check if editing hours and print blinking hours
  //Hours. Blinking print shown.
  if (blinking == true && hours_chosen == true){
    blinking = false;
    lcd.setCursor(3,1);
    if((current_hour + time_modifier_hour) <10){
      lcd.print('0');
    }
    lcd.print(current_hour + time_modifier_hour);
  }
  
  //Hours. Blinking print hidden.
  if (blinking == false && hours_chosen == true){
    blinking = true;
    lcd.setCursor(5,1);
    lcd.print(':');
  }
  
  //Minutes
  if (hours_chosen == true) {
    if(current_minute + time_modifier_minute < 10){
      lcd.print('0');
    }
    lcd.print(current_minute+time_modifier_minute);
  }

  
  //IF EDITING MINUTES
  //Check if editing minutes and print blinking minutes
  //Blinking print shown
  if (blinking == true && minutes_chosen == true){
    blinking = false;
    
    //Hours
    lcd.setCursor(3,1);
    if(current_hour + time_modifier_hour <10){
      lcd.print('0');
    }
    lcd.print(current_hour + time_modifier_hour);
    lcd.print(':');
    
    //Minutes
    if(current_minute + time_modifier_minute < 10){
      lcd.print('0');
    }
    lcd.print(current_minute+time_modifier_minute);
  }
  else{
    blinking = true;
    lcd.setCursor(3,1);
    if(current_hour + time_modifier_hour <10){
      lcd.print('0');
    }
    lcd.print(current_hour + time_modifier_hour);
    lcd.print(':');
  }
}




void set_alarm() {
 
}


void play_alarm_sound() {
  tone(buzzer_pin,4000,500);
  //delay(500);
  noTone(buzzer_pin);
}




void display_alarm_icon() {
  //Print the custom alarm icon to the rightmost side of the screen.
  lcd.setCursor(15,0);
  lcd.write(byte(0));
  lcd.setCursor(16,0);
  lcd.write(byte(1));
  lcd.setCursor(15,1);
  lcd.write(byte(2));
  lcd.setCursor(16,1);
  lcd.write(byte(3));
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




//////////////////////////////////////////////////
///All DS3231 RTC clock functions defined below///
//////////////////////////////////////////////////


// Convert normal decimal numbers to binary coded decimal
byte decToBcd(byte val) {
  return( (val/10*16) + (val%10) );
}


// Convert binary coded decimal to normal decimal numbers
byte bcdToDec(byte val) {
  return( (val/16*10) + (val%16) );
}


void setDS3231time(byte second, byte minute, byte hour, byte dayOfWeek, byte
dayOfMonth, byte month, byte year) {
  //Sets time and date data to DS3231 RTC clock
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0); // set next input to start at the seconds register
  Wire.write(decToBcd(second)); // set seconds
  Wire.write(decToBcd(minute)); // set minutes
  Wire.write(decToBcd(hour)); // set hours
  Wire.write(decToBcd(dayOfWeek)); // set day of week (1=Sunday, 7=Saturday)
  Wire.write(decToBcd(dayOfMonth)); // set date (1 to 31)
  Wire.write(decToBcd(month)); // set month
  Wire.write(decToBcd(year)); // set year (0 to 99)
  Wire.endTransmission();
}


void readDS3231time(byte *second, byte *minute, byte *hour, byte *dayOfWeek,
byte *dayOfMonth, byte *month, byte *year) {
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0); // set DS3231 register pointer to 00h
  Wire.endTransmission();
  Wire.requestFrom(DS3231_I2C_ADDRESS, 7);
  // request seven bytes of data from DS3231 starting from register 00h
  *second = bcdToDec(Wire.read() & 0x7f);
  *minute = bcdToDec(Wire.read());
  *hour = bcdToDec(Wire.read() & 0x3f);
  *dayOfWeek = bcdToDec(Wire.read());
  *dayOfMonth = bcdToDec(Wire.read());
  *month = bcdToDec(Wire.read());
  *year = bcdToDec(Wire.read());
}


void updateTime() {
  //Reads the time from DS3231 and updates the global variables accordingly
  byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
  readDS3231time(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month, &year);
  current_hour = (int) hour;
  current_minute = (int) minute;
}






void set_time(int hours, int minutes) {
  //Changes DS3231 internal time to match the given parameters.
  byte h_byte = (byte) hours;
  byte m_byte = (byte) minutes;
  
  h_byte = decToBcd(h_byte);
  m_byte = decToBcd(m_byte);
  setDS3231time(1, m_byte, h_byte, 1, 2, 3, 16);
}


void change_hour() {
 
}




void change_minute() {
 
}

