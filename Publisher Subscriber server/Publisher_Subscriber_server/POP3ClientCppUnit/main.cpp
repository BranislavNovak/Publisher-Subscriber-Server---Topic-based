
#include <stdio.h>
#include <conio.h>

#include "const.h"
#include "fsmsystem.h"
#include "logfile.h"
#include "ChAuto.h"

bool g_ProgramEnd;

#define AUTOMAT_COUNT 3
#define MSGBOX_COUNT 3


/* FSM system instance. */
static FSMSystem sys(AUTOMAT_COUNT, MSGBOX_COUNT);

DWORD WINAPI SystemThread(void *data) {
	ChAuto Channel;
	
	/* Kernel buffer description block */
	/* number of buffer types */
	const uint8 buffClassNo =  1; 
	/* number of buffers of each buffer type */
	uint32 buffsCount[buffClassNo] = {50}; 
	/* buffer size for each buffer type */
 	uint32 buffsLength[buffClassNo] = { 256}; 
	
	/* Logging setting - to a file in this case */
	LogFile lf("log.log" /*log file name*/, "./log.ini" /* message translator file */);
	LogAutomateNew::SetLogInterface(&lf);

	/* Mandatory kernel initialization */
	printf("[*] Initializing system...\n");
	sys.InitKernel(buffClassNo, buffsCount, buffsLength, 1, Timer1s);

	/* Add automates to the system */
	sys.Add(&Channel, CH_AUTOMATE_TYPE_ID, 1, true);
	
	printf("[*] Starting system...\n");

	/* Start the first automate - usually it sends the first message, 
	since only automates can send messages */
	Channel.Start();

	/* Starts the system - dispatches system messages */
	sys.Start();

	/* Finish thread */
	return 0;
}

void mainProgram(int argc, char* argv[]) {
	
	DWORD thread_id;
	HANDLE thread_handle;

	/* Start operating thread. */
	thread_handle = CreateThread(NULL, 0, SystemThread, NULL, 0, &thread_id);

	/* Wait for end. */
	while(!g_ProgramEnd){}
	getch();
	

	/* Notify the system to stop - this causes the thread to finish */
	printf("[*] Stopping system...\n");
	sys.StopSystem();

	/* Free the thread handle */
	CloseHandle(thread_handle);
}