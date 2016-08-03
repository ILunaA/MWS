//Found @ http://arduinotronics.blogspot.fr/2015/08/measuring-wind-speed-with-arduino.html


 // diameter of anemometer
 float diameter = 2.75; //inches from center pin to middle of cup
 float mph;
 
 // read RPM
 int half_revolutions = 0;
 int rpm = 0;
 unsigned long lastmillis = 0;
 
 void setup(){
 Serial.begin(9600); 
 pinMode(2, INPUT_PULLUP); 
 attachInterrupt(0, rpm_fan, FALLING);
 }
 
 void loop(){
 if (millis() - lastmillis == 1000){ //Update every one second, this will be equal to reading frequency (Hz).
 detachInterrupt(0);//Disable interrupt when calculating
 rpm = half_revolutions * 30; // Convert frequency to RPM, note: 60 works for one interruption per full rotation. For two interrupts per full rotation use half_revolutions * 30.
 Serial.print("RPM =\t"); //print the word "RPM" and tab.
 Serial.print(rpm); // print the rpm value.
 Serial.print("\t Hz=\t"); //print the word "Hz".
 Serial.print(half_revolutions/2); //print revolutions per second or Hz. And print new line or enter. divide by 2 if 2 interrupts per revolution
 half_revolutions = 0; // Restart the RPM counter
 lastmillis = millis(); // Update lastmillis
 attachInterrupt(0, rpm_fan, FALLING); //enable interrupt
 mph = diameter / 12 * 3.14 * rpm * 60 / 5280;
 mph = mph * 3.5; // calibration factor for anemometer accuracy, adjust as necessary

 Serial.print("\t MPH=\t"); //print the word "MPH".
 Serial.println(mph);
  }
 }
 // this code will be executed every time the interrupt 0 (pin2) gets low.
 void rpm_fan(){
  half_revolutions++;
 }


