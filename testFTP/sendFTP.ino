int8_t answer;
int onModulePin = 7;
char aux_str[30];

char incoming_data[120];

char test_str[ ]= "lon=0.000000,lat=0.000000,altitude=0.00,sats=0,date=2014-05-09,time=10:32:56,RTCdate=2000-01-01,RTCtime=00:00:00,winddir=-1,windspeedms=nan,windgustms=0.0,windgustdir=0,windspdms_avg2m=0.0,winddir_avg2m=0,windgustms_10m=0.0,windgustdir_10m=0,humidity=72.3,tempc=32.1,rainhourmm=0.00,raindailymm=0.00,rainindicate=0,pressure=100731.00,batt_lvl=4.20,light_lvl=0.07";

int data_size, aux;


void setup(){

  pinMode(onModulePin, OUTPUT);
  Serial.begin(115200);  


  Serial.println("Starting...");
  power_on();

  delay(5000);

  Serial.println("Connecting to the network...");

  while( (sendATcommand("AT+CREG?", "+CREG: 0,1", 500) 
    || sendATcommand("AT+CREG?", "+CREG: 0,5", 500)) == 0 );

  configure_FTP();
  uploadFTP();

  Serial.print("Incoming data: ");
  Serial.println(incoming_data);
}


void loop(){

}


void configure_FTP(){
  sendATcommand("AT+SAPBR=3,1,\"Contype\",\"GPRS\"", "OK", 2000);
  sendATcommand("AT+SAPBR=3,1,\"APN\",\"APN\"", "OK", 2000);
  sendATcommand("AT+SAPBR=3,1,\"USER\",\"user_name\"", "OK", 2000);
  sendATcommand("AT+SAPBR=3,1,\"PWD\",\"password\"", "OK", 2000);

  while (sendATcommand("AT+SAPBR=1,1", "OK", 20000) != 1);
  sendATcommand("AT+FTPCID=1", "OK", 2000);
  sendATcommand("AT+FTPSERV=\"ftp.lk.iwmi.org\"", "OK", 2000);
  sendATcommand("AT+FTPPORT=21", "OK", 2000);
  sendATcommand("AT+FTPUN=\"iwmiftpuser\"", "OK", 2000);
  sendATcommand("AT+FTPPW=\"ii3mu$ft%e_2\"", "OK", 2000);
}


void uploadFTP(){
  sendATcommand("AT+FTPPUTNAME=\"AWS_test.csv\"", "OK", 2000);
  sendATcommand("AT+FTPPUTPATH=\"/Yann\"", "OK", 2000);
  if (sendATcommand("AT+FTPPUT=1", "+FTPPUT:1,1,", 30000) == 1){
    data_size = 0;
    while(Serial.available()==0);
    aux = Serial.read();
    do{
      data_size *= 10;
      data_size += (aux-0x30);
      while(Serial.available()==0);
      aux = Serial.read();        
    } 
    while(aux != 0x0D);

    if (data_size >= 100){
      if (sendATcommand("AT+FTPPUT=2,100", "+FTPPUT:2,100", 30000) == 1){
        Serial.println(sendATcommand(test_str, "+FTPPUT:1,1", 30000), DEC);          
        Serial.println(sendATcommand("AT+FTPPUT=2,0", "+FTPPUT:1,0", 30000), DEC);
        Serial.println("Upload done!!");
      } 
      else {
        sendATcommand("AT+FTPPUT=2,0", "OK", 30000);                    
      }
    } 
    else {
      sendATcommand("AT+FTPPUT=2,0", "OK", 30000); 
    }
  } 
  else {
    Serial.println("Error openning the FTP session");
  }
}




void power_on(){

  uint8_t answer=0;

  // checks if the module is started
  answer = sendATcommand("AT", "OK", 2000);
  if (answer == 0)
  {
    digitalWrite(onModulePin,HIGH);
    delay(3000);
    digitalWrite(onModulePin,LOW);

    while(answer == 0){     // Send AT every two seconds and wait for the answer
      answer = sendATcommand("AT", "OK", 2000);    
    }
  }
}


int8_t sendATcommand(char* ATcommand, char* expected_answer, unsigned int timeout){

  uint8_t x=0,  answer=0;
  char response[100];
  unsigned long previous;

  memset(response, '\0', 100);    // Initialize the string

  delay(100);

  while( Serial.available() > 0) Serial.read();    // Clean the input buffer

  Serial.println(ATcommand);    // Send the AT command 


    x = 0;
  previous = millis();

  // this loop waits for the answer
  do{
    if(Serial.available() != 0){    
      // if there are data in the UART input buffer, reads it and checks for the asnwer
      response[x] = Serial.read();
      //Serial.print(response[x]);
      x++;
      // check if the desired answer  is in the response of the module
      if (strstr(response, expected_answer) != NULL)    
      {
        answer = 1;
      }
    }
  }
  // Waits for the asnwer with time out
  while((answer == 0) && ((millis() - previous) < timeout));    

  return answer;
}

int8_t sendATcommand2(char* ATcommand, char* expected_answer1, 
char* expected_answer2, unsigned int timeout){

  uint8_t x=0,  answer=0;
  char response[100];
  unsigned long previous;

  memset(response, '\0', 100);    // Initialize the string

  delay(100);

  while( Serial.available() > 0) Serial.read();    // Clean the input buffer

  Serial.println(ATcommand);    // Send the AT command 


    x = 0;
  previous = millis();

  // this loop waits for the answer
  do{ 
    // if there are data in the UART input buffer, reads it and checks for the asnwer
    if(Serial.available() != 0){               
      response[x] = Serial.read();
      x++;
      // check if the desired answer 1 is in the response of the module
      if (strstr(response, expected_answer1) != NULL)    
      {
        answer = 1;
      }
      // check if the desired answer 2 is in the response of the module
      if (strstr(response, expected_answer2) != NULL)    
      {
        answer = 2;
      }
    }
    // Waits for the asnwer with time out
  }
  while((answer == 0) && ((millis() - previous) < timeout));    

  return answer;
}

