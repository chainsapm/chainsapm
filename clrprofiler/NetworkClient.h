#ifndef NETCLIENT
#define NETCLIENT

#pragma once
#include "ICommand.h"

class Cprofilermain;

enum NetworkCommands
{
	PING_SERVER,// 0x1
	PING_AGGREGATOR, 
	PING_AGENT,
	SEND_MODULENAME,
	SEND_CLASSNAME,
	SEND_FUNCTIONNAME
	
};

class NetworkClient
{
public:
	NetworkClient(Cprofilermain *profMain, std::wstring hostName, std::wstring port);
	~NetworkClient();
	static SOCKET m_SocketConnection;

private:
	// Create a singleton socket so we can control what happens if this
	// class is instantiated more than once.

	std::wstring m_HostName;
	std::wstring m_HostPort;

	// List of ICommand implementations by code
	std::map<short, ICommand> m_CommandList;

	// Double buffered queues for reading and writing so we don't bog down the enter/exit methods
	std::queue<std::shared_ptr<std::vector<char>>> m_OutboundQueueFront;
	std::queue<std::shared_ptr<std::vector<char>>> m_OutboundQueueBack;
	std::queue<std::shared_ptr<std::vector<char>>> m_InboundQueueFront;
	std::queue<std::shared_ptr<std::vector<char>>> m_InboundQueueBack;
	

	//Double buffered queues need double the locks
	CRITICAL_SECTION lockNetworkReader;
	CRITICAL_SECTION lockNetworkWriter;
	CRITICAL_SECTION lockMemoryReader;
	CRITICAL_SECTION lockMemoryWriter;

	// This is the main loop that will be used for sending and receving data. When a call comes in we will have a callback to a correct processor
	void ControllerLoop();

	// Completion routines
	LPWSAOVERLAPPED_COMPLETION_ROUTINE m_DataSent;
	LPWSAOVERLAPPED_COMPLETION_ROUTINE m_DataReceived;

	static void CALLBACK NewDataReceived(
		IN DWORD dwError,
		IN DWORD cbTransferred,
		IN LPWSAOVERLAPPED lpOverlapped,
		IN DWORD dwFlags
		);

	static void CALLBACK DataSent(
		IN DWORD dwError,
		IN DWORD cbTransferred,
		IN LPWSAOVERLAPPED lpOverlapped,
		IN DWORD dwFlags
		);

public:
	// Start the network client when we're ready.
	void Start();
	HRESULT SendCommand(std::shared_ptr<ICommand> packet);
	std::shared_ptr<ICommand> ReceiveCommand();
	HRESULT SendCommands(std::vector<std::shared_ptr<ICommand>> &packet);
	std::vector<std::shared_ptr<ICommand>>& ReceiveCommands();
	
};





#endif // !NETCLIENT
