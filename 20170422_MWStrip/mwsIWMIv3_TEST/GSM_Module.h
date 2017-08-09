#include "Module_Codes.h"
#include <SoftwareSerial.h>
#include <avr/pgmspace.h>
#include <string.h>

//UNO gnew board sim 900
#define Default_RX 10 //for Arduino Mega, so TX for GSM
#define Default_TX 11

//#define Default_RX 17 //RX2 Arduino Mega
//#define Default_TX 16 //TX2 Arduino Mega
//#define Default_RX 18 //RX1 Arduino Mega
//#define Default_TX 19 //TX1 Arduino Mega
//#define Default_RX 0 //RX0 Arduino Mega //creates pb at uploading
//#define Default_TX 1 //TX0 Arduino Mega //creates pb at uploading

class GSM_Module{
    public:
        GSM_Module(uint8_t Type);
        int Init(int baud); 
        void Listen();
        void Close();
        void Refresh(); 
        bool String_Received();
        bool Module_Waiting();
        String Received_String();
        String Decoded_String();
        void Wait(int time_out);
        void Read_Response();
        String Decode_Sys_Info();
        String Decode_GPRS_Info();
        String Decode_SIM900_GPRS_Info();
        String Decode_TCP_Info();
        int Decode_Response();
        bool OK_Received();
        bool Sys_Info_Received();
        bool GPRS_Info_Received();
        bool SMS_Ready();
        bool GPRS_Ready();
        bool Netwok_OK();
        bool Socket_Connecting();
        String Match_System_Code(int code);
        int Wait_For_Respond(int Response_Type, bool OK_Needed, bool *Flag, int Time_out);
        int Query_GPRS();
        int SIM900_Reset_IP();
        int SIM900_Check_IP_Stack();
        int SIM900_Single_Connection();
        int SIM900_Configure_APN(String APN, String USER_NAME, String PASSWARD);
        int Activate_PDP();
        int SIM900_Get_IP();
        int Configure_TCP(String Host, int Port);
        int Start_TCP_Connection();
        int Query_Socket_Status();
        void Send_AT_Command(String command);    
        int ERROR_CODE(int Status, int Error);
        int Send_Data(String Data);
        int Send_SMS(String Number, String MSG);
        int TCP_Connect(String HOST, int PORT);
        int Close_TCP_Connection();
        int POST_Data(String HOST, int PORT, int temp, int hum);
             
};


