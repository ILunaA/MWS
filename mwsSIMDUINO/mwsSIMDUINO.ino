//DEBUG
//#include <MemoryFree.h>
#include<string.h>
#include <Wire.h> //I2C needed for sensors
//https://github.com/sparkfun/SparkFun_MPL3115A2_Breakout_Arduino_Library
#include "MPL3115A2.h" //Pressure sensor
//https://github.com/sparkfun/SparkFun_HTU21D_Breakout_Arduino_Library
#include "HTU21D.h" //Humidity sensor
#include <SoftwareSerial.h> //Needed for GPS
//http://arduiniana.org/libraries/tinygpsplus/
#include <TinyGPS++.h> //GPS parsing


#define DEBUG true

//For SIMDUINO GPS/GSM from Simduino
static const int RXPin = 7, TXPin = 8; //GPS is attached to pin 7(TX from GPS) and pin 8(RX into GPS)
SoftwareSerial ss(RXPin, TXPin);
//HW serial is RXPin=1 TXPin=0 (to check order) can high baud rate.

MPL3115A2 myPressure; //Create an instance of the pressure sensor
HTU21D myHumidity; //Create an instance of the humidity sensor

//Hardware pin definitions
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// digital I/O pins
const byte RAIN = 2;
const byte WSPEED = 3;
const byte SIM808 = 10;

// analog I/O pins
const byte WDIR = A0;
const byte LIGHT = A1;
const byte BATT = A2;
const byte REFERENCE_3V3 = A3;
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

//Global Variables
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
long lastSecond; //The millis counter to see when a second rolls by
long loopMSecond; //time it took to loop once in millis
byte seconds; //When it hits 60, increase the current minute
byte seconds_2m; //Keeps track of the "wind speed/dir avg" over last 2 minutes array of data
byte minutes; //Keeps track of where we are in various arrays of data
byte minutes_10m; //Keeps track of where we are in wind gust/dir over last 10 minutes array of data

long lastWindCheck = 0;
volatile long lastWindIRQ = 0;
volatile byte windClicks = 0;

//We need to keep track of the following variables:
//Wind speed/dir each update (no storage)
//Wind gust/dir over the day (no storage)
//Wind speed/dir, avg over 2 minutes (store 1 per second)
//Wind gust/dir over last 10 minutes (store 1 per minute)
//Rain over the past hour (store 1 per minute)
//Total rain over date (store one per day)

byte windspdavg[120]; //120 bytes to keep track of 2 minute average
int winddiravg[120]; //120 ints to keep track of 2 minute average
float windgust_10m[10]; //10 floats to keep track of 10 minute max
int windgustdirection_10m[10]; //10 ints to keep track of 10 minute max
volatile float rainHour[60]; //60 floating numbers to keep track of 60 minutes of rain

//These are all the weather values that wunderground expects:
int winddir = 0; // [0-360 instantaneous wind direction]
float windspeedms = 0; // [mph instantaneous wind speed]
float windgustms = 0; // [mph current wind gust, using software specific time period]
int windgustdir = 0; // [0-360 using software specific time period]
float windspdms_avg2m = 0; // [mph 2 minute average wind speed mph]
int winddir_avg2m = 0; // [0-360 2 minute average wind direction]
float windgustms_10m = 0; // [mph past 10 minutes wind gust mph ]
int windgustdir_10m = 0; // [0-360 past 10 minutes wind gust direction]
float humidity = 0; // [%]
float tempf = 0; // [temperature F]
float rainin = 0; // [rain inches over the past hour)] -- the accumulated rainfall in the past 60 min
volatile float dailyrainin = 0.0; // [rain inches so far today in local time]
//float baromin = 30.03;// [barom in] - It's hard to calculate baromin locally, do this in the agent
float pressure = 0;
//float dewptf; // [dewpoint F] - It's hard to calculate dewpoint locally, do this in the agent

float batt_lvl = 11.8; //[analog value from 0 to 1023]
float light_lvl = 455; //[analog value from 0 to 1023]
//Variables used for GPS
//unsigned long age;
int gpsyear = 2000;
int gpsmonth, gpsday, gpshour, gpsminute, gpssecond, gpshundredths;
float gpslng = 0.0, gpslat = 0.0, gpsalt = 0.0;
int gpssat = 0;

