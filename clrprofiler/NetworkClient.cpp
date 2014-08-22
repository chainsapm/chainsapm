#include "stdafx.h"
#include <WinSock2.h>
#include <ws2tcpip.h>
#include "profilermain.h"

// Initialize the socket to NULL.
SOCKET NetworkClient::m_SocketConnection = NULL;

//NetworkClient::NetworkClient()
//{
//}

void CALLBACK NewDataReceived(
	IN DWORD dwError,
	IN DWORD cbTransferred,
	IN LPWSAOVERLAPPED lpOverlapped,
	IN DWORD dwFlags
	)
{
}

void CALLBACK DataSent(
	IN DWORD dwError,
	IN DWORD cbTransferred,
	IN LPWSAOVERLAPPED lpOverlapped,
	IN DWORD dwFlags
	)
{

}

NetworkClient::NetworkClient(Cprofilermain *profMain, std::wstring hostName, std::wstring port)
{
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
	std::string s = std::string("Test from netclient");
	bufs[0].buf = s._Myptr();
	bufs[0].len = s.length();
	DWORD bytesSent = 0;
	DWORD flags = 0;
	WSAOVERLAPPED overlapped;
	SecureZeroMemory(&overlapped, sizeof(WSAOVERLAPPED));
	WSASend(this->m_SocketConnection, bufs, 1, &bytesSent, flags, &overlapped, &DataSent);
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
