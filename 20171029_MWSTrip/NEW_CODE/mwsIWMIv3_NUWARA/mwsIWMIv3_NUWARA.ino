#include <Wire.h> //I2C needed for sensors
#include "MPL3115A2.h" //Pressure sensor
#include "HTU21D.h" //Humidity sensor
#include <SoftwareSerial.h> //Needed for GPS
#include "GSM_Module.h" //Needed for SMS/Server upload
#include <TinyGPS++.h> //GPS parsing

//FOR GSM COMS
GSM_Module Module(SIM900);// For SIM900 GSM            
//GSM_Module Module(SM5100B);// For SM5100B GSM

String Mobile_No2 = "+94713805660";          // IrDep Lasindu Mobile Number 2 which the SMS Sends
String Mobile_No7 = "+94716494873";          // IWMI Yann Mobile Number 7 which the SMS Sends
String Mobile_No8 = "+94774496950";          // IWMI David Mobile Number 8 which the SMS Sends 

int Status;
TinyGPSPlus gps;

//For Arduino Uno
//static const int RXPin = 5, TXPin = 4; //GPS is attached to pin 4(TX from GPS) and pin 5(RX into GPS)
//For Arduino Mega
static const int RXPin = 50, TXPin = 51; //GPS is attached to pin 7(TX from GPS) and pin 6(RX into GPS)
SoftwareSerial ss(RXPin, TXPin); 

MPL3115A2 myPressure; //Create an instance of the pressure sensor
HTU21D myHumidity; //Create an instance of the humidity sensor

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//Hardware pin definitions
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// digital I/O pins
const byte RAIN = 2;
const byte WSPEED = 3;
const byte GPS_PWRCTL = 6; //Pulling this pin low puts GPS to sleep but maintains RTC and RAM
// analog I/O pins
const byte WDIR = A0;
const byte LIGHT = A1;
const byte BATT = A2;
const byte REFERENCE_3V3 = A3;
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//Global Variables
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
long lastMSecond      = 0; //The millis counter to see when a second rolls by
long loopMSecond      = 0; //time it took to loop once in millis
long loopMSecond_Rem  = 0; // Remining millis of Second calcultion.
int  seconds          = 0; //When it hits 60, increase the current minute
byte loop_seconds     = 0;
byte last_10_seconds  = 0;
byte ten_sec_count    = 0;
byte ten_second_5m    = 0; //Keeps track of the "wind speed/dir avg" over last 5 minutes array of data
int  minutes          = 0; //Keeps track of where we are in various arrays of data
byte minutes_5m       = 0; //Keeps track of where we are in rain5min over last 5 minutes array of data
int  hours            = 0; 
byte last_hour        = 0;

long lastWindCheck        = 0;
volatile long lastWindIRQ = 0;
volatile byte windClicks  = 0;

//We need to keep track of the following variables:
//Wind speed/dir each update (no storage)
//Wind gust/dir over the day (no storage)
//Wind speed/dir, avg over 2 minutes (store 1 per second)
//Wind gust/dir over last 10 minutes (store 1 per minute)
//Rain over the past hour (store 1 per minute)
//Total rain over date (store one per day)

float windspdavg[30] = {0.0}; //30 bytes to keep track of 2 minute average
int winddiravg[30] = {0}; //30 ints to keep track of 2 minute average

volatile float rainHour = 0.0; //60 floating numbers to keep track of 60 minutes of rain
volatile float rainDay  = 0.0; //24 floating numbers to keep track of 24 hours of rain
volatile float rain5min = 0.0; //5 floating numbers to keep track of 5 minutes of rain

