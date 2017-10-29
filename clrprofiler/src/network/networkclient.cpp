#include "stdafx.h"
#include <WinSock2.h>
#include <ws2tcpip.h>
#include "NetworkClient.h"
#include "critsec_helper.h"

// Initialize the socket to NULL.
SOCKET NetworkClient::m_SocketConnection = NULL;
HANDLE NetworkClient::DataReceived = NULL;
HANDLE NetworkClient::DataSent = NULL;
HANDLE NetworkClient::DataToBeSent = NULL;

#define SOCKET_BUFFER_MAX 1 * 1024 * 1024

NetworkClient::NetworkClient(std::wstring hostName, std::wstring port,
	std::shared_ptr<CommandProcessor> commandProc)
{
	// TODO: Complete the network client.
	// TODO: Create packet structure to allow ease of transmission of data.
	this->m_HostName.assign(hostName);
	this->m_HostPort.assign(port);
	InitializeCriticalSection(&FrontInboundLock);
	InitializeCriticalSection(&BackInboundLock);
	InitializeCriticalSection(&FrontOutboundLock);
	InitializeCriticalSection(&BackOutboundLock);
	InitializeCriticalSection(&OverflowBufferLock);

	CommandProc = commandProc;

	OverflowBuffer.reserve(SOCKET_BUFFER_MAX);

	DataSent = CreateEvent(NULL, TRUE, FALSE, TEXT("DataSentEvent"));
	DataReceived = CreateEvent(NULL, TRUE, FALSE, TEXT("DataReceivedEvent"));
	DataToBeSent = CreateEvent(NULL, TRUE, FALSE, TEXT("DataToBeSent"));

	// Select the highest socket version 2.2
	WORD wVersionRequested = MAKEWORD(2, 2);
	WSADATA wsaData;
	WSAStartup(wVersionRequested, &wsaData);
	NetworkClient::m_SocketConnection = WSASocket(AF_INET6, SOCK_STREAM, IPPROTO_TCP, NULL, NULL, WSA_FLAG_OVERLAPPED);
	if (NetworkClient::m_SocketConnection != INVALID_SOCKET)
	{
		DWORD ipv6only = 0;
		int retval = setsockopt(NetworkClient::m_SocketConnection, IPPROTO_IPV6, IPV6_V6ONLY, (char*)&ipv6only, sizeof(ipv6only));
		if (retval == SOCKET_ERROR)
		{
			fprintf(stdout, "Error setting SOCKET OPTION for IPV6_ONLY");
			OutputDebugString(L"Error setting SOCKET OPTION for IPV6_ONLY");
		}
		else {
			timeval t;
			t.tv_sec = 2;
			fprintf(stdout, "Connecting to %S on port %S\n", m_HostName.c_str(), m_HostPort.c_str());
			ADDRINFOW *ptr = NULL;
			LPSOCKADDR sockaddr_ip = NULL;
			sockaddr_in * sockaddr_ipv4 = NULL;
			sockaddr_in6 * sockaddr_ipv6 = NULL;
			DWORD ipbufferlength = 46;
			wchar_t ipstringbuffer[46];
			GetAddrInfo(m_HostName.c_str(), NULL, NULL, &ptr);
			while (ptr != NULL)
			{
				switch (ptr->ai_family) {
				case AF_UNSPEC:
					fprintf(stdout, "Unspecified\n");
					break;
				case AF_INET:
					printf("AF_INET (IPv4)");
					sockaddr_ipv4 = (struct sockaddr_in *) ptr->ai_addr;
					fprintf(stdout, "\tIPv4 address %S\n",
						InetNtop(AF_INET, &sockaddr_ipv4->sin_addr, ipstringbuffer, ipbufferlength));
					break;
				case AF_INET6:
					printf("AF_INET6 (IPv6)");
					sockaddr_ipv6 = (struct sockaddr_in6 *) ptr->ai_addr;
					printf("\tIPv6 address %S\n",
						InetNtop(AF_INET6, &sockaddr_ipv6->sin6_addr, ipstringbuffer, ipbufferlength));
					break;

				}
				ptr = ptr->ai_next;

			}
			WSAConnectByName(NetworkClient::m_SocketConnection, (LPWSTR)m_HostName.c_str(), (LPWSTR)m_HostPort.c_str(), NULL, NULL, NULL, NULL, &t, NULL);

		}
	}


}


NetworkClient::~NetworkClient()
{
	int i = 0;
	i++;
}

