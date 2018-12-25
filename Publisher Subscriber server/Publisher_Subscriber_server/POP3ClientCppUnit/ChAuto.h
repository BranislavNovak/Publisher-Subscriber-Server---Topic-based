#ifndef _CH_AUTO_H_
#define _CH_AUTO_H_

#include <fsm.h>
#include <fsmsystem.h>
#include "../kernel/stdMsgpc16pl16.h"
#include "NetFSM.h"

#define SERVER_PORT 27015	// Port number of server that will be used for communication with clients
#define BUFFER_SIZE 1024		// Size of buffer that will be used for sending and receiving messages to clients
#define TOPIC_BUFFER_SIZE 50
#define FIRST_LETTER_OF_TOPIC 4
#define PUBLISHED_DATA 150
#define NUMBER_OF_TOPICS 20
#define MAX_PUBLISHERS 10

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
						FSM_Ch_Connected, 
						FSM_Ch_Share_Data_on_Topic,
						FSM_Ch_Store_Data,
						FSM_Ch_Share_All_Topics_State,
						FSM_Ch_Publish_ACK_State};

	void	FSM_Ch_Idle_Cl_Connected();
	void	FSM_Ch_Idle_Cl_Failed();
	void	FSM_Ch_Share_All_Topics();
	void	FSM_Ch_Share_Topic_Data();
	void	FSM_Ch_Store_Data_SubPub();
	void	FSM_Ch_Publish_ACK();
		
public:
	ChAuto();
	~ChAuto();
	
	void Initialize();
	void Start();
	void getTopic(char* serverData, char* currentTopicTmp);

};

#endif /* _CH_AUTO_H */