//These are all the weather values that wunderground expects:
int   winddir         = 0; // [0-360 instantaneous wind direction]
float windspeedms     = 0.0; // [mph instantaneous wind speed]
float windgustms      = 0.0; // [mph current wind gust, using software specific time period]
int   windgustdir     = 0; // [0-360 using software specific time period]
float windspdms_avg5m = 0.0; // [mph 5 minute average wind speed mph]
int   winddir_avg5m   = 0; // [0-360 5 minute average wind direction]
float windgust_5m     = 0.0; // [mph past 5 minutes wind gust mph ]
int   windgustdir_5m  = 0; // [0-360 past 5 minutes wind gust direction]
float humidity        = 0.0; // [%]
float tempf           = 0.0; // [temperature F]
float pressure        = 0.0;

//float baromin = 30.03;// [barom in] - It's hard to calculate baromin locally, do this in the agent
//float dewptf; // [dewpoint F] - It's hard to calculate dewpoint locally, do this in the agent

float batt_lvl = 11.8; //[analog value from 0 to 1023]
float light_lvl = 455; //[analog value from 0 to 1023]
//Rain time stamp
int Rainindi=0;
//Variables used for GPS
int gps_year, gps_month, gps_day;
int seconds_new, minutes_new, hours_new, gps_year_new, gps_month_new, gps_day_new; 

//Calibrate rain bucket here
//Rectangle raingauge from Sparkfun.com weather sensors
float rain_bucket_mm = 0.011*25.4;//Each dump is 0.011" of water
//DAVISNET Rain Collector 2
//float rain_bucket_mm = 0.01*25.4;//Each dump is 0.01" of water

// volatiles are subject to modification by IRQs
volatile unsigned long raintime, rainlast, raininterval, rain, Rainindtime, Rainindlast;

float currentSpeed;
int currentDirection;

//for loop
int i;
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
    rainDay  += rain_bucket_mm; 
    rainHour += rain_bucket_mm; //Increase this minute's amount of rain
    rain5min += rain_bucket_mm; // increase this 5 mnts amout of rain
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
  /*****************************************************************************************************************
   * Initialize the Serial port for Data Logging 
   *****************************************************************************************************************/
  Serial.begin(9600);
  // Inform data logger about CSV header
  Serial.print(F("lon,lat,altitude,sats,date,GMTtime,winddir"));
  Serial.print(F(",windspeedms,windgustms,windgustdir,windspdms_avg5m,winddir_avg5m,windgustms_5m,windgustdir_5m"));
  Serial.print(F(",humidity,tempc,rainhourmm,raindailymm,rainindicate,rain5min,pressure,batt_lvl,light_lvl"));
  /*****************************************************************************************************************
   * Initialize the Sensors 
   *****************************************************************************************************************/
  pinMode(WSPEED, INPUT_PULLUP); // input from wind meters windspeed sensor
  //pinMode(RAIN, INPUT_PULLUP); // input from wind meters rain gauge sensor
  //using 0.1uF capacitor between PIN2 and GND, does not need PULLUP resistor
  pinMode(RAIN, INPUT); // input from wind meters rain gauge sensor
  pinMode(REFERENCE_3V3, INPUT); //For battery level
  pinMode(LIGHT, INPUT);//For light sensor onboard weather shield
  // Configure the pressure sensor
  myPressure.begin(); // Get sensor online
  myPressure.setModeBarometer(); // Measure pressure in Pascals from 20 to 110 kPa
  myPressure.setOversampleRate(7); // Set Oversample to the recommended 128
  myPressure.enableEventFlags(); // Enable all three pressure and temp event flags 
  // Configure the humidity sensor
  myHumidity.begin();
  // attach external interrupt pins to IRQ functions
  attachInterrupt(0, rainIRQ, FALLING);
  attachInterrupt(1, wspeedIRQ, FALLING);
  // turn on interrupts
  interrupts();
  delay(2000);
  /*****************************************************************************************************************
   * Initialize the GSM module. 
   *****************************************************************************************************************/
  pinMode(SIM900_Power_Pin, OUTPUT);
  Module.Init(9600);
  Module.Refresh();
  Module.Close();
  delay(10000);
  /*****************************************************************************************************************
   * Sending a test SMS 
   *****************************************************************************************************************/
  sendInitSMS();
  delay(1000);
  /*****************************************************************************************************************
   * Initialize GPS data to give date & time 
   *****************************************************************************************************************/
  pinMode(GPS_PWRCTL, OUTPUT);
  smartdelay(240000); //During 4 minutes, gather GPS data (hoping for a fix !)
  minutes       = gps.time.minute();
  minutes_5m    = (gps.time.minute())%5;
  seconds       = gps.time.second();
  hours         = gps.time.hour();
  gps_year      = gps.date.year();
  gps_month     = gps.date.month();
  gps_day       = gps.date.day();
  lastMSecond = millis();
  /*****************************************************************************************************************
   * Finished Setup 
   *****************************************************************************************************************/
}

