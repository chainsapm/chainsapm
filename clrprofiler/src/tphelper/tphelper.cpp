#include "stdafx.h"
#include "tphelper.h"

NetworkClient * tp_helper::m_NetworkClient = NULL;

tp_helper::tp_helper(NetworkClient * m_NetworkClient, int min, int max)
{
	m_ptpcbe = new TP_CALLBACK_ENVIRON();
	InitializeThreadpoolEnvironment(m_ptpcbe);
	HMODULE _ignore;
	GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_PIN, TEXT("clrprofiler.dll"), &_ignore);
	GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_PIN, TEXT("msvcrt.dll"), &_ignore);
	GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_PIN, TEXT("msvcrtd.dll"), &_ignore);
	GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_PIN, TEXT("ucrtbase.dll"), &_ignore);
	GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_PIN, TEXT("ucrtbased.dll"), &_ignore);

	SetThreadpoolCallbackLibrary(m_ptpcbe, GetModuleHandle(TEXT("clrprofiler.dll")));
	SetThreadpoolCallbackLibrary(m_ptpcbe, GetModuleHandle(TEXT("msvcrt.dll")));
	SetThreadpoolCallbackLibrary(m_ptpcbe, GetModuleHandle(TEXT("msvcrtd.dll")));
	SetThreadpoolCallbackLibrary(m_ptpcbe, GetModuleHandle(TEXT("ucrtbase.dll")));
	SetThreadpoolCallbackLibrary(m_ptpcbe, GetModuleHandle(TEXT("ucrtbased.dll")));
	SetThreadpoolCallbackLibrary(m_ptpcbe, GetModuleHandle(TEXT("MSVCR120_CLR0400.dll")));
	SetThreadpoolCallbackLibrary(m_ptpcbe, GetModuleHandle(TEXT("MSVCP140D.dll")));
	SetThreadpoolCallbackLibrary(m_ptpcbe, GetModuleHandle(TEXT("VCRUNTIME140D.dll")));
	SetThreadpoolCallbackLibrary(m_ptpcbe, GetModuleHandle(TEXT("MSVCP140.dll")));
	SetThreadpoolCallbackLibrary(m_ptpcbe, GetModuleHandle(TEXT("VCRUNTIME140.dll")));

	m_customThreadPool = CreateThreadpool(NULL);
	SetThreadpoolThreadMinimum(m_customThreadPool, min);
	SetThreadpoolThreadMaximum(m_customThreadPool, max);
	m_ptpcug = CreateThreadpoolCleanupGroup();
	SetThreadpoolCallbackPool(m_ptpcbe, m_customThreadPool);
	SetThreadpoolCallbackCleanupGroup(m_ptpcbe, m_ptpcug, NULL);
}

void tp_helper::CreateNetworkIoThreadPool(NetworkClient* NetClient)
{

	NetClient->SetThreadPoolIO(CreateThreadpoolIo(reinterpret_cast<HANDLE>(NetworkClient::m_SocketConnection),
		NetworkClient::IoCompletionCallback, NetClient, m_ptpcbe));

}

tp_helper::~tp_helper()
{
}
