#ifndef _CH_AUTO_H_
#define _CH_AUTO_H_

#include <fsm.h>
#include <fsmsystem.h>
#include "../kernel/stdMsgpc16pl16.h"
#include "NetFSM.h"

//#pragma comment (lib, "Ws2_32.lib")

#define SERVER_PORT 27015	// Port number of server that will be used for communication with clients
#define BUFFER_SIZE 512		// Size of buffer that will be used for sending and receiving messages to clients
#define TOPIC_BUFFER_SIZE 50
#define FIRST_LETTER_OF_TOPIC 4
#define PUBLISHED_DATA 200
#define NUMBER_OF_TOPICS 20

class ChAuto : public FiniteStateMachine {
	
	// for FSM
	StandardMessage StandardMsgCoding;
	
	MessageInterface *GetMessageInterface(uint32 id);
	void	SetDefaultHeader(uint8 infoCoding);
	void	SetDefaultFSMData();
	void	NoFreeInstances();
	void	Reset();
	uint8	GetMbxId();
	uint8	GetAutomate();
	uint32	GetObject();
	void	ResetData();
	
	// FSM States
	enum	ChStates {	FSM_Ch_Idle, 
						FSM_Ch_Connecting,
						FSM_Ch_Connected, 
						FSM_Ch_Share_Data_on_Topic,
						FSM_Ch_Store_Data,
						FSM_Ch_Share_All_Data_State};

	//FSM_Ch_Idle
	void	FSM_Ch_Idle_Cl_Connection_Request();
	//FSM_Ch_Connecting
	void	FSM_Ch_Connecting_TIMER1_EXPIRED();
	void	FSM_Ch_Connecting_Sock_Connection_Acccept();
	//FSM_Ch_Connected
	void	FSM_Ch_Connected_Cl_MSG();
	void	FSM_Ch_Connected_Sock_MSG();
	void	FSM_Ch_Connected_Sock_Disconected();

	void	FSM_Ch_Idle_Cl_Connected();
	void	FSM_Ch_Idle_Cl_Failed();
	void	FSM_Ch_Share_All_Data();
	void	FSM_Ch_Share_Topic_Data();
	void	FSM_Ch_Store_Data_SubPub();
		
public:
	ChAuto();
	~ChAuto();
	
	//bool FSMMsg_2_NetMsg();
	void NetMsg_2_FSMMsg(const char* apBuffer, uint16 anBufferLength);

	void Initialize();
	void Start();
	void getTopic(char* serverData, char* currentTopicTmp);
	
	// Checks if ip address belongs to IPv4 address family
	bool is_ipV4_address();
protected:
	static DWORD WINAPI ClientListener(LPVOID);
	
	SOCKET m_Socket;
	HANDLE m_hThread;
	DWORD m_nThreadID;
	uint16 m_nMaxMsgSize;
};

#endif /* _CH_AUTO_H */