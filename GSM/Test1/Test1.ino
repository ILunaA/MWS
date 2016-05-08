#include <SoftwareSerial.h>
#include "GSM_Module.h"


String HOST      = "lahiruwije.ddns.net";   // Host name or address of the web server
int    PORT      = 80;

String Mobile_No = "+94776141305";          // Mobile Number which the SMS Sends 
String Test_SMS  = "Test SMS";              // Content of the SMS

int Status;

int temperature  = 42;
int humidity     = 20;

// For SIM900
GSM_Module Module(SIM900);            

// For SM5100B
//GSM_Module Module(SM5100B);

void setup()
{
  pinMode(SIM900_Power_Pin, OUTPUT);
//Initialize serial port for communication.
  Serial.begin(9600);
  
/*****************************************************************************************************************
    Initialize the module. 
*****************************************************************************************************************/
  Serial.println("Module Initializing..");
  
  Status = Module.Init(9600);
  
  if( Status == OK ){
    
    Serial.println("Module Ready.");
    
  }else{
    
    Serial.println("Module Initializing Failed."); 
    Serial.print("ERROR : ");
    Serial.println(Status);
    
    while(1){}; 
  }
  
  Module.Refresh();
  delay(10000);
  
/*****************************************************************************************************************/
}

void loop() {
   
/*****************************************************************************************************************
    Sending a test SMS 
*****************************************************************************************************************/
  Serial.println("Sending a Test SMS");
  Status = Module.Send_SMS(Mobile_No, Test_SMS);
  
  if( Status == OK ){
    
    Serial.println("SMS Sent");
    
  }else{
    
    Serial.print("SMS ERROR : ");
    Serial.println(Status);
    delay(2000);
    
  }

/*****************************************************************************************************************
    Connecting to the Server and Sending Data 
*****************************************************************************************************************/
//  Serial.println("Connecting to Server");
//  
//  Status = Module.POST_Data(HOST, PORT, temperature, humidity);
//  
//  if( Status == OK ){
//  
//    Serial.print("Connecting Successful");
//    Serial.print("    ");
//    Serial.println(Module.Received_String());
//    
//  }else{
//    
//    Serial.print("TCP ERROR Code : ");
//    Serial.print(Status);
//    Serial.print("    ");
//    Serial.println(Module.Received_String());
//    
//  }
//  
//// Listen to the Module print received data.   
//  
//  while(1){
//  
//    do{
//      
//        Module.Wait(1000);
//      
//      }while(!Module.String_Received());
//      
//      Serial.println(Module.Received_String());
//  
//  };
//
}



