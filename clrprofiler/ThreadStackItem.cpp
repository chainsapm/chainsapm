#pragma once
#include "stdafx.h"
#include "ThreadStackItem.h"


// Default constructor for Thread Stack Item
StackItemBase::StackItemBase()
{
	// CRITICAL 2 Add local thread safety for this class and it's subclasses.
	// CRITICAL 4 Suggest to use the Slim Read Writer lock for shared access.
	QueryPerformanceCounter(&this->m_EnterTime);
	this->m_GCReasons = std::vector<GC_REASON>(); // Setting this to 10 suspend reasons
	this->m_SuspensionReasons = std::vector<COR_PRF_SUSPEND_REASON>();
	GetSystemTime(&m_StartWallTime);
}


StackItemBase::~StackItemBase()
{
	this->m_GCReasons.empty();
	this->m_SuspensionReasons.empty();
}

int StackItemBase::SequenceNumber() const
{
	return this->m_SequenceNumber;
}

void StackItemBase::SequenceNumber(int seqNumber)
{
	this->m_SequenceNumber = seqNumber;
}


ULONGLONG StackItemBase::ItemStartTime() const
{
	return this->m_EnterTime.QuadPart;
}

void StackItemBase::UpdateItemStackReason(ThreadStackReason stackReason)
{
	this->m_LastReason = stackReason;
}



ULONGLONG StackItemBase::GCSuspensionTime() const
{
	//this->m_GarbageCollectionTotal = m_GarbageCollectionTimeEnd.QuadPart - m_GarbageCollectionTimeStart.QuadPart;
	return this->m_GarbageCollectionTotal;
}

ULONGLONG StackItemBase::OtherSuspensionTime() const
{
	//this->m_SuspensionTotal = m_RuntimeSuspensionEnd.QuadPart - m_RuntimeSuspensionStart.QuadPart;
	return this->m_SuspensionTotal;
}

ULONGLONG StackItemBase::ItemRunTime() const
{
	//this->m_ItemTotal = m_LeaveTime - m_EnterTime;
	return this->m_ItemTotal;
}

void StackItemBase::UpdateLeaveTime()
{
	LARGE_INTEGER leaveTime;
	QueryPerformanceCounter(&leaveTime);
	this->m_ItemTotal = leaveTime.QuadPart - this->m_EnterTime.QuadPart;
}

ULONGLONG StackItemBase::ProfilingOverhead() const
{
	return this->m_ProfilingOverheadTotal;
}

int StackItemBase::Depth() const
{
	return this->m_Depth;
}

void StackItemBase::Depth(int depth)
{
	this->m_Depth = depth;
}

const ThreadStackReason& StackItemBase::LastReason()
{
	return this->m_LastReason;
}

void StackItemBase::PolyDummy()
{

}

FunctionStackItem::FunctionStackItem(FunctionID funcId, ThreadStackReason reason, std::shared_ptr<std::vector<UINT_PTR>>& byteData)
{
	this->m_FunctionID = funcId;
	this->m_LastReason = reason;
	this->m_ParameterValues.swap(byteData);
	
}

FunctionStackItem::~FunctionStackItem()
{
	delete[] this->m_ParameterRanges;
	delete &this->m_ParameterValues;
}

ULONG FunctionStackItem::ParameterCount() const
{
	return this->m_ParameterInfo.numRanges;

}
const std::shared_ptr<std::vector<UINT_PTR>>& FunctionStackItem::ItemStackParameters() const
{
	return this->m_ParameterValues;
}

void FunctionStackItem::ReturnValue(const UINT_PTR& input)
{
	this->m_ReturnData = input;

}

UINT_PTR FunctionStackItem::ReturnValue() const
{
	return m_ReturnData;

}

FunctionID FunctionStackItem::FunctionId() const
{
	return this->m_FunctionID;
}



// Generic runtime suspension creation
RuntimeSuspensionStackItem::RuntimeSuspensionStackItem(COR_PRF_SUSPEND_REASON suspensionReason) : StackItemBase()
{
	this->m_SuspensionReasons.push_back(suspensionReason);
}

// Generic garbage collection suspension creation
GarbageCollectionStackItem::GarbageCollectionStackItem(COR_PRF_GC_REASON suspensionReason, int maxGenerationCollected) : StackItemBase()
{
	this->m_GCReasons.push_back(GC_REASON(suspensionReason, maxGenerationCollected));
}

ThreadStackItem::ThreadStackItem(ThreadID threadId, ThreadStackReason reason) : StackItemBase()
{
	this->m_ThreadID = threadId;
	this->m_LastReason = reason;
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