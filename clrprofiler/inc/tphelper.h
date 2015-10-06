#pragma once
#include "networkclient.h"

class tp_helper
{

private:

	PTP_POOL m_customThreadPool = nullptr;
	PTP_CALLBACK_ENVIRON m_ptpcbe = nullptr;
	PTP_CLEANUP_GROUP m_ptpcug = nullptr;

	template <class C>
	static VOID CALLBACK SendEventCallBack(PTP_CALLBACK_INSTANCE Instance, PVOID Parameter, PTP_WORK Work)
	{
		UNREFERENCED_PARAMETER(Instance);
		UNREFERENCED_PARAMETER(Work);
		std::shared_ptr<C> shrd(static_cast<C*>(Parameter));
		m_NetworkClient->SendCommand(shrd);
	}

public:

	tp_helper(NetworkClient * m_NetworkClient, int min, int max);
	~tp_helper();

	template <class C>
	void SendEvent(C* param)
	{
		auto tpw = CreateThreadpoolWork(&tp_helper::SendEventCallBack<C>, param, m_ptpcbe);
		SubmitThreadpoolWork(tpw);
		CloseThreadpoolWork(tpw);
	}
	static NetworkClient * m_NetworkClient;

	void CreateNetworkIoThreadPool(NetworkClient* NetClient);

};
