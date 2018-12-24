#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <map>
#include <string>
#include <iterator>
#include "conio.h"

#include "const.h"
#include "ChAuto.h"

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

// Buffer we will use to send and receive clients' messages
char dataBuffer[BUFFER_SIZE];
// WSADATA data structure that is to receive details of the Windows Sockets implementation
WSADATA wsaData;
// Declare and initialize client address that will be set from recvfrom
sockaddr_in6 clientAddress;
// Server address 
sockaddr_in6 serverAddress; 
char client_message[BUFFER_SIZE];
unsigned short clientPort;

int iResult;
SOCKET serverSocket;
int publisherCounter = 3;
int subscriberCounter = 0;

char topic1[PUBLISHED_DATA] = "WEATHER: Belgrace 8C  New York 3C  London 9C  Tokyo 5C  Moscow -9C";
char topic2[PUBLISHED_DATA] = "FOOTBALL: France 4:2 Croatia  Belgium 2:0 England  Germany 2:1 Sweden  Japan 2:2 Senegal";
char topic3[PUBLISHED_DATA] = "INTEL_CPU: i3=180$  i5=290$  i7=430$  i9=1480$";
char topic4[PUBLISHED_DATA] = "COLLEGES_IN_AMERICA: Stanford  Harvard  Princeton  Yale  Duke";
char* allServerData[NUMBER_OF_TOPICS] = {topic1, topic2, topic3, topic4};

#define StandardMessageCoding 0x00

ChAuto::ChAuto() : FiniteStateMachine(CH_AUTOMATE_TYPE_ID, CH_AUTOMATE_MBX_ID, 1, 6, 3) {
}

ChAuto::~ChAuto() {
}

uint8 ChAuto::GetAutomate() {
	return CH_AUTOMATE_TYPE_ID;
}

/* This function actually connnects the ChAuto with the mailbox. */
uint8 ChAuto::GetMbxId() {
	return CH_AUTOMATE_MBX_ID;
}

uint32 ChAuto::GetObject() {
	return GetObjectId();
}

MessageInterface *ChAuto::GetMessageInterface(uint32 id) {
	return &StandardMsgCoding;
}

void ChAuto::SetDefaultHeader(uint8 infoCoding) {
	SetMsgInfoCoding(infoCoding);
	SetMessageFromData();
}

void ChAuto::SetDefaultFSMData() {
	SetDefaultHeader(StandardMessageCoding);
}

void ChAuto::NoFreeInstances() {
	printf("[%d] ChAuto::NoFreeInstances()\n", GetObjectId());
}

void ChAuto::Reset() {
	printf("[%d] ChAuto::Reset()\n", GetObjectId());
}


void ChAuto::Initialize() {
	SetState(FSM_Ch_Idle);	
	
	//intitialization message handlers
	InitEventProc(FSM_Ch_Idle ,MSG_Channel_Wait_Cl_Message, (PROC_FUN_PTR)&ChAuto::FSM_Ch_Idle_Cl_Connected);
	InitEventProc(FSM_Ch_Idle ,MSG_Channel_Connection_Failed, (PROC_FUN_PTR)&ChAuto::FSM_Ch_Idle_Cl_Failed);
	InitEventProc(FSM_Ch_Connected ,MSG_Stay_Connected, (PROC_FUN_PTR)&ChAuto::FSM_Ch_Idle_Cl_Connected);
	InitEventProc(FSM_Ch_Connected ,MSG_Store_Data_on_Topic, (PROC_FUN_PTR)&ChAuto::FSM_Ch_Store_Data_SubPub);
	InitEventProc(FSM_Ch_Store_Data ,MSG_Share_Data_on_Topic, (PROC_FUN_PTR)&ChAuto::FSM_Ch_Share_Topic_Data);
	InitEventProc(FSM_Ch_Store_Data ,MSG_Share_All_Data, (PROC_FUN_PTR)&ChAuto::FSM_Ch_Share_All_Data);
	InitEventProc(FSM_Ch_Share_Data_on_Topic ,MSG_Stay_Connected, (PROC_FUN_PTR)&ChAuto::FSM_Ch_Idle_Cl_Connected);

	InitTimerBlock(TIMER1_ID, TIMER1_COUNT, TIMER1_EXPIRED);
}