// Start the network client when we're ready.
void NetworkClient::Start()
{
	recvTimer = CreateThreadpoolTimer(&NetworkClient::ReceiveTimerCallback, this, NULL); // See "Customized Thread Pools" section
	sendTimer = CreateThreadpoolTimer(&NetworkClient::SendTimerCallback, this, NULL); // See "Customized Thread Pools" section
	dataReceived = CreateThreadpoolWait(&NetworkClient::DataReceivedCallback, this, NULL);

	__int64 dueFileTime = -1 * _SECOND;

	FILETIME ftDue;
	ftDue.dwLowDateTime = (DWORD)(dueFileTime & 0xFFFFFFFF);
	ftDue.dwHighDateTime = (DWORD)(dueFileTime >> 32);
	SetThreadpoolTimer(recvTimer, &ftDue, 500, 0);
	SetThreadpoolTimer(sendTimer, &ftDue, 250, 0);
	SetThreadpoolWait(dataReceived, NetworkClient::DataReceived, NULL);
}

// Start the network client when we're ready.
void NetworkClient::Shutdown()
{
	Sleep(1500);
	CloseThreadpoolTimer(sendTimer);
	CloseThreadpoolTimer(recvTimer);
	closesocket(m_SocketConnection);
}

HRESULT NetworkClient::SendCommand(Commands::ICommand* packet)
{
	auto csh = critsec_helper::critsec_helper(&FrontOutboundLock);
	m_OutboundQueueFront.emplace(packet);
	ResetEvent(&NetworkClient::DataToBeSent);
	csh.leave_early();
	return S_OK;
};

HRESULT NetworkClient::SendCommand(std::shared_ptr<Commands::ICommand> packet)
{
	auto cshFQ = critsec_helper::critsec_helper(&FrontOutboundLock);
	m_OutboundQueueFront.emplace(packet);
	cshFQ.leave_early();
	return S_OK;
}

// Receive a single command from the buffer to be processed.
std::shared_ptr<Commands::ICommand> NetworkClient::ReceiveCommand()
{
	std::shared_ptr<Commands::ICommand> itemout;
	{
		auto cmdlist = std::vector<std::shared_ptr<Commands::ICommand>>();
		{
			auto csback = critsec_helper::critsec_helper(&BackInboundLock);
			auto csfront = critsec_helper::critsec_helper(&FrontInboundLock);
			if (!m_InboundQueueBack.empty() && m_InboundQueueFront.empty())
				m_InboundQueueFront.swap(m_InboundQueueBack); // Swap the back buffer with the front to allow uninterrupted data flow
			csback.leave_early(); // Done with the back buffer, bail early

			if (!m_InboundQueueFront.empty())
			{
				auto itemtodecodeout = m_InboundQueueFront.front();
				auto cmdnumber = *(short*)(itemtodecodeout->data() + 4);
				if (m_CommandList[cmdnumber] != nullptr)
				{
					itemout = m_CommandList[cmdnumber]->Decode(itemtodecodeout);
					m_InboundQueueFront.pop();
					return itemout;
				}
			}

		}

	}
	return nullptr;
}
// Send a list of commands the buffer to be processed.
HRESULT NetworkClient::SendCommands(std::vector<Commands::ICommand*> &packet)
{
	auto csh = critsec_helper::critsec_helper(&FrontOutboundLock);
	for (auto &x : packet)
	{
		m_OutboundQueueFront.emplace(x);
	}
	ResetEvent(&NetworkClient::DataToBeSent);
	csh.leave_early();
	return S_OK;
}
// Receive a list of commands from the buffer to be processed.
std::vector<std::shared_ptr<Commands::ICommand>> NetworkClient::ReceiveCommands()
{
	auto cmdlist = std::vector<std::shared_ptr<Commands::ICommand>>();
	{
		auto csback = critsec_helper::critsec_helper(&BackInboundLock);
		auto csfront = critsec_helper::critsec_helper(&FrontInboundLock);
		if (!m_InboundQueueBack.empty() && m_InboundQueueFront.empty())
			m_InboundQueueFront.swap(m_InboundQueueBack); // Swap the back buffer with the front to allow uninterrupted data flow
		csback.leave_early(); // Done with the back buffer, bail early

		while (!m_InboundQueueFront.empty())
		{
			auto itemtodecodeout = m_InboundQueueFront.front();
			auto cmdnumber = itemtodecodeout->at(4);
			if (m_CommandList[cmdnumber] != nullptr)
			{
				auto itemout = m_CommandList[cmdnumber]->Decode(itemtodecodeout);
				cmdlist.emplace_back(itemout);
			}
			m_InboundQueueFront.pop();
		}
	}
	return cmdlist;
}

