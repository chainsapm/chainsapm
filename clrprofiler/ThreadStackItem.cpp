#pragma once
#include "stdafx.h"
#include "ThreadStackItem.h"


// Default constructor for Thread Stack Item
StackItemBase::StackItemBase()
{
	QueryPerformanceCounter(&this->m_EnterTime);
	this->m_GCReasons = std::vector<GC_REASON>();
	this->m_SuspensionReasons = std::vector<COR_PRF_SUSPEND_REASON>();
	m_StartWallTime = boost::posix_time::second_clock::universal_time();
}


StackItemBase::~StackItemBase()
{
	this->m_GCReasons.empty();
	this->m_SuspensionReasons.empty();
}




ULONGLONG StackItemBase::ItemStartTime()
{
	return this->m_EnterTime.QuadPart;
}

void StackItemBase::UpdateItemStackReason(ThreadStackReason stackReason)
{
	this->m_LastReason = stackReason;
}



ULONGLONG StackItemBase::GCSuspensionTime()
{
	//this->m_GarbageCollectionTotal = m_GarbageCollectionTimeEnd.QuadPart - m_GarbageCollectionTimeStart.QuadPart;
	return this->m_GarbageCollectionTotal;
}

ULONGLONG StackItemBase::OtherSuspensionTime()
{
	//this->m_SuspensionTotal = m_RuntimeSuspensionEnd.QuadPart - m_RuntimeSuspensionStart.QuadPart;
	return this->m_SuspensionTotal;
}

ULONGLONG StackItemBase::ItemRunTime()
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

ULONGLONG StackItemBase::ProfilingOverhead()
{
	return this->m_ProfilingOverheadTotal;
}

int StackItemBase::Depth()
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

FunctionStackItem::FunctionStackItem(FunctionID funcId, ThreadStackReason reason, const COR_PRF_FUNCTION_ARGUMENT_INFO& byteData)
{
	this->m_FunctionID = funcId;
	this->m_LastReason = reason;

	if (byteData.numRanges != 0)
	{
		this->m_ParameterInfo = COR_PRF_FUNCTION_ARGUMENT_INFO(byteData);
		for (ULONG parameterCount = 0; parameterCount < this->m_ParameterInfo.numRanges; parameterCount++)
		{
			this->m_ParameterRanges[parameterCount]
				= COR_PRF_FUNCTION_ARGUMENT_RANGE(byteData.ranges[parameterCount]);
			if (this->m_ParameterRanges[parameterCount].startAddress != NULL)
			{
				this->m_ParameterValues[parameterCount]
					= *(UINT_PTR*)this->m_ParameterRanges[parameterCount].startAddress;
			}
		}
	}
}


const ULONG FunctionStackItem::ParameterCount()
{
	return this->m_ParameterInfo.numRanges;

}
const UINT_PTR* FunctionStackItem::ItemStackParameters()
{
	return this->m_ParameterValues;
}

void FunctionStackItem::ReturnValue(const COR_PRF_FUNCTION_ARGUMENT_RANGE& input)
{
	if (input.startAddress != NULL)
	{
		this->m_ReturnData = *(UINT_PTR*)input.startAddress;
	}

}

const UINT_PTR& FunctionStackItem::ReturnValue()
{
	return m_ReturnData;

}

const FunctionID FunctionStackItem::FunctionId()
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
	//this->m_ThreadName.assign(ThreadName);
}

const std::wstring& ThreadStackItem::ThreadName()
{
	return this->m_ThreadName;
}

void ThreadStackItem::ThreadName(const std::wstring& threadName)
{
	this->m_ThreadName.assign(threadName);
}

const ThreadID ThreadStackItem::ThreadId()
{
	return this->m_ThreadID;
}