//Calibrate rain bucket here
//Rectangle raingauge from Sparkfun.com/MISOL weather sensors
float rain_bucket_mm = 0.011 * 25.4; //Each dump is 0.011" of water
//DAVISNET Rain Collector 2
//float rain_bucket_mm = 0.01*25.4;//Each dump is 0.01" of water

// volatiles are subject to modification by IRQs
volatile unsigned long raintime, rainlast, raininterval, rain, Rainindtime, Rainindlast;

//for loop
int i;
int hourReport = 0;
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

//Interrupt routines (these are called by the hardware interrupts, not by the main code)
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void rainIRQ()
// Count rain gauge bucket tips as they occur
// Activated by the magnet and reed switch in the rain gauge, attached to input D2
{
  raintime = millis(); // grab current time
  raininterval = raintime - rainlast; // calculate interval between this and last event

  if (raininterval > 100) // ignore switch-bounce glitches less than 10mS after initial edge
  {
    dailyrainin += rain_bucket_mm;
    rainHour[minutes] += rain_bucket_mm; //Increase this minute's amount of rain
    rainlast = raintime; // set up for next event
  }
}

void wspeedIRQ()
// Activated by the magnet in the anemometer (2 ticks per rotation), attached to input D3
{
  if (millis() - lastWindIRQ > 10) // Ignore switch-bounce glitches less than 10ms (142MPH max reading) after the reed switch closes
  {
    lastWindIRQ = millis(); //Grab the current time
    windClicks++; //There is 1.492MPH for each click per second.
  }
}


void setup()
{
  //Start Serial port reporting
  Serial.begin(9600);
  //Start software serial for GPS/GSM
  ss.begin(19200);  //Default serial port setting for the GPRS modem is 19200bps 8-N-1
  //Start Modem
  pinMode(SIM808, OUTPUT);
  digitalWrite(SIM808, HIGH); //SWITCH ON SIM808
  delay(40000); //WAIT FOR SIM808
  //Serial.print(sendData("AT+CPIN?", 1000, DEBUG));
  //Serial.print(sendData("AT+CPIN?", 1000, DEBUG));
  //delay(5000); //WAIT FOR SIM808
  //*//Serial.print(sendData(" ", 2000, DEBUG));
  //*//Serial.println(sendData("AT+CMGF=1", 1000, true));
  //Serial.println(sendData("AT+CSCA=\"+9477000003\"", 1000, DEBUG));  //Setting for the SMS Message center number,
  //Note that when specifying a String of characters " is entered as \"
  //Serial.println(sendData("AT+CMGS=\"+33785351655\"", 1000, DEBUG));    //Start accepting the text for the message Stef
  //*//Serial.println(sendData("AT+CMGS=\"+94774496950\"", 1000, DEBUG));    //Start accepting the text for the message Dave
  //Serial.println(sendData("AT+CMGS=\"+94716494873\"", 1000, DEBUG));    //Start accepting the text for the message YannLK
  //*//Serial.println(sendData("\"Yann test mws simduino setup\"", 1000, DEBUG));   //The text for the message
  //*//ss.write(0x1A);
  delay(1000);
  Serial.print(F("lon,lat,altitude,sats,date,GMTtime,winddir"));
  Serial.print(F(",windspeedms,windgustms,windgustdir,windspdms_avg2m,winddir_avg2m,windgustms_10m,windgustdir_10m"));
  Serial.print(F(",humidity,tempc,rainhourmm,raindailymm,pressure,batt_lvl,light_lvl"));

  pinMode(WSPEED, INPUT_PULLUP); // input from wind meters windspeed sensor
  pinMode(RAIN, INPUT_PULLUP); // input from wind meters rain gauge sensor

  pinMode(REFERENCE_3V3, INPUT);
  pinMode(LIGHT, INPUT);

  //Configure the pressure sensor
  myPressure.begin(); // Get sensor online
  myPressure.setModeBarometer(); // Measure pressure in Pascals from 20 to 110 kPa
  myPressure.setOversampleRate(7); // Set Oversample to the recommended 128
  myPressure.enableEventFlags(); // Enable all three pressure and temp event flags

  //Configure the humidity sensor
  myHumidity.begin();

  seconds = 0;
  lastSecond = millis();

  // attach external interrupt pins to IRQ functions
  attachInterrupt(0, rainIRQ, FALLING);
  attachInterrupt(1, wspeedIRQ, FALLING);

  // turn on interrupts
  interrupts();

  //GPS on, get once GPS coordinates on setup => true
  getgps(true);
  hourReport = gpshour - 1;
  minutes = gpsminute;
  minutes_10m = gpsminute;
  seconds = gpssecond;
  lastSecond = millis();
  //DEBUG
  //Serial.println(freeMemory());
}