VOID CALLBACK NetworkClient::SendTimerCallback(
	PTP_CALLBACK_INSTANCE pInstance, // See "Callback Termination Actions" section
	PVOID pvContext,
	PTP_TIMER pTimer)
{
	auto netClient = static_cast<NetworkClient*>(pvContext);
	if (!netClient->insideSendLock)
	{
		ResetEvent(NetworkClient::DataSent);
		std::vector<std::shared_ptr<std::vector<char>>> * m_Passable = new std::vector<std::shared_ptr<std::vector<char>>>();
		netClient->insideSendLock = true;
		WSABUF *bufs = NULL;
		size_t bufscount = 0;
		{
			auto cshFQ = critsec_helper::critsec_helper(&netClient->FrontOutboundLock);
			auto cshBQ = critsec_helper::critsec_helper(&netClient->BackOutboundLock);
			netClient->m_OutboundQueueBack.swap(netClient->m_OutboundQueueFront);

			cshBQ.leave_early();
			cshFQ.leave_early();
		}


		auto cshBQ = critsec_helper::critsec_helper(&netClient->BackOutboundLock);
		size_t buffersize = netClient->m_OutboundQueueBack.size() + 2;
		bufs = new WSABUF[buffersize];
		int counter = 1;
		size_t sizeCounter = 0;
		while (!netClient->m_OutboundQueueBack.empty())
		{
			auto itemtodecodeout = netClient->m_OutboundQueueBack.front()->Encode();
			bufs[counter].buf = itemtodecodeout->data();
#pragma warning(suppress : 4267) // I'm only sending max 4k of data in one command however, the size() prop is __int64. This is valid.
			bufs[counter].len = itemtodecodeout->size();
			m_Passable->emplace_back(itemtodecodeout);
			++counter;
			sizeCounter += itemtodecodeout->size();
			netClient->m_OutboundQueueBack.pop();
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
			ncc->Direction = NetClietCallback::_direction::SEND;
			DWORD bytesSent = 0;
			DWORD flags = 0;
			WSAOVERLAPPED overlapped;
			SecureZeroMemory(&overlapped, sizeof(WSAOVERLAPPED));
			StartThreadpoolIo(netClient->m_ptpIO);
			auto result = WSASend(netClient->m_SocketConnection, bufs, bufscount, &bytesSent, flags, ncc, nullptr);
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
				CancelThreadpoolIo(netClient->m_ptpIO);
			}
		}
		//WaitForThreadpoolIoCallbacks(netClient->m_ptpIO, FALSE);
		SetEvent(NetworkClient::DataToBeSent); // Let any waiters know that the data was sent to the completion port
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
		ResetEvent(NetworkClient::DataReceived);
		netClient->insideReceiveLock = true;
		auto bigBufferChars = new char[SOCKET_BUFFER_MAX];
		LPWSABUF bigBuffer = new WSABUF;
		bigBuffer->buf = bigBufferChars;
		bigBuffer->len = SOCKET_BUFFER_MAX;
		SecureZeroMemory(bigBufferChars, SOCKET_BUFFER_MAX);
		DWORD bytesRecvd = 0;
		DWORD flags = 0;
		WSAOVERLAPPED overlapped;
		SecureZeroMemory(&overlapped, sizeof(WSAOVERLAPPED));

		NetClietCallback *ncc = new NetClietCallback();
		ncc->netclient = netClient;
		ncc->queue = bigBuffer;
		ncc->Direction = NetClietCallback::_direction::RECV;

		StartThreadpoolIo(netClient->m_ptpIO);
		auto result = WSARecv(netClient->m_SocketConnection, bigBuffer, 1, &bytesRecvd, &flags, ncc, nullptr);
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
			CancelThreadpoolIo(netClient->m_ptpIO);
		}
		// If there is nothing here we need to continue
		//WaitForThreadpoolTimerCallbacks(pTimer, FALSE);
		//WaitForThreadpoolIoCallbacks(netClient->m_ptpIO, FALSE);
		netClient->insideReceiveLock = false;
	}
}

