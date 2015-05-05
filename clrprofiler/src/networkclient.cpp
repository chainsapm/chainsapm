#include "stdafx.h"
#include <WinSock2.h>
#include <ws2tcpip.h>
#include "NetworkClient.h"
#include "critsec_helper.h"




// Initialize the socket to NULL.
SOCKET NetworkClient::m_SocketConnection = NULL;
PTP_IO NetworkClient::m_ptpIO = NULL;
HANDLE NetworkClient::SendRecvEvent;



NetworkClient::NetworkClient(std::wstring hostName, std::wstring port)
{
	// TODO: Complete the network client.
	// TODO: Create packet structure to allow ease of transmission of data.
	this->m_HostName.assign(hostName);
	this->m_HostPort.assign(port);
	InitializeCriticalSection(&FrontInboundLock);
	InitializeCriticalSection(&BackInboundLock);
	InitializeCriticalSection(&FrontOutboundLock);
	InitializeCriticalSection(&BackOutboundLock);
	
	// Select the highest socket version 2.2
	WORD wVersionRequested = MAKEWORD(2, 2);
	WSADATA wsaData;
	WSAStartup(wVersionRequested, &wsaData);
	NetworkClient::m_SocketConnection = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, NULL, WSA_FLAG_OVERLAPPED);
	timeval t;
	t.tv_sec = 2;
	// Just connect to the 
	WSAConnectByName(NetworkClient::m_SocketConnection, (LPWSTR)m_HostName.c_str(), (LPWSTR)m_HostPort.c_str(), NULL, NULL, NULL, NULL, &t, NULL);
	
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

	recvTimer = CreateThreadpoolTimer(&NetworkClient::ReceiveTimerCallback, this, NULL); // See "Customized Thread Pools" section
	sendTimer = CreateThreadpoolTimer(&NetworkClient::SendTimerCallback, this, NULL); // See "Customized Thread Pools" section

	__int64 dueFileTime = -1 * _SECOND;
	FILETIME ftDue;
	ftDue.dwLowDateTime = (DWORD)(dueFileTime & 0xFFFFFFFF);
	ftDue.dwHighDateTime = (DWORD)(dueFileTime >> 32);
	SetThreadpoolTimer(recvTimer, &ftDue, 1000, 0);
	SetThreadpoolTimer(sendTimer, &ftDue, 250, 0);
}

// Start the network client when we're ready.
void NetworkClient::Shutdown()
{
	Sleep(1500);
	CloseThreadpoolTimer(sendTimer);
	CloseThreadpoolTimer(recvTimer);
	closesocket(m_SocketConnection);
}

// Send a single command to the buffer to be processed.
HRESULT NetworkClient::SendRoutedCommand(std::shared_ptr<Commands::RouteCommand> packet)
{
	/*auto cshFQ = critsec_helper::critsec_helper(&FrontOutboundLock);
	m_OutboundQueueFront.emplace(packet->Encode());
	cshFQ.leave_early();*/
	return S_OK;
}


// Receive a single command from the buffer to be processed.
std::shared_ptr<Commands::ICommand> NetworkClient::ReceiveCommand()
{
	std::shared_ptr<Commands::ICommand> itemout;
	{
		critsec_helper::critsec_helper(&FrontInboundLock);
		auto itemtodecodeout = m_InboundQueueFront.front();
		auto cmdnumber = itemtodecodeout->at(4);
		itemout = m_CommandList[cmdnumber].Decode(itemtodecodeout);
		m_InboundQueueBack.pop();
	}
	return itemout;
}
// Send a list of commands the buffer to be processed.
HRESULT NetworkClient::SendCommands(std::vector<std::shared_ptr<Commands::ICommand>> &packet)
{
	{
		critsec_helper::critsec_helper(&FrontOutboundLock);
		for (auto &x : packet)
		{
			m_OutboundQueueFront.emplace(x->Encode());
		}
	}

	return S_OK;
}
// Receive a list of commands from the buffer to be processed.
std::vector<std::shared_ptr<Commands::ICommand>>& NetworkClient::ReceiveCommands()
{
	auto cmdlist = std::vector<std::shared_ptr<Commands::ICommand>>();
	{
		critsec_helper::critsec_helper(&FrontInboundLock);
		while (!m_InboundQueueFront.empty())
		{
			auto itemtodecodeout = m_InboundQueueFront.front();
			auto cmdnumber = itemtodecodeout->at(4);
			auto itemout = m_CommandList[cmdnumber].Decode(itemtodecodeout);
			cmdlist.emplace_back(itemout);
			m_InboundQueueBack.pop();
		}
	}
	return cmdlist = std::vector<std::shared_ptr<Commands::ICommand>>();
}