void loop()
{
  //Keep track of which minute it is
  loopMSecond = millis() - lastSecond;
  lastSecond = millis();

  //Take a speed and direction reading every second for 2 minute average
  seconds_2m += loopMSecond / 1000;
  if (++seconds_2m > 119) seconds_2m = 0;

  //Calc the wind speed and direction every second for 120 second to get 2 minute average
  float currentSpeed = get_wind_speed();
  int currentDirection = get_wind_direction();
  windspdavg[seconds_2m] = (int)currentSpeed;
  winddiravg[seconds_2m] = currentDirection;

  //Check to see if this is a gust for the minute
  if (currentSpeed > windgust_10m[minutes_10m])
  {
    windgust_10m[minutes_10m] = currentSpeed;
    windgustdirection_10m[minutes_10m] = currentDirection;
  }

  //Check to see if this is a gust for the day
  if (currentSpeed > windgustms)
  {
    windgustms = currentSpeed;
    windgustdir = currentDirection;
  }
  seconds += loopMSecond / 1000;
  if (++seconds > 59) seconds = 0;
  if (++minutes > 59) minutes = 0;
  minutes += seconds / 60;
  minutes_10m += seconds / 60;
  if (++minutes_10m > 9) {
    minutes_10m = 0;
    for (i = 0; i < 10; i++) windgust_10m[i] = 0;
  }
  rainHour[minutes] = 0; //Zero out this minute's rainfall amount
  windgust_10m[minutes_10m] = 0; //Zero out this minute's gust
  //DEBUG: Report all readings every loop
  printWeather();
  //Wait 1 second, and gather GPS data
  delay(1000);
  getgps(false);
  //REPORTING
  //Check for a new hour passing
  //Serial.print(hourReport);
  //Serial.print(gpshour);
  //Serial.print(gpsminute);
  if ((gpsminute >= 0) && (gpsminute < 2)) {
    //Serial.print(F("inside the reporting if 1"));
    //Check that the actual hour is incremented from last report
    if (hourReport < gpshour) {
      //Serial.print(F("inside the reporting if 2"));
      //Report all readings hourly
      printWeather();
      //Turn off stat LED
      delay(1000);
      hourReport = gpshour;
    }
  }
}

