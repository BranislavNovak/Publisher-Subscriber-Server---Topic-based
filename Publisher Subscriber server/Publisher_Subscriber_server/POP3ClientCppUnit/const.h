#ifndef _CONST_H_
#define _CONST_H_

#include "./kernelTypes.h"

//#define D_TESTING

const uint8 CH_AUTOMATE_TYPE_ID = 0x00;
const uint8 SERVER_AUTOMATE_TYPE_ID = 0x01;

const uint8 CH_AUTOMATE_MBX_ID = 0x00;
const uint8 SERVER_AUTOMATE_MBX_ID = 0x01;

// channel messages
const uint16 MSG_Channel_Wait_Cl_Message				= 0x0001;
const uint16 MSG_Channel_Connection_Failed				= 0x0002;
const uint16 MSG_Sock_Connection_Acccept			= 0x0003;
const uint16 MSG_Cl_MSG								= 0x0004;
const uint16 MSG_Sock_MSG							= 0x0005;
const uint16 MSG_Disconnect_Request					= 0x0006;
const uint16 MSG_Sock_Disconected					= 0x0007;
const uint16 MSG_Sock_Disconnecting_Conf			= 0x0008;

// client messages
const uint16 MSG_User_Check_Mail		= 0x0009;
const uint16 MSG_Cl_Connection_Reject	= 0x000a;
const uint16 MSG_Cl_Connection_Accept	= 0x000b;
const uint16 MSG_User_Name_Password		= 0x000c;
const uint16 MSG_MSG					= 0x000d;
const uint16 MSG_Cl_Disconected			= 0x000f;

// user messages
const uint16 MSG_Set_All				= 0x0010;
const uint16 MSG_User_Connected			= 0x0011;
const uint16 MSG_User_Connecton_Fail	= 0x0012;
const uint16 MSG_Mail					= 0x0013;
const uint16 MSG_User_Save_Mail			= 0x0015;
const uint16 MSG_User_Disconnected		= 0x0014;


// server messages
const uint16 MSG_Notification				= 0x0016;	//First message from chAuto
const uint16 MSG_Stay_Connected				= 0x0017;
const uint16 MSG_Share_Data_on_Topic		= 0x0018;
const uint16 MSG_Store_Data_on_Topic		= 0x0019;
const uint16 MSG_Share_All_Data				= 0x0020;
const uint16 MSG_Client_Message				= 0x0021;

#define ADRESS "localhost"
//#define ADRESS "mail.spymac.com"
//#define ADRESS "krtlab8"
#define PORT 27015 //110

#define TIMER1_ID 0
#define TIMER1_COUNT 10
#define TIMER1_EXPIRED 0x20

#define PARAM_DATA 0x01
#define PARAM_Name 0x02
#define PARAM_Pass 0x03


#endif //_CONST_H_