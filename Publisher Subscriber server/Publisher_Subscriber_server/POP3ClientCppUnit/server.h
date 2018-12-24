#ifndef _SERVER_H_
#define _SERVER_H_

#include <fsm.h>
#include <fsmsystem.h>

#include "../kernel/stdMsgpc16pl16.h"
#include "NetFSM.h"

class server : public FiniteStateMachine {
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

	// FSM states
	enum serverStates { FSM_Server_Idle,
						FSM_Server_Add_Subscriber,
						FSM_Server_Add_Publisher,
						FSM_Server_Send_Data };			
public:
	server();
	~server();

	void Initialize();
	void Start();

	// Functions for each state
	
	// FSM_Server_Idle
	void FSM_Server_Idle_Receive_Msg();
	// FSM_Server_Add_Subscriber
	void FSM_Server_Add_Subscriber_Add_Sub();
	// FSM_Server_Add_Publisher
	void FSM_Server_Add_Publisher_Add_Pub();
	// FSM_Server_Send_Data
	void FSM_Server_Send_Data_To_Subs();
};

#endif /* _SERVE_H_ */