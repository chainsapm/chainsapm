#include "stdafx.h"
#include "stackitems.h"


// Default constructor for Thread Stack Item
StackItemBase::StackItemBase()
{
	// CRITICAL 2 Add local thread safety for this class and it's subclasses.
	// CRITICAL 4 Suggest to use the Slim Read Writer lock for shared access.
	QueryPerformanceCounter(&this->m_EnterTime);
	GetSystemTime(&m_StartWallTime);
}

StackItemBase::StackItemBase(ThreadID thread, ThreadStackReason reason) : StackItemBase::StackItemBase()
{
	
	this->m_Thread = thread;
	this->m_Reason = reason;
}

int StackItemBase::SequenceNumber() const
{
	return this->m_SequenceNumber;
}

void StackItemBase::SequenceNumber(int seqNumber)
{
	this->m_SequenceNumber = seqNumber;
}

SYSTEMTIME StackItemBase::ItemStartTime() const
{
	return this->m_StartWallTime;
}

LARGE_INTEGER StackItemBase::ItemProfilingTime() const
{
	return this->m_EnterTime;
}

int StackItemBase::Depth() const
{
	return this->m_Depth;
}

void StackItemBase::Depth(int depth)
{
	this->m_Depth = depth;
}

const ThreadStackReason& StackItemBase::Reason()
{
	return this->m_Reason;
}


std::shared_ptr<std::vector<BYTE>> StackItemBase::PacketHeader()
{
	auto vec = std::vector < BYTE >();
	auto sharedvec = std::make_shared<std::vector<BYTE>>(vec);
	return sharedvec;
}

