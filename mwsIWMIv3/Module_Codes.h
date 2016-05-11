/*****************************************************************************************************************

*****************************************************************************************************************/
#define ERROR_RESPONSE_TIMED_OUT       -1
#define ERROR_CME                      -2
#define ERROR_CMS                      -3
#define ERROR_WRONG_RESPONCE_TYPE      -4
#define UNKNOWN_RESPONSE               -5
#define ECHO                           -6

#define ERROR_GPRS_NOT_ATTACHED        -10
#define ERROR_IP_RESET_FAILED          -20
#define ERROR_IP_STACK_FAILED          -30
#define ERROR_SINGLE_CONNECTION_FAILED -40
#define ERROR_APN_SETTING_FAILED       -50
#define ERROR_PDP_ACTIVATION_FAILED    -60
#define ERROR_COULD_NOT_GET_IP         -70
#define ERROR_TCP_CONFIG_FAILED        -80
#define ERROR_TCP_START_FAILED         -90
#define ERROR_TCP_CONNECTION_FAILED    -100

#define ERROR_SMS_MODE_NOT_SELECTED    -10
#define ERROR_SMS_NUMBER_NOT_SET       -20

#define OK                              1

#define TYPE_SYS_INFO                   2
#define TYPE_GPRS_INFO                  3
#define TYPE_TCP_INFO                   4
#define WAITING_FOR_DATA                5
#define SIM900_GPRS_INFO                6

#define SIM900                          2
#define SM5100B                         3

//#define SIM900_Power_Pin                7
//new board sim 900
#define SIM900_Power_Pin                9

/*****************************************************************************************************************

*****************************************************************************************************************/

const char Sys_Codes_0[3]        = "S1";                  //SEARCHING SIM";
const char Sys_Codes_1[3]        = "S2";                  //SIM INSERTED";
const char Sys_Codes_3[3]        = "S3";                  //CALL READY";
const char Sys_Codes_4[3]        = "S4";                  //SMS READY";
const char Sys_Codes_7[3]        = "S5";                  //NO SIM - ERROR";     // Assumption
const char Sys_Codes_10[3]       = "S6";                  //NO SIM FOUND";
const char Sys_Codes_11[3]       = "S7";                  //REGISTERED TO NETWORK";
const char Sys_Codes_12[3]       = "S8";                  //SIM READY";

const char Unknown_code[3]       = "UC";                  //UNKNOWN CODE";

const char GPRS_Info_0[3]        = "G1";                  //GPRS NOT ATTACHED";
const char GPRS_Info_1[3]        = "G2";                  //GPRS ATTACHED";

const char TCP_Info_0[3]         = "T1";                  //CONNECTING";
const char TCP_Info_1[3]         = "T2";                  //CONNECTED";
const char TCP_Info_2[3]         = "T3";                  //CONNECTION FAILED";

const char No_Response[3]        = "NR";                  //NO RESPONSE";

const char CME_Error[3]          = "CE";                  //CME ERROR";
const char CMS_Error[3]          = "CS";                  //CMS ERROR";

const char SIM900_GPRS_Info_0[3] = "90";
const char SIM900_GPRS_Info_1[3] = "91";

const char AT_CMD_Query_GPRS[7]      = "CGATT?";
const char AT_CMD_Activate_PDP[10]   = "CGACT=1,1";
const char AT_CMD_TCP_Config[13]     = "SDATACONF=1,";
const char AT_CMD_Start_TCP[15]      = "SDATASTART=1,1";
const char AT_CMD_Query_Socket[14]   = "SDATASTATUS=1";
const char AT_CMD_Send_Data[14]      = "SDATATSEND=1,";
const char AT_CMD_SMS_Mode[7]        = "CMGF=1";
const char AT_CMD_SMS_Number[8]      = "CMGS=\"";

const char AT_CMD_SIM900_Reset_IP[8] = "CIPSHUT";
const char AT_CMD_SIM900_IP_Stack[10]= "CIPSTATUS";
const char AT_CMD_S9_Single_Conn[9]  = "CIPMUX=0";
const char AT_CMD_S9_APN_Config[7]   = "CSTT= ";
const char AT_CMD_S9_Activate_PDP[6] = "CIICR";
const char AT_CMD_S9_TCP_Config[10]  = "CIPSTART=";
const char AT_CMD_S9_Get_IP[6]       = "CIFSR";
const char AT_CMD_S9_Send_Data[8]    = "CIPSEND";

const char POST_Data_Line_1[25]      = "POST /add.php HTTP/1.1\r\n"; 
const char POST_Data_Line_2[7]       = "Host: ";  
const char POST_Data_Line_3[52]      = "\r\nContent-Type: application/x-www-form-urlencoded\r\n"; //49
const char POST_Data_Line_4[19]      = "Content-Length: "; //20
const char POST_Data_Line_5_1[11]    = "\r\n\r\ntemp1="; //16
const char POST_Data_Line_5_2[7]     = "&hum1="; //16



