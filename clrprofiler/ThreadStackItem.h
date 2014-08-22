#pragma once
#include "commonstructures.h"

class StackItemBase
{
	
public:
	friend class TimerItem;
	
	StackItemBase();
	~StackItemBase();

	ThreadID ItemThreadID();

	void UpdateItemStackReason(ThreadStackReason);
	const ThreadStackReason& LastReason();

	int Depth();
	void Depth(int depth);

	int SequenceNumber();
	void SequenceNumber(int seqNumber);

	ULONGLONG ItemRunTime();
	ULONGLONG GCSuspensionTime();
	ULONGLONG OtherSuspensionTime();
	ULONGLONG ProfilingOverhead();
	ULONGLONG ItemStartTime();
	

protected:
	void UpdateLeaveTime();
	void virtual PolyDummy();
	boost::posix_time::ptime m_StartWallTime;
	int m_SequenceNumber;
	LARGE_INTEGER m_EnterTime;
	LARGE_INTEGER m_LeaveTime;
	
	LARGE_INTEGER m_GarbageCollectionTimeStart;
	LARGE_INTEGER m_GarbageCollectionTimeEnd;

	LARGE_INTEGER m_RuntimeSuspensionStart;
	LARGE_INTEGER m_RuntimeSuspensionEnd;


	ULONGLONG m_ItemTotal;
	ULONGLONG m_GarbageCollectionTotal;
	ULONGLONG m_SuspensionTotal;
	ULONGLONG m_ProfilingOverheadTotal;

	
	ThreadStackReason m_LastReason;

	std::vector<GC_REASON> m_GCReasons;
	std::vector<COR_PRF_SUSPEND_REASON> m_SuspensionReasons;

	/* 
	Flag to tell the Network Client to send data. 
	The idea is that it's always false until something updates the times or other properties at that time it will be set to true
	and the network client will update the field to false
	*/
	BOOL m_NeedsToBeSent; 




	int m_Depth;
};

class FunctionStackItem : public StackItemBase
{
public:

	FunctionStackItem(FunctionID, ThreadStackReason, const COR_PRF_FUNCTION_ARGUMENT_INFO& byteData);
	// Array of parameters
	const UINT_PTR* ItemStackParameters();

	void ReturnValue(const COR_PRF_FUNCTION_ARGUMENT_RANGE& input);
	const UINT_PTR& ReturnValue();
	// Count of parameters
	const ULONG ParameterCount();
	const FunctionID FunctionId();
private:
	COR_PRF_FUNCTION_ARGUMENT_INFO m_ParameterInfo;
	COR_PRF_FUNCTION_ARGUMENT_RANGE m_ParameterRanges[255];
	UINT_PTR m_ReturnData;
	UINT_PTR m_ParameterValues[255]; // Created a max item here, but we should be a bit more dynamic maybe?
	FunctionID m_FunctionID;

};

class ThreadStackItem : public StackItemBase
{
public:
	ThreadStackItem(ThreadID threadId, ThreadStackReason reason);
	// Thread name getter and setter
	const std::wstring& ThreadName();
	void ThreadName(const std::wstring&);
	const ThreadID ThreadId();
private:
	std::wstring m_ThreadName;
	ThreadID m_ThreadID;
};

class RuntimeSuspensionStackItem : public StackItemBase
{
public:
	RuntimeSuspensionStackItem(COR_PRF_SUSPEND_REASON suspensionReason);
	COR_PRF_SUSPEND_REASON SuspensionReason;
};

class GarbageCollectionStackItem : public StackItemBase
{
public:
	GarbageCollectionStackItem(COR_PRF_GC_REASON suspensionReason, int maxGenerationCollected);
	GC_REASON GetGCReason();
private:
	COR_PRF_GC_REASON m_GCReason;
	int m_MaxGenerationsCollected;

};
