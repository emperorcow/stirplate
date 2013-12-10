// include the library code:
#include <LiquidCrystal.h>

#define charsize 16
#define fanpin 3

volatile unsigned int rpmcount; //Number of interrupts from hall sensor
unsigned int rpms; //Current calculated RPM
unsigned long rpmtimeold; //Last time that we got an RPM reading

int amount = 0; //Read from the rotary potentiometer
int percent = 0; //Percentage that we want the fan at

LiquidCrystal lcd(7, 8, 9, 10, 11, 12); // initialize the library with the numbers of the interface pins
char buffer[16];

//Characters for bottom bar
byte p0[8] = {  0x10,  0x10,  0x10,  0x10,  0x10,  0x10,  0x10,  0x10 };
byte p1[8] = {  0x18,  0x18,  0x18,  0x18,  0x18,  0x18,  0x18,  0x18 };
byte p2[8] = {  0x1C,  0x1C,  0x1C,  0x1C,  0x1C,  0x1C,  0x1C,  0x1C };
byte p3[8] = {  0x1E,  0x1E,  0x1E,  0x1E,  0x1E,  0x1E,  0x1E,  0x1E };
byte p4[8] = {  0x1F,  0x1F,  0x1F,  0x1F,  0x1F,  0x1F,  0x1F,  0x1F };

void setup() {
  //Just in case we're using the serial console. 
  Serial.begin(9600);
  
  // set up the LCD's number of columns and rows: 
  lcd.begin(16, 2);
  
  lcd.print("Starting up...");
  
  //Setup characters
  lcd.createChar(0, p0);
  lcd.createChar(1, p1);
  lcd.createChar(2, p2);
  lcd.createChar(3, p3);
  lcd.createChar(4, p4);  

  //Set Hall sensor pin as an input.
  pinMode(fanpin, INPUT);
  
  //If the hall sensor goes high, do something
  attachInterrupt(0, rpm_interrupt, RISING);
  
  //Zero out our variables. 
  rpmcount = 0;
  rpms = 0;
  rpmtimeold = 0;  
}

void loop() {
  //Set cursor to the first line
  lcd.setCursor(0, 0);
  
  //Get the reading off of our pot.
  amount = analogRead(0);

  //The pot reads from 0-1023, we want a percent. 
  percent = map(amount, 0, 1023, 0, 100);

  //Our first line should be formatted and printed to the LCD. 
  //sprintf(buffer, "RPMs: %-5d %3d%%", rpm_read(), percent);
  sprintf(buffer, "%15d%%", percent);
  lcd.print(buffer);
  
  //Setup the second row to be a bar that shows the speed.
  bottombar(percent);
  
  Serial.print("Writing to pin: "); Serial.println(map(amount, 0, 1023, 0, 255));
  
  //PWM fun on the pin we have the fan running on!
  analogWrite(fanpin, map(amount, 0, 1023, 0, 255));
}

int a; //Tmp variable for our function, its here so we save memory
void bottombar(int value) {
  //Go to the second line of the LCD
  lcd.setCursor(0, 1);
   
  //Map the percent to the number of 5 pixel characters we have
  a = map(value, 0, 100, 0, charsize * 5);
  
  //Loop through each character
  for(int col = 0; col <= charsize * 5; col += 5) {
    //For each character, figure out how many columsn we're supposed to 
    // have and either print a full block, or just print the appropriate 
    // row from the custom byte-maps above. Or blank is cool too.
    if (a - col == 1) {
      lcd.print((char)0);
    } else if (a - col == 2) {
      lcd.print((char)1);
    } else if (a - col == 3) {
      lcd.print((char)2);
    } else if (a - col == 4) {
      lcd.print((char)3);
    } else if (a - col >= 5) {
      lcd.print((char)4);
    } else {
      lcd.print(' '); 
    }
  }
}

unsigned int rpm_read() {
  if (rpmcount >= 10) {   
    //Take the number of seconds that have elapsed since our last update and
    // extrapolate out to 60 seconds. We get two hits every revolution, so we take 30 seconds.
    rpms = 30000 / (millis() - rpmtimeold) * rpmcount; //60000 ms = 60 seconds
    rpmtimeold = millis();
    rpmcount = 0;   
  }

  return rpms;  
}

//Interrupt function, gets called every time our hall sensor pin goes high. 
//Doesn't seem to be working right now and I just want to get this done, will deal with it later.
void rpm_interrupt() {
  rpmcount++;
}


