///////////////////////////////
///All LCD display functions///
///////////////////////////////

int time_row = 1;
int hour_column = 8;
int minute_column = hour_column + 3;

//Bit art for the custom made alarm symbol. The symbol is in four pieces,
//as it takes 4 digits of display space on the LCD screen.
byte left_upper[8] = {B00000,B00000,B00000,B00010,B00110,B11110,B11110,B11110,};
byte left_lower[8] = {B11110,B11110,B11110,B00110,B00010,B00000,B00000,B00000,};
byte right_upper[8] = {B00000,B00010,B01001,B00101,B10101,B10101,B10101,B10101,};
byte right_lower[8] = {B10101,B10101,B10101,B10101,B00101,B01001,B00010,B00000,};



void initialize_alarm_symbol() {
  //Adds the created alarm symbol pieces to the LCD character set.
  lcd.createChar(0, left_upper);
  lcd.createChar(1, right_upper);
  lcd.createChar(2, left_lower);
  lcd.createChar(3, right_lower);
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



void print_hour(int optional_modifier = 0) {
  //Prints hours and a ':' -symbol after that in a format: 'HH:'
  //LCD cursor is left where printing ends.
  //The optional input modifier is an integer which is added to the current hour.
  lcd.setCursor(hour_column, time_row);
  if(current_hour <10) {
    lcd.print('0');
  }
  lcd.print(current_hour + optional_modifier);
  lcd.print(':');
}



void print_alarm_hour(int optional_modifier = 0) {
  //Prints hours and a ':' -symbol after that in a format: 'HH:'
  //LCD cursor is left where printing ends.
  //The optional input modifier is an integer which is added to the hour.
  lcd.setCursor(hour_column, time_row);
  if(alarm_hour <10) {
    lcd.print('0');
  }
  lcd.print(alarm_hour + optional_modifier);
  lcd.print(':');
}



void print_minute(int optional_modifier = 0) {
  //Prints a ':' -symbol and minutes after that in a format: ':MM'
  //LCD cursor is left where printing ends.
  //The optional input modifier is an integer which is added to the current minute.
  lcd.setCursor(minute_column - 1, time_row);
  lcd.print(':');
  if(current_minute <10) {
    lcd.print('0');
  }
  lcd.print(current_minute + optional_modifier);
}



void print_alarm_minute(int optional_modifier = 0) {
  //Prints a ':' -symbol and minutes after that in a format: ':MM'
  //LCD cursor is left where printing ends.
  //The optional input modifier is an integer which is added to the minute.
  lcd.setCursor(minute_column - 1, time_row);
  lcd.print(':');
  if(alarm_minute <10) {
    lcd.print('0');
  }
  lcd.print(alarm_minute + optional_modifier);
}



void display_basic() {
  //Print out current time in HH:MM format. Also print the alarm symbol if alarm is set.
  lcd.clear();
  if (alarm_on == true) {
    display_alarm_icon();
  }
  print_hour();
  print_minute();
}



void display_choose_set_time() {
  //Prints the option of proceeding into set time function
  lcd.clear();
  lcd.setCursor(2,0);
  lcd.print("Set time?");
}




void display_choose_set_alarm() {
  //Prints the option of proceeding into set alarm function
  lcd.clear();
  lcd.setCursor(1,0);
  lcd.print("Set alarm?");
}




void display_menu_set_time() {
  //Display the set time menu in the following format:    Set time:
  //                                                        HH:MM

  //Print the correct headline
  lcd.clear();
  lcd.setCursor(2,0);
  lcd.print("Set time:");

  //If editing hours, minutes are always printed and vice versa.
  if (hours_chosen == true) {
    print_minute(time_modifier_minute);
  }
  else {
    print_hour(time_modifier_hour);
  }

  //IF EDITING HOURS
  if (blinking == true && hours_chosen == true) {
    blinking = false;
    print_hour(time_modifier_hour);
  }

  //IF EDITING MINUTES
  if (blinking == true && hours_chosen == false) {
    blinking = false;
    print_minute(time_modifier_minute);
  }

  //If the minutes or hours were not printed, we make it do so the next time this function is ran.
  if (blinking == false) {
    blinking = true;
  }
}



void display_menu_set_alarm() {
  //Display the set alarm menu in the following format:    Set alarm:
  //                                                         HH:MM
  //The set alarm menu remembers and displays the last
  //alarm time as default.
  
  //Print the correct headline
  lcd.clear();
  lcd.setCursor(2,0);
  lcd.print("Set alarm:");

  //If editing hours, minutes are always printed and vice versa.
  if (hours_chosen == true) {
    print_alarm_minute(time_modifier_minute);
  }
  else {
    print_alarm_hour(time_modifier_hour);
  }
  
  //IF EDITING HOURS
  if (blinking == true && hours_chosen == true) {
    blinking = false;
    print_alarm_hour(time_modifier_hour);
  }

  //IF EDITING MINUTES
  if (blinking == true && hours_chosen == false) {
    blinking = false;
    print_alarm_minute(time_modifier_minute);
  }

  //If the minutes or hours were not printed, we make it do so the next time this function is ran.
  if (blinking == false) {
    blinking = true;
  }
}
