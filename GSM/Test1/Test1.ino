#include <SoftwareSerial.h>
#include "GSM_Module.h"


String HOST      = "lahiruwije.ddns.net";
int    PORT      = 80;

String Mobile_No = "+94776141305";
String Test_SMS  = "Test SMS";

int Status;

int temperature  = 32;
int humidity     = 10;

GSM_Module Module(2,3);

void setup()
{
  //Initialize serial ports for communication.
  Serial.begin(9600);
  Module.Start(9600);
  Module.Refresh();
  
/*****************************************************************************************************************
    Waiting for the module to be ready. 
*****************************************************************************************************************/
Serial.println("Module Initializing..");
  do{
    
    do{
    
      Module.Wait(1000);
    
    }while(!Module.String_Received());
        
    Status = Module.Decode_Response();  
    
    Module.Refresh();
    
  }while(!(Module.SMS_Ready() && Module.Netwok_OK())); 
 
  Serial.println("Module Ready.");
  delay(1000);
  
/*****************************************************************************************************************/
}

void loop() {
   
/*****************************************************************************************************************
    Sending a test SMS 
*****************************************************************************************************************/
  Status = Module.Send_SMS(Mobile_No, Test_SMS);
  
  if( Status == OK ){
    
    Serial.println("SMS Sent");
    
  }else{
    
    Serial.print("SMS ERROR : ");
    Serial.println(Status);
    
  }

/*****************************************************************************************************************
    Posting test data to the server 
*****************************************************************************************************************/
  Serial.print("Posting Data to ");
  Serial.println(HOST);

  Status = Module.POST_Data(HOST, PORT, temperature, humidity);
  
  if( Status == OK ){
  
    Serial.print("POST Data Successful");
    
  }else{
    
    Serial.print("TCP ERROR Code : ");
    Serial.println(Status);
    
  }
  
  while(1){};

}