VOID CALLBACK NetworkClient::SendTimerCallback(
	PTP_CALLBACK_INSTANCE pInstance, // See "Callback Termination Actions" section
	PVOID pvContext,
	PTP_TIMER pTimer)
{
	auto netClient = static_cast<NetworkClient*>(pvContext);
	if (!netClient->insideSendLock)
	{
		std::queue<std::shared_ptr<std::vector<char>>> * m_Passable = new std::queue<std::shared_ptr<std::vector<char>>>();
		netClient->insideSendLock = true;
		WSABUF *bufs = NULL;
		int bufscount = 0;
		{
			auto cshFQ = critsec_helper::critsec_helper(&netClient->FrontOutboundLock);
			auto cshBQ = critsec_helper::critsec_helper(&netClient->BackOutboundLock);
			netClient->m_OutboundQueueBack.swap(netClient->m_OutboundQueueFront);
			cshBQ.leave_early();
			cshFQ.leave_early();
		}

		auto cshBQ = critsec_helper::critsec_helper(&netClient->BackOutboundLock);
		int buffersize = netClient->m_OutboundQueueBack.size() + 2;
		bufs = new WSABUF[buffersize];
		int counter = 1;
		int sizeCounter = 0;
		while (!netClient->m_OutboundQueueBack.empty())
		{
			auto itemtodecodeout = netClient->m_OutboundQueueBack.front();
			bufs[counter].buf = itemtodecodeout->data();
#pragma warning(suppress : 4267) // I'm only sending max 4k of data in one command however, the size() prop is long long. This is valid.
			bufs[counter].len = itemtodecodeout->size();
			netClient->m_OutboundQueueBack.pop();
			m_Passable->emplace(itemtodecodeout);
			++counter;
			sizeCounter += itemtodecodeout->size();
		}
		cshBQ.leave_early();
		sizeCounter += 8;
		char *size = (char*)&sizeCounter;
		char term[] = { 0xCC, 0xCC, 0xCC, 0xCC };
		bufs[0].buf = size;
		bufs[0].len = 4;
		bufs[counter].buf = term;
		bufs[counter].len = 4;
		
		bufscount = buffersize;

		if (bufscount > 2)
		{
			NetClietCallback *ncc = new NetClietCallback();
			ncc->netclient = netClient;
			ncc->sendqueue = m_Passable;
			DWORD bytesSent = 0;
			DWORD flags = 0;
			WSAOVERLAPPED overlapped;
			SecureZeroMemory(&overlapped, sizeof(WSAOVERLAPPED));
			StartThreadpoolIo(NetworkClient::m_ptpIO);
			auto result = WSASend(netClient->m_SocketConnection, bufs, bufscount, &bytesSent, flags, &overlapped, nullptr);
			if (!result)
			{
				result = WSA_IO_PENDING;
			}
			else
			{
				result = WSAGetLastError();
			}
			if (WSA_IO_PENDING != result)
			{
				CancelThreadpoolIo(NetworkClient::m_ptpIO);
			}
		}
		WaitForThreadpoolIoCallbacks(NetworkClient::m_ptpIO, FALSE);
		netClient->insideSendLock = false;
	}
}