void loop()
{
  /*****************************************************************************************************************
   * Time keeping
   *****************************************************************************************************************/
  //Keep track of which minute it is
  loopMSecond     = (millis() - lastMSecond) + loopMSecond_Rem;
  lastMSecond     = millis();
  loop_seconds    = loopMSecond/1000; 
  loopMSecond_Rem = loopMSecond%1000;
  seconds         += loop_seconds; 
  if(seconds > 59){
    seconds     = seconds%60;
    minutes     ++;
    minutes_5m  ++;
  }
  if(hours > 23 ){ 
    hours    = 0;
  }
  /*****************************************************************************************************************
   * Take a speed and direction reading every 10 second for 5 minute average
   *****************************************************************************************************************/
  ten_second_5m   = ((seconds/10) + (minutes%5)*6);
  if(last_10_seconds != ten_second_5m){ 
    currentSpeed              = get_wind_speed();
    currentDirection          = get_wind_direction();
    windspdavg[ten_second_5m] = currentSpeed;
    winddiravg[ten_second_5m] = currentDirection;
    //Check to see if this is a gust for the minute
    if(currentSpeed > windgust_5m)
    {
      windgust_5m     = currentSpeed;
      windgustdir_5m  = currentDirection;
    }
    //Check to see if this is a gust for the day
    if(currentSpeed > windgustms)
    {
      windgustms  = currentSpeed;
      windgustdir = currentDirection;
    }
    last_10_seconds  = ten_second_5m;
    ten_sec_count    ++;
  }
  /*****************************************************************************************************************
   * 1 hour reporting
   *****************************************************************************************************************/
  if(minutes > 59) {  
    minutes = minutes%60;
    hours ++;
    if(rainHour > 200.0){ //This is a fix for IRQs getting crazy bc of irregular power in the box (Yann: 28 April 2017, reduced to 200 27Sept2017)
      rainDay = rainDay - rainHour; // Fix the daily total too
      rainHour = 0.0;
    }
    if(rainHour > 10.0){ //SMS Alert if hourly rain > 10 mm/h, Modified after Lasindu's request 22 April 2016
      sendAlert();
    }
    if(hours == 2){ 
      sendDailyRainSMS();
      rainDay = 0;
    }
    rainHour = 0; 
  }
  /*****************************************************************************************************************
   * 5 minutes reporting
   *****************************************************************************************************************/
  if(minutes_5m > 4){
    minutes_5m = minutes_5m%5;
    //Calc the wind speed and direction every 10 seconds for 120 seconds to get 5 minutes average
    //Calc windspdms_avg5m
    float temp = 0;
    for(int i = 0 ; i < 30 ; i++){
      temp += windspdavg[i];
      windspdavg[i] = 0;
    }
    temp /= ten_sec_count;
    windspdms_avg5m = temp;
    //Calc winddir_avg5m
    temp = 0; //Can't use winddir_avg2m because it's an int
    for(int i = 0 ; i < 30 ; i++){
      temp += winddiravg[i];
      winddiravg[i] = 0;  
    }
    temp /= ten_sec_count;
    winddir_avg5m = temp;  
    //Rain or not (1 or 0)
    if(rain5min > 0)
    {
      Rainindi=1;
      Rainindtime = millis();
    }
    else{
      Rainindi=0; 
      Rainindlast = Rainindtime;
    } 
    //Report all readings
    printWeather(); 
    windgust_5m      = 0;
    windgustdir_5m   = 0;
    rain5min         = 0;
    ten_sec_count    = 0; 

  } //END OF 5Min reporting
  /*****************************************************************************************************************
   * Siwtch ON GPS, gather data during 1 minute, switch OFF GPS
   *****************************************************************************************************************/
  //Wait, and gather GPS data
  smartdelay(60000); //Wait 60 seconds, and gather GPS data
  seconds_new   = gps.time.second();
  minutes_new   = gps.time.minute();
  hours_new     = gps.time.hour();
  gps_year_new  = gps.date.year();
  gps_month_new = gps.date.month();
  gps_day_new   = gps.date.day();
  if ((gps_year_new > gps_year) || ((gps_month_new > gps_month) && (gps_year_new >= gps_year)) || ((gps_day_new > gps_day) && (gps_month_new >= gps_month)) || ((hours_new > hours) && (gps_day_new >= gps_day)) || ((minutes_new > minutes) && (hours_new >= hours))){
    minutes       = minutes_new;
    minutes_5m    = minutes_new%5;
    seconds       = seconds_new;
    hours         = hours_new;
    gps_year      = gps_year_new;
    gps_month     = gps_month_new;
    gps_day       = gps_day_new;
  }
}