void getgps(int LATLONG)
{
  //digitalWrite(SIM808, HIGH); //SWITCH ON SIM808
  delay(2000);
  String gpsdata = "";
  String gpsdatatmp = "";
  String tmp = "";
  //GPS baud rate is 9600 for coms
  switchGPSonoff( "AT+UART=9600,8,1,0,0", 1000, DEBUG);
  switchGPSonoff( "AT+CGNSPWR=1", 1000, DEBUG);
  switchGPSonoff( "AT+CGNSSEQ=RMC", 1000, DEBUG);
  do
  {
    //GET GPS DATA
    gpsdata = "";
    gpsdata.concat(sendData( "AT+CGNSINF", 1000, DEBUG));
    gpsdatatmp = "";
    delay(1000);
    gpsdatatmp.concat(gpsdata);
    gpsdatatmp.remove(75);
    //get YEAR
    gpsdatatmp.remove(31);
    gpsdatatmp.remove(0, 27);
    gpsyear = gpsdatatmp.toInt();
    gpsdatatmp = "";
    gpsdatatmp.concat(gpsdata);
    //get MONTH
    gpsdatatmp.remove(33);
    gpsdatatmp.remove(0, 31);
    gpsmonth = gpsdatatmp.toInt() ;
    gpsdatatmp = "";
    gpsdatatmp.concat(gpsdata);
    //get DAY
    gpsdatatmp.remove(35);
    gpsdatatmp.remove(0, 33);
    gpsday = gpsdatatmp.toInt() ;
    gpsdatatmp = "";
    gpsdatatmp.concat(gpsdata);
    //get HOUR
    gpsdatatmp.remove(37);
    gpsdatatmp.remove(0, 35);
    gpshour = gpsdatatmp.toInt() ;
    gpsdatatmp = "";
    gpsdatatmp.concat(gpsdata);
    //get MINUTE
    gpsdatatmp.remove(39);
    gpsdatatmp.remove(0, 37);
    gpsminute = gpsdatatmp.toInt() ;
    gpsdatatmp = "";
    gpsdatatmp.concat(gpsdata);
    //get SECOND
    gpsdatatmp.remove(41);
    gpsdatatmp.remove(0, 39);
    gpssecond = gpsdatatmp.toInt() ;
    gpsdatatmp = "";
    gpsdatatmp.concat(gpsdata);
    //Test if there is Lat/Long data
    tmp = gpsdatatmp[46];
    if (!tmp.equals(",") && LATLONG == true) {
      //get LAT (8.424)
      gpslat = tmp.toInt();
      tmp = gpsdatatmp[48];
      gpslat += 0.1 * tmp.toInt();
      tmp = gpsdatatmp[49];
      gpslat += 0.01 * tmp.toInt();
      tmp = gpsdatatmp[50];
      gpslat += 0.001 * tmp.toInt();
      //get LONG (80.357)
      tmp = gpsdatatmp[55];
      gpslng = 10 * tmp.toInt();
      tmp = gpsdatatmp[56];
      gpslng += tmp.toInt();
      tmp = gpsdatatmp[58];
      gpslng += 0.1 * tmp.toInt();
      tmp = gpsdatatmp[59];
      gpslng += 0.01 * tmp.toInt();
      tmp = gpsdatatmp[60];
      gpslng += 0.001 * tmp.toInt();
      //get ALT
      tmp = gpsdatatmp[66];
      if (tmp.equals(".")) {
        tmp = gpsdatatmp[65];
        gpsalt = tmp.toInt();
        tmp = gpsdatatmp[67];
        gpsalt += 0.1 * tmp.toInt();
      }
      tmp = gpsdatatmp[67];
      if (tmp.equals(".")) {
        tmp = gpsdatatmp[65];
        gpsalt = 10 * tmp.toInt();
        tmp = gpsdatatmp[66];
        gpsalt += tmp.toInt();
        tmp = gpsdatatmp[68];
        gpsalt += 0.1 * tmp.toInt();
      }
      tmp = gpsdatatmp[68];
      if (tmp.equals(".")) {
        tmp = gpsdatatmp[65];
        gpsalt = 100 * tmp.toInt();
        tmp = gpsdatatmp[66];
        gpsalt += 10 * tmp.toInt();
        tmp = gpsdatatmp[67];
        gpsalt += tmp.toInt();
        tmp = gpsdatatmp[68];
        gpsalt += 0.1 * tmp.toInt();
      }
      //get SAT
      gpsdatatmp.remove(71);
      gpsdatatmp.remove(0, 69);
      gpssat = gpsdatatmp.toInt();
      gpsdatatmp = "";
      gpsdata = "";
    }
  }
  while (gpsyear < 2017 || gpslng <= 0.01 || gpslat <= 0.01 || gpsalt <= 0.01); // SHOULD BE IN 2017 OR MORE TO BE VALID
  switchGPSonoff( "AT+CGNSPWR=0", 1000, DEBUG);
  //FGSM baud rate is 19200 max through software serial
  switchGPSonoff( "AT+UART=19200,8,1,0,0", 1000, DEBUG);
  //FGSM baud rate is 115200 max through HW serial PINs 0-1)
  //switchGPSonoff( "AT+UART=115200,8,1,0,0",1000,DEBUG);
  digitalWrite(SIM808, LOW); //SWITCH OFF SIM808
}

