#include "stdafx.h"
#include "stackitems.h"


// Generic garbage collection suspension creation


ThreadStackItem::ThreadStackItem(ThreadID threadId, ThreadStackReason reason) : StackItemBase()
{
	this->m_ThreadID = threadId;
	this->m_Reason = reason;
	this->m_ThreadName.assign(L"Worker Thread");
}

const std::wstring& ThreadStackItem::ThreadName() const
{
	return this->m_ThreadName;
}

void ThreadStackItem::ThreadName(const std::wstring& threadName)
{
	this->m_ThreadName.assign(threadName);
}

ThreadID ThreadStackItem::ThreadId() const
{
	return this->m_ThreadID;
}

std::shared_ptr<std::vector<BYTE>> ThreadStackItem::SendPacket()
{
	auto vec = std::vector < BYTE >();
	auto sharedvec = std::make_shared<std::vector<BYTE>>(vec);
	return sharedvec;
}