VOID CALLBACK NetworkClient::ReceiveTimerCallback(
	PTP_CALLBACK_INSTANCE pInstance, // See "Callback Termination Actions" section
	PVOID pvContext,
	PTP_TIMER pTimer)
{
	auto netClient = static_cast<NetworkClient*>(pvContext);
	if (!netClient->insideReceiveLock)
	{
		netClient->insideReceiveLock = true;


		auto bigBufferChars = new char[10 * 1024];
		LPWSABUF bigBuffer = new WSABUF;
		bigBuffer->buf = bigBufferChars;
		bigBuffer->len = 10 * 1024;
		SecureZeroMemory(bigBufferChars, 10 * 1024);
		DWORD bytesRecvd = 0;
		DWORD flags = 0;
		WSAOVERLAPPED overlapped;
		SecureZeroMemory(&overlapped, sizeof(WSAOVERLAPPED));

		NetClietCallback *ncc = new NetClietCallback();
		ncc->netclient = netClient;
		ncc->queue = bigBuffer;
		StartThreadpoolIo(NetworkClient::m_ptpIO);
		auto result = WSARecv(netClient->m_SocketConnection, bigBuffer, 1, &bytesRecvd, &flags, &overlapped, nullptr);
		if (!result)
		{
			result = WSA_IO_PENDING;
		}
		else
		{
			result = WSAGetLastError();
		}
		if (WSA_IO_PENDING != result)
		{
			CancelThreadpoolIo(NetworkClient::m_ptpIO);
		}
		int err = WSAGetLastError();
		DWORD flag2s = 0;
		WaitForThreadpoolIoCallbacks(NetworkClient::m_ptpIO, FALSE);
		netClient->insideReceiveLock = false;
	}
}


void CALLBACK NetworkClient::NewDataReceived(
	IN DWORD dwError,
	IN DWORD cbTransferred,
	IN LPWSAOVERLAPPED lpOverlapped,
	IN DWORD dwFlags
	)
{
	if (cbTransferred > 0)
	{
		if (lpOverlapped->hEvent != NULL)
		{
			NetClietCallback *ncc = (NetClietCallback*)lpOverlapped->hEvent;
			ncc->netclient->insideReceiveLock = false;
			auto myqueue = ncc->queue;
			ncc->netclient = NULL;
			delete lpOverlapped->hEvent;
		}
	}
}

void CALLBACK NetworkClient::DataSent(
	IN DWORD dwError,
	IN DWORD cbTransferred,
	IN LPWSAOVERLAPPED lpOverlapped,
	IN DWORD dwFlags
	)
{
		if (lpOverlapped->hEvent != NULL )
		{
			NetClietCallback *ncc = (NetClietCallback*)lpOverlapped->hEvent;
			ncc->netclient->insideSendLock = false;
			ncc->netclient = NULL;
			delete lpOverlapped->hEvent;
		}
}


VOID CALLBACK NetworkClient::SendRecvData(
	_In_ PVOID   lpParameter,
	_In_ BOOLEAN TimerOrWaitFired
	)
{
	Sleep(5000);
	ResetEvent(NetworkClient::SendRecvEvent);
}

VOID CALLBACK NetworkClient::IoCompletionCallback(
	_Inout_     PTP_CALLBACK_INSTANCE Instance,
	_Inout_opt_ PVOID                 Context,
	_Inout_opt_ PVOID                 Overlapped,
	_In_        ULONG                 IoResult,
	_In_        ULONG_PTR             NumberOfBytesTransferred,
	_Inout_     PTP_IO                Io
	)
{
	ResetEvent(NetworkClient::SendRecvEvent);
}

void NetworkClient::SetPTPIO(PTP_IO ptpIO)
{
	NetworkClient::m_ptpIO = ptpIO;
}