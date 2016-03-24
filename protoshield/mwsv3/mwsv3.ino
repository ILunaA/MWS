//SD card
//http://blog.oscarliang.net/sd-card-arduino/
//#include <SD.h>
//File sd;

//Gyro/compass SDA=A4 SCL=A5
//Removed as BMP180 sensor already uses the pins
#include <Wire.h>
//#include <HMC5883L.h>
//HMC5883L compass;

//Light garage lamp Sensor on A1
//int lightPin1 = 1;

// DS18B20 Temperature (+ 4.7kOhm resistor on VCC 5V)
#include <OneWire.h>
#include <DallasTemperature.h>
// Data wire is plugged into pin 9 on the Arduino
// pin 10 is needed for the SD card SS
#define ONE_WIRE_BUS 9
// Setup a oneWire instance to communicate with any OneWire devices 
// (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

//Light Sensor on A0 (+1KOhm resistor on GND)
int lightPin = 0;

//BMP180 SDA=A4, SDC=A5
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085_U.h>
Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085);

//DHT11 data pin=Digital 6
#include <dht11.h>
dht11 DHT11;
#define DHT11PIN 6

// Get the LCD I2C Library here: 
// https://bitbucket.org/fmalpartida/new-liquidcrystal/downloads
// Move any other LCD libraries to another folder or delete them
// See Library "Docs" folder for possible commands etc.
#include <LiquidCrystal_I2C.h>
/*-----( Declare Constants )-----*/
/*-----( Declare objects )-----*/
// set the LCD address to 0x27 for a 20 chars 4 line display
// Set the pins on the I2C chip used for LCD connections:
//                    addr, en,rw,rs,d4,d5,d6,d7,bl,blpol
LiquidCrystal_I2C lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address



void setup()
{
  Serial.begin(115200);
  //Gyro/Compass
  Wire.begin();
  //compass = HMC5883L();
  //compass.SetScale(1.3);
  //compass.SetMeasurementMode(Measurement_Continuous);
  //Start Dallas D18B20
  sensors.begin();
  //Start the BMP180 Pressure sensor
  bmp.begin();
  // set up the LCD's number of columns and rows:
  lcd.begin(20, 4);
  // Print a message to the LCD.
  // ------- Quick 3 blinks of backlight  -------------
  for(int i = 0; i< 3; i++)
  {
    lcd.backlight();
    delay(250);
    lcd.noBacklight();
    delay(250);
  }
  lcd.backlight(); // finish with backlight on  

  lcd.setCursor(0, 0);
  lcd.print("MWS v3");
  lcd.setCursor(0, 2);
  lcd.print("Starting Sensors");
  delay(2000);
}

