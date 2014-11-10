#include "stdafx.h"
#include "stackitems.h"


// Generic garbage collection suspension creation


ThreadStackItem::ThreadStackItem(int depth, int sequence, ThreadID threadId, ThreadStackReason reason) : StackItemBase(depth, sequence, threadId, reason)
{
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



std::shared_ptr<std::vector<BYTE>> ThreadStackItem::SendPacket()
{
	auto vec = std::vector < BYTE >();
	auto sharedvec = std::make_shared<std::vector<BYTE>>(vec);
	return sharedvec;
}