#include "Module_Codes.h"
#include <SoftwareSerial.h>
#include <avr/pgmspace.h>
#include <string.h>

#define Default_RX 2
#define Default_TX 3

class GSM_Module{
    public:
        GSM_Module(uint8_t Module_RX, uint8_t Module_TX);
        void Start(int baud); 
        void Listen();
        void Refresh(); 
        bool String_Received();
        bool Module_Waiting();
        String Received_String();
        String Decoded_String();
        void Wait(int time_out);
        void Read_Response();
        String Decode_Sys_Info();
        String Decode_GPRS_Info();
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
        int Activate_PDP();
        int Configure_TCP(String Host, int Port);
        int Start_TCP_Connection();
        int Query_Socket_Status();
        void Send_AT_Command(String command);    
        int ERROR_CODE(int Status, int Error);
        int Send_Data(String Data);
        int Send_SMS(String Number, String MSG);
        int TCP_Connect(String HOST, int PORT);
        int POST_Data(String HOST, int PORT, int temp, int hum);
             
};