VOID NetworkClient::DataReceivedCallback(PTP_CALLBACK_INSTANCE Instance, PVOID Context, PTP_WAIT Wait, TP_WAIT_RESULT WaitResult)
{
	auto netClient = static_cast<NetworkClient*>(Context);
	auto cmdList = netClient->ReceiveCommands();
	for (auto &cmd : cmdList)
	{
		if (std::dynamic_pointer_cast<Commands::SendInjectionMetadata>(cmd) != nullptr)
		{
			netClient->CommandProc->Process(std::dynamic_pointer_cast<Commands::SendInjectionMetadata>(cmd));
		}
		else {
			// TODO: Add logging for command not found
		}
	}
	cmdList.clear();
	if (WaitForSingleObject(NetworkClient::DataReceived, 1) == WAIT_OBJECT_0)
	{
		ResetEvent(NetworkClient::DataReceived);
	}
	SetThreadpoolWait(Wait, NetworkClient::DataReceived, NULL);

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
	auto lpOverlapped = (NetClietCallback*)Overlapped;
	auto cbTransferred = NumberOfBytesTransferred;
	if (lpOverlapped->Direction == NetClietCallback::_direction::RECV)
	{
		if (cbTransferred > 0)
		{
			auto charBuff = (char*)lpOverlapped->queue->buf;
			auto iterBuff = (char*)lpOverlapped->queue->buf;
			DWORD totalBuffSize = *(DWORD*)charBuff;

			if (totalBuffSize == cbTransferred)
			{
				auto term = *(unsigned int*)(charBuff + (totalBuffSize - 4));
				lpOverlapped->netclient->AddItemsToBackBuffer(term, iterBuff, totalBuffSize);
			}
			else {
				auto cshFQ = critsec_helper::critsec_helper(&lpOverlapped->netclient->OverflowBufferLock);
				std::copy(charBuff, charBuff + cbTransferred, lpOverlapped->netclient->OverflowBuffer.begin() + lpOverlapped->netclient->OverflowBuffer.size());
			}

			auto cshFQ = critsec_helper::critsec_helper(&lpOverlapped->netclient->OverflowBufferLock);
			{
				totalBuffSize = *(DWORD*)lpOverlapped->netclient->OverflowBuffer.data();

				if (lpOverlapped->netclient->OverflowBuffer.size() > 0 && lpOverlapped->netclient->OverflowBuffer.size() == totalBuffSize) {
					auto term = *(unsigned int*)(lpOverlapped->netclient->OverflowBuffer.data() + (lpOverlapped->netclient->OverflowBuffer.size() - 4));
					charBuff = lpOverlapped->netclient->OverflowBuffer.data();
					iterBuff = lpOverlapped->netclient->OverflowBuffer.data();
					lpOverlapped->netclient->AddItemsToBackBuffer(term, iterBuff, totalBuffSize);
				}
			}
		}
		try
		{
			SetEventWhenCallbackReturns(Instance, NetworkClient::DataReceived);
		}
		catch (...)
		{
			auto s = L"TEST";
		}
		delete lpOverlapped->queue->buf;
		delete lpOverlapped->queue;
	}
	else {
		SetEventWhenCallbackReturns(Instance, NetworkClient::DataSent);; // Signal to the event processor that we have sent the data.

		// Get rid of all of the vector<char> we sent
		lpOverlapped->sendqueue->clear();
		delete lpOverlapped->sendqueue;
	}
}

void NetworkClient::AddItemsToBackBuffer(unsigned int term, char * &iterBuff, const DWORD &totalBuffSize)
{
	if (term == 0xCCCCCCCC)
	{
		char* lastBuffer = iterBuff + (totalBuffSize - 4);
		iterBuff += 4; // Skip the first 4 bytes for the size
		while (iterBuff < lastBuffer)
		{
			int localBufferSize = *(int*)iterBuff; // Size of the current command
			auto vec = new std::vector<char>(iterBuff, iterBuff + localBufferSize);
			{
				critsec_helper csh(&BackInboundLock);
				m_InboundQueueBack.emplace(vec);
			}
			iterBuff += localBufferSize;

		}
	}
}

void NetworkClient::SetThreadPoolIO(PTP_IO ptpIO)
{
	DWORD dw = 0;
	if (ptpIO == NULL)
	{
		dw = GetLastError();
	}
	HRESULT hr = HRESULT_FROM_WIN32(dw);
	m_ptpIO = ptpIO;
}

