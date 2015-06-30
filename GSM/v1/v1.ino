#include <SoftwareSerial.h> //Include the NewSoftSerial library to send serial commands to the cellular module.
#include <string.h>         //Used for string manipulations

char incoming_char=0;      //Will hold the incoming character from the Serial Port.

//NewSoftSerial cell(2,3);  //Create a 'fake' serial port. Pin 2 is the Rx pin, pin 3 is the Tx pin.
SoftwareSerial cell(10, 11); // RX, TX

int i;
String Incoming_String;
bool String_Received;

void setup()
{
  //Initialize serial ports for communication.
  pinMode(2, INPUT);
  pinMode(3, INPUT);
  Serial.begin(9600);
  cell.begin(9600);
  Incoming_String = "";
  String_Received = 0;
  //Let's get started!
  Serial.println("Starting SM5100B Communication...");
}

void loop() {
  //If a character comes in from the cellular module...
  if(cell.available() >0)
  {
    incoming_char=cell.read();    //Get the character from the cellular serial port.
    Incoming_String += incoming_char; 
    // Process message when new line character is recieved
    if (incoming_char == '\n')
    {
      if(Incoming_String == "\r\n"){
        Incoming_String = ""; 
      }else{ 
        String_Received = 1; 
      }
    }   
    //Serial.print(incoming_char);  //Print the incoming character to the terminal.
  }
  //If a character is coming from the terminal to the Arduino...
  if(Serial.available() >0)
  {
    incoming_char = Serial.read();  //Get the character coming from the terminal
    
    if(incoming_char == '*'){
      Send_SMS();  
    }else if(incoming_char == '%'){
      Send_Data();  
    }else{
      cell.print(incoming_char);    //Send the character to the cellular module.
    }
  }
  
  if(String_Received){
    
    if(Incoming_String.startsWith("OK")){
     Serial.print("OK Received..");   
    }else{
    Serial.print("Arduino Received: ");
    Serial.println(Incoming_String);
    }
    Incoming_String = "";
    String_Received = 0;  
  }
}

void Wait_For_String(){
  
  String_Received = 0;
  while(String_Received == 1){    
    if(cell.available() >0)
    {
      incoming_char=cell.read();    //Get the character from the cellular serial port.
      Incoming_String += incoming_char; 
      // Process message when new line character is recieved
      if (incoming_char == '\n')
      {
        if(Incoming_String == "\r\n"){
          Incoming_String = ""; 
        }else{ 
          String_Received = 1; 
        }
      }
    }  
  }
}

void Send_SMS(){
  
  Serial.println("Sending a SMS..."); 
     Serial.println("Sending AT+CMGF=1");
     cell.println("AT+CMGF=1");
     for(i=0; i<100;i++){
       if(cell.available() >0)
       { 
        incoming_char=cell.read();    //Get the character from the cellular serial port.
        Serial.print(incoming_char); 
       }
       delay(20);
     }
     Serial.println("Sending AT+CMGS=+94776141305");
     cell.println("AT+CMGS=+94776141305"); 
     
     cell.print("AT+CMGS=\""); // send the SMS number
     cell.print("+94776141305");
     cell.println("\"");
     
     
     for(i=0; i<100;i++){
       if(cell.available() >0)
       { 
        incoming_char=cell.read();    //Get the character from the cellular serial port.
        Serial.print(incoming_char); 
       }
       delay(20);
     }
     cell.print("Test SMS"); // SMS body
     cell.write(0x1A);
     cell.write(0x0D);
     cell.write(0x0A);
     Serial.println("Done");   
}

void Send_Data(){
  
  Serial.println("Sending Data..."); 
     Serial.print("Sending AT+SDATACONFIG=1,");
     Serial.print('"');
     Serial.print("TCP");
     Serial.print('"');
     Serial.print(',');
     Serial.print('"');
     Serial.print("112.134.231.85");
     Serial.print('"');
     Serial.println(",100");
     
     cell.print("Sending AT+SDATACONFIG=1,");
     cell.print('"');
     cell.print("TCP");
     cell.print('"');
     cell.print(',');
     cell.print('"');
     cell.print("112.134.231.85");
     cell.print('"');
     cell.println(",100");
     
     //cell.println("AT+SDATACONFIG=1,"TCP","112.134.231.85",100");
     for(i=0; i<100;i++){
       if(cell.available() >0)
       { 
        incoming_char=cell.read();    //Get the character from the cellular serial port.
        Serial.print(incoming_char); 
       }
       delay(20);
     }
     Serial.println("Sending AT+SDATASTART=1,1");
     cell.println("AT+SDATASTART=1,1"); 
     
     for(i=0; i<100;i++){
       if(cell.available() >0)
       { 
        incoming_char=cell.read();    //Get the character from the cellular serial port.
        Serial.print(incoming_char); 
       }
       delay(20);
     }
     
     Serial.println("Sending AT+SDATASTATUS=1");
     cell.println("AT+SDATASTATUS=1"); 
     
     for(i=0; i<100;i++){
       if(cell.available() >0)
       { 
        incoming_char=cell.read();    //Get the character from the cellular serial port.
        Serial.print(incoming_char); 
       }
       delay(20);
     }
     
     Serial.println("Sending AT+SDATATSEND=1,10");
     cell.println("AT+SDATATSEND=1,10");
     
     
     for(i=0; i<100;i++){
       if(cell.available() >0)
       { 
        incoming_char=cell.read();    //Get the character from the cellular serial port.
        Serial.print(incoming_char); 
       }
       delay(20);
     }
     cell.print("Test_Data"); // SMS body
     cell.write(0x1A);
     cell.write(0x0D);
     cell.write(0x0A);
     Serial.println("Done");   
}
