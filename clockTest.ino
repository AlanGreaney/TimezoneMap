#include <TimeLib.h> //https://github.com/PaulStoffregen/Time
#include <TM1637Display.h> //https://github.com/avishorp/TM1637/
#include <Arduino.h>

//initializing the display boards
//Each display has 2 power and 2 data pins. the (x, y) are the data pins named CLK and DIO respectively, and where they should go on the board
TM1637Display displayUTC(8, 9); //CLK, DIO
TM1637Display displayPST(2, 3); //CLK, DIO
TM1637Display displayEST(4, 5); //CLK, DIO
TM1637Display displayJPN(6, 7); //CLK, DIO

 //pin numbers
const int brightnessButton = 10;
const int timeButton = 11; 

//starting time around noon PST to help get close, then use buttons to get exact. only really cares about 24hr clock, but could build in automatic switch to DST based on date. a more advanced (and expensive) board with wifi/bluetooth could have automatic detection. 
//starting time is in UTC
long startingTime = 1670908246964 / 1000;

//offsets from UTC
int pstOffset = -8;
int estOffset = -5;
int jpnOffset = 9;

int brightness = 0; //dynamic brightness variable
int timeModifier = 1000; //allow the clock to run faster (or slower) for testing/debug
time_t lastWrittenMinute = -1; //used to only send write commands to displays when the minute has changed to minimize power usage
int dstOffset = 0; //whether DST is enabled or not

void setup()
{
  Serial.begin(9600);
  
  setBrightness();

  //setup buttons
  pinMode(brightnessButton, INPUT);
  pinMode(timeButton, INPUT);

  delay(10);
  
  //the following manually creates data arrays that define which of the 8 segment display should be on. Used to show the timezones for 10 seconds before switching to clock mode.
  //for example, 'UTC' is displayed in the 4 slots using a rectangular U, a T using two spaces, then a rectangular C

  uint8_t dataUTC[] = {0b00111110, 0b00000001, 0b00110001, 0b00111001};
  displayUTC.setSegments(dataUTC);

  uint8_t dataPST[] = {0b01110011, 0b01101101, 0b00000001, 0b00110001};
  displayPST.setSegments(dataPST);

  uint8_t dataEST[] = {0b01111001, 0b01101101, 0b00000001, 0b00110001};
  displayEST.setSegments(dataEST);

  uint8_t dataJPN[] = {0b00000000, 0b00001110, 0b01110011, 0b01010100};
  displayJPN.setSegments(dataJPN);
  
  delay(10000);
}

//sets brightness using the current brightness value on all displays
void setBrightness()
{
  displayUTC.setBrightness(brightness);
  displayPST.setBrightness(brightness);
  displayEST.setBrightness(brightness);
  displayJPN.setBrightness(brightness);

}

//converts our time from a seperate int HH and int MM format into a '####' string format for display using the display API. adds leading 0s to the minutes. API auto-adds it to the hours.
int createTimeInt(int hours, int minutes)
{
  String minuteString = String(minutes);
  if(minutes < 10)
  {
    minuteString = "0" + minuteString;
  }

  String timeStr = String(hours) + "" + minuteString;
  return timeStr.toInt();
}


void loop()
{
  bool shouldUpdate = false;
  bool longPressDelay = false;

  if(digitalRead(brightnessButton) == HIGH) //if the brightness button is pressed,
  {
    //check how long the button was held for. Basically, stay in a while loop until its unpressed, or held for half a second.
    long started = millis();
    while(digitalRead(brightnessButton) == HIGH && millis() - started < 500)
    {
      delay(10);
    }

    //if it was held for a long time:
    if(millis() - started >= 500)
    {
      longPressDelay = true;
      if(digitalRead(timeButton) == HIGH)
      {
        timeModifier = 2; //makes the time run REALLY fast if both buttons are held
      }
      //normally, if they long hold the brightness button, we instead toggle DST on or off.
      else if(dstOffset == 0)
      {
        dstOffset = -1;
      }
      else
      {
        dstOffset = 0;
      }
    }
    else //if it was held for a short time, cycle the brightness
    {
      brightness += 1; //the library automatically does a %7 or something so no need to loop over to 0 ourselves. im not too worried about someone pressing the button MAX_INT times. i think it'd break first.
      setBrightness();
    }
    
    shouldUpdate = true;
  }

  if(digitalRead(timeButton) == HIGH)
  {
    long started = millis();
    while(digitalRead(timeButton) == HIGH && millis() - started < 500)
    {
      delay(10);
    }
    if(millis() - started >= 500)
    {
      longPressDelay = true;
      if(digitalRead(brightnessButton) == HIGH)
      {
        timeModifier = 2; //party mode
      }
      else
      {
        startingTime += (60*60); //long press adds one hour
      }
    }
    else
    {
      startingTime += 60; //short press adds a minute
    }
    
    shouldUpdate = true;
  }

  //create a time variable using current time. millis() is how long the arduino has been powered on.
  //below will also create the same variables for EST/PST/etc, but we only do that work and send it if the minute() is different from what was last sent to the displays.
  //minute() and hour() built in functions from the time_t
  time_t utcTime = (startingTime) + (millis()/timeModifier); //timelib.h is in seconds

  if(minute(utcTime) != lastWrittenMinute || shouldUpdate)
  {
    lastWrittenMinute = minute(utcTime);

    time_t pstTime = (startingTime) + (millis()/timeModifier) + ((pstOffset + dstOffset)*60*60);
    time_t estTime = (startingTime) + (millis()/timeModifier) + ((estOffset + dstOffset)*60*60);
    time_t jpnTime = (startingTime) + (millis()/timeModifier) + (jpnOffset*60*60);
    displayUTC.showNumberDecEx(createTimeInt(hour(utcTime), minute(utcTime)), 0b01000000, true); //number to display, mode "00:00 (0b01000000)" (show with colon in middle), leading zeroes = on
    displayPST.showNumberDecEx(createTimeInt(hour(pstTime), minute(pstTime)), 0b01000000, true);
    displayEST.showNumberDecEx(createTimeInt(hour(estTime), minute(estTime)), 0b01000000, true);
    displayJPN.showNumberDecEx(createTimeInt(hour(jpnTime), minute(jpnTime)), 0b01000000, true);
  }

  //if the button was held down for over 500ms, we want to give time before checking everything again for the user to release the button
  //example, if they were holding down the time button to increase the hours, this gives time to remove the finger when it finally hits the right hour before moving onto the next hour.
  if(longPressDelay)
  {
    delay(500);
  }
  else
  {
    delay(10);
  }
  
}