String switchGPSonoff(String command, const int timeout, boolean debug)
{
  //Serial.println("switchGPSonoff");
  String response = "";
  ss.println(command);
  long int time = millis();
  while ( (time + timeout) > millis())
  {
    while (ss.available())
    {
      char c = ss.read();
      response += c;
    }
  }
  if (debug)
  {
    Serial.println(response);
  }
  return response;
}


String sendData(String command, const int timeout, boolean debug)
{
  String response = "";
  ss.println(command);
  long int time = millis();
  while ( (time + timeout) > millis())
  {
    while (ss.available())
    {
      char c = ss.read();
      response += c;
    }
  }
  if (debug)
  {
    Serial.println(response);
  }
  return response;
}

//Calculates each of the variables that wunderground is expecting
void calcWeather()
{
  digitalWrite(SIM808, HIGH); //SWITCH OFF SIM808
  getgps(false); //get gps info (no update of coordinates => false

  //Calc winddir
  winddir = get_wind_direction();

  //Calc windspeed
  windspeedms = get_wind_speed();

  //Calc windgustms
  //Calc windgustdir
  //Report the largest windgust today
  windgustms = 0;
  windgustdir = 0;

  //Calc windspdms_avg2m
  float temp = 0;
  for (int i = 0 ; i < 120 ; i++)
    temp += windspdavg[i];
  temp /= 120.0;
  windspdms_avg2m = temp;

  //Calc winddir_avg2m
  temp = 0; //Can't use winddir_avg2m because it's an int
  for (int i = 0 ; i < 120 ; i++)
    temp += winddiravg[i];
  temp /= 120;
  winddir_avg2m = temp;

  //Calc windgustms_10m
  //Calc windgustdir_10m
  //Find the largest windgust in the last 10 minutes
  windgustms_10m = 0;
  windgustdir_10m = 0;
  //Step through the 10 minutes
  for (int i = 0; i < 10 ; i++)
  {
    if (windgust_10m[i] > windgustms_10m)
    {
      windgustms_10m = windgust_10m[i];
      windgustdir_10m = windgustdirection_10m[i];
    }
  }

  //Calc humidity
  humidity = myHumidity.readHumidity();
  if (myHumidity.readTemperature())
  {
    tempf = myHumidity.readTemperature();
  }
  else
  {
    //Calc tempf from pressure sensor
    tempf = myPressure.readTemp();
  }

  //Calculate amount of rainfall for the last 60 minutes
  rainin = 0;
  for (int i = 0 ; i < 60 ; i++)
    rainin += rainHour[i];

  //Calc pressure
  pressure = myPressure.readPressure();

  //Calc light level
  light_lvl = get_light_level();

  //Calc battery level
  batt_lvl = get_battery_level();
}

//Returns the voltage of the light sensor based on the 3.3V rail
//This allows us to ignore what VCC might be (an Arduino plugged into USB has VCC of 4.5 to 5.2V)
float get_light_level()
{
  float operatingVoltage = analogRead(REFERENCE_3V3);
  float lightSensor = analogRead(LIGHT);
  operatingVoltage = 3.3 / operatingVoltage; //The reference voltage is 3.3V
  lightSensor = operatingVoltage * lightSensor;
  return (lightSensor);
}

//Returns the voltage of the raw pin based on the 3.3V rail
//This allows us to ignore what VCC might be (an Arduino plugged into USB has VCC of 4.5 to 5.2V)
//Battery level is connected to the RAW pin on Arduino and is fed through two 5% resistors:
//3.9K on the high side (R1), and 1K on the low side (R2)
float get_battery_level()
{
  float operatingVoltage = analogRead(REFERENCE_3V3);
  float rawVoltage = analogRead(BATT);
  operatingVoltage = 3.30 / operatingVoltage; //The reference voltage is 3.3V
  rawVoltage = operatingVoltage * rawVoltage; //Convert the 0 to 1023 int to actual voltage on BATT pin
  rawVoltage *= 4.90; //(3.9k+1k)/1k - multiple BATT voltage by the voltage divider to get actual system voltage
  return (rawVoltage);
}

