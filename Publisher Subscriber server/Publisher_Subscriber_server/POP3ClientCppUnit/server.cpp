#include <stdio.h>
#include <conio.h>
#include <time.h>

#include "const.h"
#include "server.h"

#define StandardMessageCoding 0x00

server::server() : FiniteStateMachine(SERVER_AUTOMATE_TYPE_ID, SERVER_AUTOMATE_MBX_ID, 1, 4, 2){
}

server::~server() {
}

uint8 server::GetAutomate(){
	return SERVER_AUTOMATE_TYPE_ID;
}

uint8 server::GetMbxId(){
	return SERVER_AUTOMATE_MBX_ID;
}

uint32 server::GetObject(){
	return GetObjectId();
}

MessageInterface *server::GetMessageInterface(uint32 id){
	return &StandardMsgCoding;
}

void server::SetDefaultHeader(uint8 infoCoding) {
	SetMsgInfoCoding(infoCoding);
	SetMessageFromData();
}

void server::SetDefaultFSMData() {
	SetDefaultHeader(StandardMessageCoding);
}

void server::NoFreeInstances() {
	printf("[%d] server::NoFreeInstances()\n", GetObjectId());
}

void server::Reset() {
	printf("[%d] server::Reset()\n", GetObjectId());
}


void server::Initialize(){
	SetState(FSM_Server_Idle);

	InitEventProc(FSM_Server_Idle, MSG_Notification, (PROC_FUN_PTR)&server::FSM_Server_Idle_Receive_Msg);
}

void server::Start(){
	PrepareNewMessage(0x00, MSG_Set_All);
	SetMsgToAutomate(SERVER_AUTOMATE_TYPE_ID);
	SetMsgObjectNumberTo(0);
	SendMessage(SERVER_AUTOMATE_MBX_ID);
}

void server::FSM_Server_Idle_Receive_Msg(){

}
