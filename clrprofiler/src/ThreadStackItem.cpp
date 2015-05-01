#include "stdafx.h"
#include "stackitems.h"


// Generic garbage collection suspension creation


ThreadStackItem::ThreadStackItem(ThreadID threadId, ThreadStackReason reason) : StackItemBase(threadId, reason)
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

