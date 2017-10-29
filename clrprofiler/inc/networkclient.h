#ifndef NETCLIENT
#define NETCLIENT

#pragma once
#include "INetworkClient.h"
#include "commands\commandprocessor.h"
#include "critsec_helper.h"

#define _SECOND ((__int64) 10000000)
#define _MINUTE (60 * _SECOND)
#define _HOUR   (60 * _MINUTE)
#define _DAY    (24 * _HOUR)

enum NetworkCommands
{
	PING_SERVER,// 0x1
	PING_AGGREGATOR,
	PING_AGENT,
	SEND_MODULENAME,
	SEND_CLASSNAME,
	SEND_FUNCTIONNAME

};


class NetworkClient : public INetworkClient
{
public:
	NetworkClient(std::wstring hostName, std::wstring port, 
		std::shared_ptr<CommandProcessor> commandProc);
	~NetworkClient();
	static SOCKET m_SocketConnection;
	PTP_IO m_ptpIO;
	void SetThreadPoolIO(PTP_IO ptpIO);
	std::vector<char> OverflowBuffer;
	std::map<short, std::shared_ptr<Commands::ICommand>> m_CommandList;
	static VOID CALLBACK TimerRoutine(PVOID lpParam, BOOLEAN TimerOrWaitFired);
	static VOID CALLBACK TimerRoutine2(PVOID lpParam, BOOLEAN TimerOrWaitFired);

private:
	// Create a singleton socket so we can control what happens if this
	// class is instantiated more than once.

	std::wstring m_HostName;
	std::wstring m_HostPort;

	// List of ICommand implementations by code
	

	// Double buffered queues for reading and writing so we don't bog down the enter/exit methods
	std::queue<std::shared_ptr<Commands::ICommand>> m_OutboundQueueFront;
	std::queue<std::shared_ptr<Commands::ICommand>> m_OutboundQueueBack;
	std::vector<std::shared_ptr<Commands::ICommand>> m_DelayedRemovalQueue;
	std::queue<std::shared_ptr<std::vector<char>>> m_InboundQueueFront;
	std::queue<std::shared_ptr<std::vector<char>>> m_InboundQueueBack;


	//Double buffered queues need double the locks
	CRITICAL_SECTION FrontOutboundLock;
	CRITICAL_SECTION BackOutboundLock;
	CRITICAL_SECTION FrontInboundLock;
	CRITICAL_SECTION BackInboundLock;
	CRITICAL_SECTION OverflowBufferLock;

	PTP_TIMER recvTimer;

	PTP_TIMER sendTimer;

	PTP_WAIT dataReceived;

	std::shared_ptr<CommandProcessor> CommandProc;

	static VOID CALLBACK SendTimerCallback(
		PTP_CALLBACK_INSTANCE pInstance, // See "Callback Termination Actions" section
		PVOID pvContext,
		PTP_TIMER pTimer);

	static VOID CALLBACK ReceiveTimerCallback(
		PTP_CALLBACK_INSTANCE pInstance, // See "Callback Termination Actions" section
		PVOID pvContext,
		PTP_TIMER pTimer);

	static VOID CALLBACK DataReceivedCallback(
		PTP_CALLBACK_INSTANCE Instance,
		PVOID                 Context,
		PTP_WAIT              Wait,
		TP_WAIT_RESULT        WaitResult);

	bool insideSendLock;
	bool insideReceiveLock;


public:

	// Start the network client when we're ready.
	void Start();
	// Shutdown the socket and stop send and recv.
	void Shutdown();

	HRESULT SendCommand(Commands::ICommand* packet);


	// Send a single command to the buffer to be processed.
	HRESULT SendCommand(std::shared_ptr<Commands::ICommand> packet);

	std::shared_ptr<Commands::ICommand> ReceiveCommand();

	HRESULT SendCommands(std::vector<Commands::ICommand*> &packet);

	std::vector<std::shared_ptr<Commands::ICommand>> ReceiveCommands();

	static HANDLE DataReceived;
	static HANDLE DataSent;
	static HANDLE DataToBeSent;


	static VOID CALLBACK IoCompletionCallback(
		_Inout_     PTP_CALLBACK_INSTANCE Instance,
		_Inout_opt_ PVOID                 Context,
		_Inout_opt_ PVOID                 Overlapped,
		_In_        ULONG                 IoResult,
		_In_        ULONG_PTR             NumberOfBytesTransferred,
		_Inout_     PTP_IO                Io
		);

	void AddItemsToBackBuffer(unsigned int term, char * &iterBuff, const DWORD &totalBuffSize);
};


struct NetClietCallback : OVERLAPPED
{
	HANDLE rstHandle;
	NetworkClient *netclient;
	std::vector<std::shared_ptr<std::vector<char>>> *sendqueue;
	LPWSABUF queue;
	enum _direction
	{
		SEND,
		RECV
	} Direction;
};


#endif // !NETCLIENT