void ChAuto::Start(){
	SetState(FSM_Ch_Idle);

	// Initialize windows sockets library for this process
    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0)
    {
        printf("WSAStartup failed with error: %d\n", WSAGetLastError());
        return;
    }

    // Initialize serverAddress structure used by bind function
	memset((char*)&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin6_family = AF_INET6; 			// set server address protocol family
    serverAddress.sin6_addr = in6addr_any;			// use all available addresses of server
    serverAddress.sin6_port = htons(SERVER_PORT);	// Set server port
	serverAddress.sin6_flowinfo = 0;				// flow info

    // Create a socket 
    serverSocket = socket(AF_INET6,      // IPv6 address famly
								 SOCK_DGRAM,   // datagram socket
								 IPPROTO_UDP); // UDP

	// Check if socket creation succeeded
    if (serverSocket == INVALID_SOCKET)
    {
        printf("Creating socket failed with error: %d\n", WSAGetLastError());
        WSACleanup();
		
		PrepareNewMessage(0x00, MSG_Channel_Connection_Failed);
		SetMsgToAutomate(CH_AUTOMATE_TYPE_ID);
		SetMsgObjectNumberTo(0);
		SendMessage(CH_AUTOMATE_MBX_ID);
        return;
    }
	
	// Disable receiving only IPv6 packets. We want to receive both IPv4 and IPv6 packets.
	char no = 0;     
	iResult = setsockopt(serverSocket, IPPROTO_IPV6, IPV6_V6ONLY, &no, sizeof(no)); 
	
	if (iResult == SOCKET_ERROR) 
			printf("failed with error: %u\n", WSAGetLastError());


    // Bind server address structure (type, port number and local address) to socket
    iResult = bind(serverSocket,(SOCKADDR *)&serverAddress, sizeof(serverAddress));

	// Check if socket is succesfully binded to server datas
    if (iResult == SOCKET_ERROR)
    {
        printf("Socket bind failed with error: %d\n", WSAGetLastError());
        closesocket(serverSocket);
        WSACleanup();

		PrepareNewMessage(0x00, MSG_Channel_Connection_Failed);
		SetMsgToAutomate(CH_AUTOMATE_TYPE_ID);
		SetMsgObjectNumberTo(0);
		SendMessage(CH_AUTOMATE_MBX_ID);
        return;
    }

	printf("Simple UDP server waiting client messages.\n");	

	PrepareNewMessage(0x00, MSG_Channel_Wait_Cl_Message);
	SetMsgToAutomate(CH_AUTOMATE_TYPE_ID);
	SetMsgObjectNumberTo(0);
	SendMessage(CH_AUTOMATE_MBX_ID);
}

void ChAuto::FSM_Ch_Idle_Cl_Connected(){
	 
	SetState(FSM_Ch_Connected);

	memset(&clientAddress, 0, sizeof(clientAddress));

	// Set whole buffer to zero
    memset(dataBuffer, 0, BUFFER_SIZE);

	// size of client address
	int sockAddrLen = sizeof(clientAddress);

	// Receive client message
    iResult = recvfrom(serverSocket,						// Own socket
			            dataBuffer,							// Buffer that will be used for receiving message
						BUFFER_SIZE,							// Maximal size of buffer
						0,									// No flags
						(struct sockaddr *)&clientAddress,	// Client information from received message (ip address and port)
						&sockAddrLen);						// Size of sockadd_in structure
		
	// Check if message is succesfully received
	if (iResult == SOCKET_ERROR)
	{
		printf("recvfrom failed with error: %d\n", WSAGetLastError());
	}

    char ipAddress[INET6_ADDRSTRLEN]; // INET6_ADDRSTRLEN 65 spaces for hexadecimal notation of IPv6
		
	// Copy client ip to local char[]
	inet_ntop(clientAddress.sin6_family, &clientAddress.sin6_addr, ipAddress, sizeof(ipAddress));
        
	// Convert port number from network byte order to host byte order
    clientPort = ntohs(clientAddress.sin6_port);

	printf("IPv6 Client connected from ip: %s, port: %d, sent: %s.\n", ipAddress, clientPort, dataBuffer);
		    

	PrepareNewMessage(0x00, MSG_Store_Data_on_Topic);
	SetMsgToAutomate(CH_AUTOMATE_TYPE_ID);
	SetMsgObjectNumberTo(0);
	SendMessage(CH_AUTOMATE_MBX_ID);
}

void ChAuto::FSM_Ch_Store_Data_SubPub(){
	SetState(FSM_Ch_Store_Data);

	char dataFromClient[BUFFER_SIZE];
	char currentTopic[TOPIC_BUFFER_SIZE];
	char publishMessageTmp[BUFFER_SIZE];

	strcpy(dataFromClient, dataBuffer);
	
	
	if(dataFromClient[0] == 'P' && dataFromClient[1] == 'U' && dataFromClient[2] == 'B'){
		int i = FIRST_LETTER_OF_TOPIC;	
		int j = 0, k = 0;
		while(dataFromClient[i] != ' '){
			currentTopic[j++] = dataFromClient[i++];
		}
		j = 0;

		while(dataFromClient[i] != '\0'){
			publishMessageTmp[j++] = dataFromClient[i++];
		}

		for(j = 0; j < publisherCounter; j++){
			char topicTmp[TOPIC_BUFFER_SIZE];
			getTopic(allServerData[j], topicTmp);

			if(!strcmp(currentTopic, topicTmp)){
				printf("Topic already exists!\n");
			}
		}

		char tmpOnCurrentTopicData[PUBLISHED_DATA];
		int tmp = 0;
		for(j = 0; j < strlen(currentTopic); j++){
			tmpOnCurrentTopicData[tmp++] = currentTopic[j];
		}
		tmpOnCurrentTopicData[tmp++] = ':';
		tmpOnCurrentTopicData[tmp++] = ' ';

		for(j = 0; j < strlen(publishMessageTmp); j++){
			tmpOnCurrentTopicData[tmp++] = publishMessageTmp[j];
		}

		allServerData[publisherCounter++] = tmpOnCurrentTopicData;

	}else if(dataFromClient[0] == 'S' && dataFromClient[1] == 'U' && dataFromClient[2] == 'B'){
		printf("Here I will send data on currentTopic!\n");

	}else if(dataFromClient[0] == 'L' && dataFromClient[1] == 'I' && dataFromClient[2] == 'S' && dataFromClient[3] == 'T'){
		printf("Here I will send whole list of topics!\n");

	}

	PrepareNewMessage(0x00, MSG_Share_Data_on_Topic);
	SetMsgToAutomate(CH_AUTOMATE_TYPE_ID);
	SetMsgObjectNumberTo(0);
	SendMessage(CH_AUTOMATE_MBX_ID);
}