//While we delay for a given amount of time, gather GPS data
static void smartdelay(unsigned long ms)
{
  unsigned long start = millis();
  digitalWrite(GPS_PWRCTL, HIGH); //Pulling this pin low puts GPS to sleep but maintains RTC and RAM
  do 
  {
    while (ss.available())
      gps.encode(ss.read());
  } 
  while (millis() - start < ms);
  digitalWrite(GPS_PWRCTL, LOW); //Pulling this pin low puts GPS to sleep but maintains RTC and RAM
}

//Calculates each of the variables that wunderground is expecting
void calcWeather()
{  
  humidity = myHumidity.readHumidity();//Calc humidity
  if(myHumidity.readTemperature())
    tempf = myHumidity.readTemperature();//Calc Temperature
  else tempf = myPressure.readTemp();//Calc tempf from pressure sensor
  //Total rainfall for the day is calculated within the interrupt
  pressure = myPressure.readPressure();//Calc pressure
  light_lvl = get_light_level();//Calc light level
  batt_lvl = get_battery_level();//Calc battery level
}

//Returns the voltage of the light sensor based on the 3.3V rail
//This allows us to ignore what VCC might be (an Arduino plugged into USB has VCC of 4.5 to 5.2V)
float get_light_level()
{
  float operatingVoltage = analogRead(REFERENCE_3V3);
  float lightSensor = analogRead(LIGHT);
  operatingVoltage = 3.3 / operatingVoltage; //The reference voltage is 3.3V
  lightSensor = operatingVoltage * lightSensor;
  return(lightSensor);
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
  return(rawVoltage);
}

//Returns the instantaneous wind speed
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
  return(windSpeed);
}

//Read the wind direction sensor, return heading in degrees
int get_wind_direction() 
{
  unsigned int adc;
  adc = analogRead(WDIR); // get the current reading from the sensor
  // The following table is ADC readings for the wind direction sensor output, sorted from low to high.
  // Each threshold is the midpoint between adjacent headings. The output is degrees for that ADC reading.
  // Note that these are not in compass degree order! See Weather Meters datasheet for more information.
  if (adc < 350) return (113);//NOT WORKING
  else if (adc < 440) return (90);  // E is 90 degrees CW from North = 0
  else if (adc < 525) return (135); //SE is 135 degrees CW from North = 0
  else if (adc < 635) return (180); // S is 180 degrees CW from North = 0
  else if (adc < 755) return (45);  //NE is 45 degrees CW from North = 0
  else if (adc < 855) return (225); //SW is 225 degrees CW from North = 0
  else if (adc < 965) return (0);   // N is 0 degrees CW from North = 0
  else if (adc < 1000) return (270);// W is 270 degrees CW from North = 0
  else return (325);                //NW is 325 degrees CW from North = 0
}

