#pragma once
#include "stdafx.h"
#include "commonstructures.h"



ThreadStackItem::ThreadStackItem()
{

}
ThreadStackItem::ThreadStackItem(ThreadID tid, FunctionID funcId, ThreadStackReason reason, byte* byteData)
{
	this->m_ThreadID = tid;
	this->m_FunctionID = funcId;
	this->m_LastReason = reason;
	this->m_EnterTime = boost::posix_time::microsec_clock::universal_time();
	this->m_GCReasons = std::vector<COR_PRF_GC_REASON>();
	this->m_SuspensionReasons = std::vector<COR_PRF_SUSPEND_REASON>();
}


ThreadStackItem::~ThreadStackItem()
{
	this->m_GCReasons.empty();
	this->m_SuspensionReasons.empty();
}

ThreadID ThreadStackItem::ItemThreadID()
{
	return this->m_ThreadID;
}

FunctionID ThreadStackItem::ItemFunctionID()
{
	return this->m_FunctionID;
}


boost::posix_time::ptime ThreadStackItem::ItemStartTime()
{
	return this->m_EnterTime;
}

void ThreadStackItem::UpdateItemStackReason(ThreadStackReason stackReason)
{
	this->m_LastReason = stackReason;
}

byte* ThreadStackItem::ItemStackParameters()
{
	return this->m_RawParameterData;
}

void ThreadStackItem::ItemStackReturnValue(byte* input)
{
	this->m_RawReturnData = input;
}

boost::posix_time::time_duration ThreadStackItem::GCSuspensionTime()
{
	this->m_GarbageCollectionTotal = m_GarbageCollectionTimeEnd - m_GarbageCollectionTimeStart;
	return this->m_SuspensionTotal;
}

boost::posix_time::time_duration ThreadStackItem::OtherSuspensionTime()
{
	this->m_SuspensionTotal = m_RuntimeSuspensionEnd - m_RuntimeSuspensionStart;
	return this->m_SuspensionTotal;
}

boost::posix_time::time_duration ThreadStackItem::ItemRunTime()
{
	//this->m_ItemTotal = m_LeaveTime - m_EnterTime;
	return this->m_ItemTotal;
}

void ThreadStackItem::UpdateLeaveTime()
{
	this->m_ItemTotal = boost::posix_time::time_duration(boost::posix_time::microsec_clock::universal_time()
		- this->m_EnterTime);
}

boost::posix_time::time_duration ThreadStackItem::ProfilingOverhead()
{
	return this->m_ProfilingOverheadTotal;
}

int ThreadStackItem::Depth()
{
	return this->m_Depth;
}

void ThreadStackItem::Depth(int depth)
{
	this->m_Depth = depth;
}

const ThreadStackReason& ThreadStackItem::LastReason()
{
	return this->m_LastReason;
}