void ChAuto::FSM_Ch_Share_Topic_Data(){
	SetState(FSM_Ch_Share_Data_on_Topic);

	iResult = sendto(serverSocket,						// Own socket
						 dataBuffer,						// Text of message
						 strlen(dataBuffer),				// Message size
						 0,									// No flags
						 (SOCKADDR *)&clientAddress,		// Address structure of server (type, IP address and port)
						 sizeof(clientAddress));			// Size of sockadr_in structure

		// Check if message is succesfully sent. If not, close client application
		if (iResult == SOCKET_ERROR)
		{
			printf("sendto failed with error: %d\n", WSAGetLastError());
			closesocket(serverSocket);
			WSACleanup();
			return;
		}

	PrepareNewMessage(0x00, MSG_Stay_Connected);
	SetMsgToAutomate(CH_AUTOMATE_TYPE_ID);
	SetMsgObjectNumberTo(0);
	SendMessage(CH_AUTOMATE_MBX_ID);
}


void ChAuto::FSM_Ch_Share_All_Data(){
	SetState(FSM_Ch_Share_All_Data_State);
}

void ChAuto::FSM_Ch_Idle_Cl_Failed(){
	SetState(FSM_Ch_Idle);
}

void ChAuto::FSM_Ch_Idle_Cl_Connection_Request(){
}



void ChAuto::FSM_Ch_Connecting_TIMER1_EXPIRED(){

	/*PrepareNewMessage(0x00, MSG_Cl_Connection_Reject);
	SetMsgToAutomate(CL_AUTOMATE_TYPE_ID);
	SetMsgObjectNumberTo(0);
	SendMessage(CL_AUTOMATE_MBX_ID);

	SetState(FSM_Ch_Idle);*/

}
void ChAuto::FSM_Ch_Connecting_Sock_Connection_Acccept(){

	/*PrepareNewMessage(0x00, MSG_Cl_Connection_Accept);
	SetMsgToAutomate(CL_AUTOMATE_TYPE_ID);
	SetMsgObjectNumberTo(0);
	SendMessage(CL_AUTOMATE_MBX_ID);

	StopTimer(TIMER1_ID);

	SetState(FSM_Ch_Connected);*/

}
void ChAuto::FSM_Ch_Connected_Cl_MSG(){

	char* data = new char[255];
	uint8* buffer = GetParam(PARAM_DATA);
	uint16 size = buffer[2];

	memcpy(data,buffer + 4,size);

	data[size] = 0;
	if (send(m_Socket, data, size, 0) != size) {
		delete [] data;
	} else {
		printf("SENT: %s",data);
		delete [] data;
	}

}
void ChAuto::FSM_Ch_Connected_Sock_MSG(){

}

void ChAuto::FSM_Ch_Connected_Sock_Disconected(){

	/*PrepareNewMessage(0x00, MSG_Cl_Disconected);
	SetMsgToAutomate(CL_AUTOMATE_TYPE_ID);
	SetMsgObjectNumberTo(0);
	SendMessage(CL_AUTOMATE_MBX_ID);

	SetState(FSM_Ch_Idle);*/

}

void ChAuto::NetMsg_2_FSMMsg(const char* apBuffer, uint16 anBufferLength) {
	
	/*PrepareNewMessage(0x00, MSG_MSG);
	SetMsgToAutomate(CL_AUTOMATE_TYPE_ID);
	SetMsgObjectNumberTo(0);
	AddParam(PARAM_DATA,anBufferLength,(uint8 *)apBuffer);
	SendMessage(CL_AUTOMATE_MBX_ID);*/
	
}

DWORD ChAuto::ClientListener(LPVOID param) {
	return 1;
}

void ChAuto::getTopic(char* serverData, char* currentTopicTmp){
	int i = 0, j = 0;
	while(serverData[i] != ':'){
		currentTopicTmp[j++] = serverData[i++];
	}
}