//Prints the various variables directly to the port
//I don't like the way this function is written but Arduino doesn't support floats under sprintf
void printWeather()
{
  calcWeather(); //Go calc all the various sensors

  Serial.println();
  Serial.print(gps.location.lng(), 6);//[0]
  Serial.print(",");
  Serial.print(gps.location.lat(), 6);//[1]
  Serial.print(",");
  Serial.print(gps.altitude.meters());//[2]
  Serial.print(",");
  Serial.print(gps.satellites.value());//[3]

  char sz[32];
  Serial.print(",");
  sprintf(sz, "%02d-%02d-%02d", gps_year, gps_month, gps_day);//[4]
  Serial.print(sz);

  Serial.print(",");
  sprintf(sz, "%02d:%02d:%02d", hours, minutes, seconds);//[5]
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
  Serial.print(windspdms_avg5m, 1);//[10]
  Serial.print(",");
  Serial.print(winddir_avg5m);//[11]
  Serial.print(",");
  Serial.print(windgust_5m, 1);//[12]
  Serial.print(",");
  Serial.print(windgustdir_5m);//[13]
  Serial.print(",");
  Serial.print(humidity, 1);//[14]
  Serial.print(",");
  Serial.print(tempf, 1);//[15] Celsius
  Serial.print(",");
  Serial.print(rainHour, 3);//[16] hourly
  Serial.print(",");
  Serial.print(rainDay, 3);//[17]
  Serial.print(",");
  Serial.print(Rainindi,1);//[18] 5min indicatior (flag 0/1)
  Serial.print(",");
  Serial.print(rain5min,3);//[19] 5min rain (mm/5min)
  Serial.print(",");
  Serial.print(pressure, 2);//[20]
  Serial.print(",");
  Serial.print(batt_lvl, 2);//[21]
  Serial.print(",");
  Serial.print(light_lvl, 2);//[22]
  Serial.print(",");
}

void sendInitSMS()
{
  /*****************************************************************************************************************
   * Sending an Initialization SMS 
   *****************************************************************************************************************/
  char sz[128];
  //sprintf(sz,"IWMI MWS v3 Station initializing at HOTEL_TEST");    
  //sprintf(sz,"IWMI MWS v3 Station initializing at UO_MAHAKANADARAWA");    
  //sprintf(sz,"IWMI MWS v3 Station initializing at UO_ATHURUWELLA");    
  //sprintf(sz,"IWMI MWS v3 Station initializing at NALLAMUDAWA_MAWATHAWEWA");    
  //sprintf(sz,"IWMI MWS v3 Station initializing at THIRAPPANE_FARM");    
  sprintf(sz,"IWMI MWS v3 Station initializing at UO_NUWARAWEWA_SALIYAPURA");    
  //sprintf(sz,"IWMI MWS v3 Station initializing at SRI_SADANANDA_PIRIVENA");    

  ss.end();
  delay(500);
  if(Module.Init(9600) == OK){
    delay(1000);
    sendSMS(Mobile_No7, sz);//Yann
    delay(1000);
    //sendSMS(Mobile_No2, sz);//Lasindhu
    //delay(1000);
    //sendSMS(Mobile_No8, sz);//David
    //delay(1000);
  }
  ss.begin(9600);
}

