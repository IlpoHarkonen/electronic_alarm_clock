////////////////////////////////////
///All DS3231 RTC clock functions///
////////////////////////////////////


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

