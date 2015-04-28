#include "stdafx.h"
#include <WinSock2.h>
#include <ws2tcpip.h>
#include "profilermain.h"
#include "NetworkClient.h"
#include "SendString.h"

// Initialize the socket to NULL.
SOCKET NetworkClient::m_SocketConnection = NULL;

//NetworkClient::NetworkClient()
//{
//}

void CALLBACK NetworkClient::NewDataReceived(
	IN DWORD dwError,
	IN DWORD cbTransferred,
	IN LPWSAOVERLAPPED lpOverlapped,
	IN DWORD dwFlags
	)
{
	if (cbTransferred > 0)
	{
		printf("We did it!");
	}
}

void CALLBACK NetworkClient::DataSent(
	IN DWORD dwError,
	IN DWORD cbTransferred,
	IN LPWSAOVERLAPPED lpOverlapped,
	IN DWORD dwFlags
	)
{
	if (cbTransferred > 0)
	{
		printf("We did it!");
	}
}
/**
*
*
*/
NetworkClient::NetworkClient(Cprofilermain *profMain, std::wstring hostName, std::wstring port) 
{
	// TODO: Complete the network client.
	// TODO: Create packet structure to allow ease of transmission of data.
	this->m_HostName.assign(hostName);
	this->m_HostPort.assign(port);

	// Select the highest socket version 2.2
	WORD wVersionRequested = MAKEWORD(2, 2);
	WSADATA wsaData;
	WSAStartup(wVersionRequested, &wsaData);
	NetworkClient::m_SocketConnection = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, NULL, WSA_FLAG_OVERLAPPED);
	timeval t;
	t.tv_sec = 2;
	// Just connect to the 
	WSAConnectByName(NetworkClient::m_SocketConnection, (LPWSTR)m_HostName.c_str(), (LPWSTR)m_HostPort.c_str(), NULL, NULL, NULL, NULL, &t, NULL);
	WSABUF bufs[1];
	auto cmd = SendString(L"Test!");
	bufs[0].buf = cmd.Encode()->data();
#pragma warning(suppress : 4267) // I'm only sending max 4k of data in one command however, the size() prop is long long. This is valid.
	bufs[0].len = cmd.Encode()->size();
	DWORD bytesSent = 0;
	DWORD flags = 0;
	WSAOVERLAPPED overlapped;
	SecureZeroMemory(&overlapped, sizeof(WSAOVERLAPPED));
	this->m_DataSent = this->DataSent;
	WSASend(this->m_SocketConnection, bufs, 1, &bytesSent, flags, &overlapped, &NetworkClient::DataSent);
	int err = WSAGetLastError();
	DWORD flag2s = 0;
}


NetworkClient::~NetworkClient()
{
	int i = 0;
	i++;
}

// This is the main loop that will be used for sending and receving data. When a call comes in we will have a callback to a correct processor
void NetworkClient::ControllerLoop()
{
}

// Start the network client when we're ready.
void NetworkClient::Start()
{
}
// Send a single command to the buffer to be processed.
HRESULT NetworkClient::SendCommand(std::shared_ptr<ICommand> packet)
{
	m_OutboundQueueFront.emplace(packet->Encode());
	return S_OK; 
}
// Receive a single command from the buffer to be processed.
std::shared_ptr<ICommand> NetworkClient::ReceiveCommand()
{ 
	auto itemtodecodeout = m_InboundQueueFront.front();
	auto cmdnumber = itemtodecodeout->at(4);
	auto itemout = m_CommandList[cmdnumber].Decode(itemtodecodeout);
	return itemout;
}
// Send a list of commands the buffer to be processed.
HRESULT NetworkClient::SendCommands(std::vector<std::shared_ptr<ICommand>> &packet)
{
	for (auto &x : packet)
	{
		m_OutboundQueueFront.emplace(x->Encode());
	}
	
	return S_OK;
}
// Receive a list of commands from the buffer to be processed.
std::vector<std::shared_ptr<ICommand>>& NetworkClient::ReceiveCommands()
{
	auto cmdlist = std::vector<std::shared_ptr<ICommand>>();
	while (!m_InboundQueueFront.empty())
	{
		auto itemtodecodeout = m_InboundQueueFront.front();
		auto cmdnumber = itemtodecodeout->at(4);
		auto itemout = m_CommandList[cmdnumber].Decode(itemtodecodeout);
		cmdlist.emplace_back(itemout);
	}
	return cmdlist = std::vector<std::shared_ptr<ICommand>>();
}