void sendAlert()
{
  /*****************************************************************************************************************
   * Sending an alert SMS 
   *****************************************************************************************************************/
  char sz[128];
  char rainbuf[128];

  dtostrf(rainHour, 3, 2, rainbuf);
  //sprintf(sz, "UO_MAHAKANADARAWA\n%s mm/h",rainbuf);//[4]
  //sprintf(sz, "UO_ATHURUWELLA\n%s mm/h",rainbuf);//[4]
  //sprintf(sz, "NALLAMUDAWA_MAWATHAWEWA\n%s mm/h",rainbuf);//[4]
  //sprintf(sz, "THIRAPPANE_FARM\n%s mm/h",rainbuf);//[4]
  sprintf(sz, "UO_NUWARAWEWA_SALIYAPURA\n%s mm/h",rainbuf);//[4]
  //sprintf(sz, "SRI_SADANANDA_PIRIVENA\n%s mm/h",rainbuf);//[4]
  //sprintf(sz, "HOTEL_TEST\nxxx mm/h");//[4]
  //UO abandoned, removed 23 April 2017
  //sprintf(sz, "UO_LABUNORUWA\n%s mm/h",rainbuf);//[4]

  ss.end();
  delay(500);
  if(Module.Init(9600) == OK){
    delay(1000);
    sendSMS(Mobile_No2, sz);//Lasindhu
    delay(1000);
    sendSMS(Mobile_No7, sz);//Yann
    delay(1000);
    sendSMS(Mobile_No8, sz);//David
    delay(1000);
  }
  ss.begin(9600);

  /*****************************************************************************************************************
   * Finished Sending an alert SMS 
   *****************************************************************************************************************/
}

void sendDailyRainSMS(){
  //  /*****************************************************************************************************************
  //   * Sending an alert SMS 
  //   *****************************************************************************************************************/
  char sz[255];
  char rainbuf[128];

  dtostrf(rainDay, 4, 2, rainbuf);

  smartdelay(10000); //Wait 10 seconds, and gather GPS data

  //sprintf(sz, "UO_MAHAKANADARAWA\n%02d-%02d-%02d\n%02d:%02d:%02d GMT\n%s mm/d", gps_year, gps_month, gps_day, hours, minutes, seconds, rainbuf);
  //sprintf(sz, "UO_ATHURUWELLA\n%02d-%02d-%02d\n%02d:%02d:%02d GMT\n%s mm/d", gps_year, gps_month, gps_day, hours, minutes, seconds, rainbuf);
  //sprintf(sz, "NALLAMUDAWA_MAWATHAWEWA\n%02d-%02d-%02d\n%02d:%02d:%02d GMT\n%s mm/d", gps_year, gps_month, gps_day, hours, minutes, seconds, rainbuf);
  //sprintf(sz, "THIRAPPANE_FARM\n%02d-%02d-%02d\n%02d:%02d:%02d GMT\n%s mm/d", gps_year, gps_month, gps_day, hours, minutes, seconds, rainbuf);
  sprintf(sz, "UO_NUWARAWEWA_SALIYAPURA\n%02d-%02d-%02d\n%02d:%02d:%02d GMT\n%s mm/d", gps_year, gps_month, gps_day, hours, minutes, seconds, rainbuf);
  //sprintf(sz, "SRI_SADANANDA_PIRIVENA\n%02d-%02d-%02d\n%02d:%02d:%02d GMT\n%s mm/d", gps_year, gps_month, gps_day, hours, minutes, seconds, rainbuf);
  //sprintf(sz, "HOTEL_TEST\n%02d-%02d-%02d\n%02d:%02d:%02d GMT\n%s mm/d", gps_year, gps_month, gps_day, hours, minutes, seconds, rainbuf);

  ss.end();
  delay(500);
  if(Module.Init(9600) == OK){
    delay(1000);
    sendSMS(Mobile_No2, sz);//Lasindhu
    delay(1000);
    sendSMS(Mobile_No7, sz);//Yann
    delay(1000);
    sendSMS(Mobile_No8, sz);//David
    delay(1000);
  }
  ss.begin(9600);
}

void sendSMS(String Mobile_No, String message){
  Status = Module.Send_SMS(Mobile_No, message);
  Module.Refresh();
  delay(1000);
  Module.Close();
  delay(500);
}