//Returns the instataneous wind speed
float get_wind_speed()
{
  float deltaTime = millis() - lastWindCheck; //750ms
  deltaTime /= 1000.0; //Convert to seconds
  float windSpeed = (float)windClicks / deltaTime; //3 / 0.750s = 4
  windClicks = 0; //Reset and start watching for new wind
  lastWindCheck = millis();
  //windSpeed *= 1.492; //4 * 1.492 = 5.968MPH
  windSpeed *= 1.492; //4 * 1.492 = 5.968MPH
  windSpeed *= 1.609344; //5.968MPH * 1.609344 = 9.604564992KMPH
  windSpeed /= 3.6; //9.604564992KMPH / 3.6 = 2.66793472MPS
  return (windSpeed);
}

//Read the wind direction sensor, return heading in degrees
int get_wind_direction()
{
  unsigned int adc;
  adc = analogRead(WDIR); // get the current reading from the sensor
  // The following table is ADC readings for the wind direction sensor output, sorted from low to high.
  // Each threshold is the midpoint between adjacent headings. The output is degrees for that ADC reading.
  // Note that these are not in compass degree order! See Weather Meters datasheet for more information.
  if (adc < 380) return (113);//NOT WORKING
  else if (adc < 393) return (68);//NOT WORKING
  else if (adc < 414) return (90);//NOT WORKING
  else if (adc < 456) return (90);//(158);//E is 90 degrees CW from North = 0
  else if (adc < 508) return (135);//SE is 135 degrees CW from North = 0
  else if (adc < 551) return (180);//(203);//S is 180 degrees CW from North = 0
  else if (adc < 615) return (45);//(180);//NE is 45 degrees CW from North = 0
  else if (adc < 680) return (23);//NOT WORKING
  else if (adc < 746) return (225);//(45);//SW is 225 degrees CW from North = 0
  else if (adc < 801) return (248);//NOT WORKING
  else if (adc < 833) return (0);//(225);//N is 0 degrees CW from North = 0
  else if (adc < 878) return (338);//NOT WORKING
  else if (adc < 913) return (325);//(0);//NW is 325 degrees CW from North = 0
  else if (adc < 940) return (293);//NOT WORKING
  else if (adc < 967) return (270);//(315);//W is 270 degrees CW from North = 0
  else if (adc < 990) return (270);//NOT WORKING
  else return (-1); // error, disconnected?
}


//Prints the various variables directly to the port
//I don't like the way this function is written but Arduino doesn't support floats under sprintf
void printWeather()
{
  calcWeather(); //Go calc all the various sensors

  Serial.println();
  Serial.print(gpslng, 6);//[0]
  Serial.print(",");
  Serial.print(gpslat, 6);//[1]
  Serial.print(",");
  Serial.print(gpsalt);//[2]
  Serial.print(",");
  Serial.print(gpssat);//[3]

  char sz[32];
  Serial.print(",");
  sprintf(sz, "%02d-%02d-%02d", gpsyear, gpsmonth, gpsday);//[4]
  Serial.print(sz);

  Serial.print(",");
  sprintf(sz, "%02d:%02d:%02d", gpshour, gpsminute, gpssecond);//[5]
  Serial.print(sz);

  Serial.print(",");
  Serial.print(get_wind_direction());//[6]
  Serial.print(",");
  Serial.print(get_wind_speed(), 1);//[7]
  Serial.print(",");
  Serial.print(windgustms, 1);//[8]
  Serial.print(",");
  Serial.print(windgustdir);//[9]
  Serial.print(",");
  Serial.print(windspdms_avg2m, 1);//[10]
  Serial.print(",");
  Serial.print(winddir_avg2m);//[11]
  Serial.print(",");
  Serial.print(windgustms_10m, 1);//[12]
  Serial.print(",");
  Serial.print(windgustdir_10m);//[13]
  Serial.print(",");
  Serial.print(humidity, 1);//[14]
  Serial.print(",");
  Serial.print(tempf, 1);//[15] Celsius
  Serial.print(",");
  Serial.print(rainin, 3);//[16] hourly
  Serial.print(",");
  Serial.print(dailyrainin, 3);//[17]
  Serial.print(",");
  Serial.print(pressure, 2);//[20]
  Serial.print(",");
  Serial.print(batt_lvl, 2);//[21]
  Serial.print(",");
  Serial.print(light_lvl, 2);//[22]
  Serial.print(",");
}