void loop()
{
  //sd = SD.open("test.txt", FILE_WRITE);
  //Gyro/Compass
  //MagnetometerRaw raw = compass.ReadRawAxis();
  //MagnetometerScaled scaled = compass.ReadScaledAxis();
  //float xHeading = atan2(scaled.YAxis, scaled.XAxis);
  //float yHeading = atan2(scaled.ZAxis, scaled.XAxis);
  //float zHeading = atan2(scaled.ZAxis, scaled.YAxis);
  //if(xHeading < 0) xHeading += 2*PI;
  //if(xHeading > 2*PI) xHeading -= 2*PI;
  //if(yHeading < 0) yHeading += 2*PI; 
  //if(yHeading > 2*PI) yHeading -= 2*PI;
  //if(zHeading < 0) zHeading += 2*PI;
  //if(zHeading > 2*PI) zHeading -= 2*PI;
  //float xDegrees = xHeading * 180/M_PI;
  //float yDegrees = yHeading * 180/M_PI;
  //float zDegrees = zHeading * 180/M_PI;
  //Serial.print(xDegrees);
  //Serial.print(",");
  //Serial.print(yDegrees);
  //Serial.print(",");
  //Serial.print(zDegrees);
  //Serial.println(";");
  //delay(100); 
  //lcd.clear();
  //lcd.setCursor(0, 0);
  //lcd.print("X Y Z Heading");
  //lcd.setCursor(0, 1);
  //lcd.print(xDegrees);
  //lcd.print(",");
  //lcd.print(yDegrees);
  //lcd.print(",");
  //lcd.print(zDegrees);
  //lcd.print(" Deg");
  //delay(2000);

  // Read Light Garage Lamp Sensor
  //lcd.clear();
  //lcd.setCursor(0, 0);
  //lcd.print("Light Sensor 1 ");
  //lcd.setCursor(0, 1);
  //lcd.print((float) analogRead(lightPin1));
  //lcd.print(" -");
  //delay(2000);

  //Get temperature from Dallas D18B20
  sensors.requestTemperatures();
  float dallasT = sensors.getTempCByIndex(0);// Why "byIndex"? 
  // You can have more than one IC on the same bus. 
  // 0 refers to the first IC on the wire 
  Serial.print("Temperature DS12B80 (C): ");
  Serial.println(dallasT); 
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temperature DS12B80 ");
  lcd.setCursor(0, 1);
  lcd.print(dallasT);
  lcd.print(" C");
  //sd.print(dallasT);
  //sd.print(",");
  lcd.setCursor(0, 2);
  lcd.print("              DHT11 ");
  lcd.setCursor(0, 3);
  lcd.print((float)DHT11.temperature, 2);
  lcd.print(" C");
  delay(3000);

  // Read Light Sensor
  //1023/2=511 = night = 1KOhms (equal to paired resistor)
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Light Sensor ");
  lcd.setCursor(0, 1);
  float vout=analogRead(lightPin)*0.0048828125;
  lcd.print((int) 500/(10.0*((5.0-vout)/vout)));
  lcd.print(" Ohms");

  /* Get a new sensor event */
  sensors_event_t event;
  bmp.getEvent(&event);
  /* Display the results (barometric pressure is measure in hPa) */
  if (event.pressure)
  {
    /* Display atmospheric pressure in hPa */
    Serial.print("Pressure (hPa): "); 
    Serial.println(event.pressure); 
    lcd.setCursor(0, 2);
    lcd.print("Pressure ");
    lcd.setCursor(0, 3);
    lcd.print(event.pressure);
    lcd.print(" hPa");
    //sd.print(event.pressure);
    //sd.print(",");
    delay(3000);
  }
  else
  {
    Serial.println("BMP180 Sensor error");
  }

  int chk = DHT11.read(DHT11PIN);

  //Serial.print("Read sensor: ");
  switch (chk)
  {
  case DHTLIB_OK: 
    //Serial.println("DHT11 OK"); 
    break;
  case DHTLIB_ERROR_CHECKSUM: 
    Serial.println("DHT11 Checksum error"); 
    break;
  case DHTLIB_ERROR_TIMEOUT: 
    Serial.println("DHT11 Time out error"); 
    break;
  default: 
    Serial.println("DHT11 Unknown error"); 
    break;
  }

  Serial.print("Humidity (%): ");
  Serial.println((float)DHT11.humidity, 2);
  lcd.clear();
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  lcd.setCursor(0, 0);
  lcd.print("Humidity ");
  lcd.setCursor(0, 1);
  lcd.print((float)DHT11.humidity,2);
  lcd.print(" %");
  //sd.print((float)DHT11.humidity,2);
  //sd.print(",");
  delay(3000);
  Serial.print("Temperature DHT: ");
  Serial.println((float)DHT11.temperature, 2);
  lcd.setCursor(0, 0);
  lcd.print("Temperature ");
  lcd.setCursor(0, 1);
  lcd.print((float)DHT11.temperature, 2);
  lcd.print(" C");
  //sd.print((float)DHT11.temperature, 2);
  //sd.print(",");
  Serial.print("Temperature (F): ");
  Serial.println(Fahrenheit(DHT11.temperature), 2);
  lcd.setCursor(0, 2);
  lcd.print(Fahrenheit(DHT11.temperature), 2);
  lcd.print(" F");
  Serial.print("Temperature (K): ");
  Serial.println(Kelvin(DHT11.temperature), 2);
  lcd.setCursor(0, 3);
  lcd.print(Kelvin(DHT11.temperature), 2);
  lcd.print(" K");
  //lcd.print("hello, world!");
  delay(3000);
  Serial.print("Dew Point (C): ");
  Serial.println(dewPoint(DHT11.temperature, DHT11.humidity));
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Dew Point (NOAA)");
  lcd.setCursor(0, 1);
  lcd.print(dewPoint(DHT11.temperature, DHT11.humidity));
  lcd.print(" C");
  //lcd.print("hello, world!");
  Serial.print("Dew PointFast (C): ");
  Serial.println(dewPointFast(DHT11.temperature, DHT11.humidity));
  lcd.setCursor(0, 2);
  lcd.print("Fast code");
  lcd.setCursor(0, 3);
  lcd.print(dewPointFast(DHT11.temperature, DHT11.humidity));
  lcd.print(" C");

  //close SD card file
  //sd.print("\n");
  //sd.close();

  delay(3000);

}


//float getHeading(){
//  //Get the reading from the HMC5883L and calculate the heading
//  MagnetometerScaled scaled = compass.ReadScaledAxis(); //scaled values from compass.
//  float heading = atan2(scaled.YAxis, scaled.XAxis);
//
//  // Correct for when signs are reversed.
//  if(heading < 0) heading += 2*PI;
//  if(heading > 2*PI) heading -= 2*PI;
//
//  return heading * RAD_TO_DEG; //radians to degrees
//}


//Celsius to Fahrenheit conversion
double Fahrenheit(double celsius)
{
  return 1.8 * celsius + 32;
}

// fast integer version with rounding
//int Celcius2Fahrenheit(int celcius)
//{
//  return (celsius * 18 + 5)/10 + 32;
//}


//Celsius to Kelvin conversion
double Kelvin(double celsius)
{
  return celsius + 273.15;
}

// dewPoint function NOAA
// reference (1) : http://wahiduddin.net/calc/density_algorithms.htm
// reference (2) : http://www.colorado.edu/geography/weather_station/Geog_site/about.htm
//
double dewPoint(double celsius, double humidity)
{
  // (1) Saturation Vapor Pressure = ESGG(T)
  double RATIO = 373.15 / (273.15 + celsius);
  double RHS = -7.90298 * (RATIO - 1);
  RHS += 5.02808 * log10(RATIO);
  RHS += -1.3816e-7 * (pow(10, (11.344 * (1 - 1/RATIO ))) - 1) ;
  RHS += 8.1328e-3 * (pow(10, (-3.49149 * (RATIO - 1))) - 1) ;
  RHS += log10(1013.246);

  // factor -3 is to adjust units - Vapor Pressure SVP * humidity
  double VP = pow(10, RHS - 3) * humidity;

  // (2) DEWPOINT = F(Vapor Pressure)
  double T = log(VP/0.61078);   // temp var
  return (241.88 * T) / (17.558 - T);
}

// delta max = 0.6544 wrt dewPoint()
// 6.9 x faster than dewPoint()
// reference: http://en.wikipedia.org/wiki/Dew_point
double dewPointFast(double celsius, double humidity)
{
  double a = 17.271;
  double b = 237.7;
  double temp = (a * celsius) / (b + celsius) + log(humidity*0.01);
  double Td = (b * temp) / (a - temp);
  return Td;
}





