#pragma once
#include "commonstructures.h"

// Base item entry for all stack objects. This provides a consistent way to access the data.
class StackItemBase
{
	
public:
	friend class TimerItem;
	
	StackItemBase();
	~StackItemBase();

	ThreadID ItemThreadID();

	void UpdateItemStackReason(ThreadStackReason);
	const ThreadStackReason& LastReason();

	// Gets the current stack depth. Used primarily for stack recreation.
	int Depth() const;
	// Sets the current stack depth.
	void Depth(int depth);

	// Gets the sequence number of the stack item. Used for identification when rebuilding the stack.
	int SequenceNumber() const;
	// Sets the sequence number of the stack item.
	void SequenceNumber(int seqNumber);

	// Total calculated run time of this stack item.
	ULONGLONG ItemRunTime() const;
	// Total calculated GC caused suspension time of this stack item.
	ULONGLONG GCSuspensionTime() const;
	// Total calculated suspension time of this stack item. Does not include GC.
	ULONGLONG OtherSuspensionTime() const;
	// Total calculated overhead added by the profiler.
	ULONGLONG ProfilingOverhead() const;
	// The GMT wall clock time of this item start
	ULONGLONG ItemStartTime() const;
	

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
	and the network client will update the field to false.
	*/
	BOOL m_NeedsToBeSent; 

	int m_Depth;
};

// Defines a terse version of a function on the stack. The pointers are used to look up more verbose information.
class FunctionStackItem : public StackItemBase
{
public:

	FunctionStackItem(FunctionID, ThreadStackReason, const COR_PRF_FUNCTION_ARGUMENT_INFO& byteData);
	~FunctionStackItem();
	// Array of parameters
	const UINT_PTR* ItemStackParameters() const;

	void ReturnValue(const COR_PRF_FUNCTION_ARGUMENT_RANGE& input);
	UINT_PTR ReturnValue() const;
	// Count of parameters
	ULONG ParameterCount() const;
	FunctionID FunctionId() const;
private:
	COR_PRF_FUNCTION_ARGUMENT_INFO m_ParameterInfo;
	COR_PRF_FUNCTION_ARGUMENT_RANGE * m_ParameterRanges;
	UINT_PTR m_ReturnData;
	UINT_PTR * m_ParameterValues; // Created a max item here, but we should be a bit more dynamic maybe?
	FunctionID m_FunctionID;

};

// This class defines the "header" or first item in the thread stack
class ThreadStackItem : public StackItemBase
{
public:
	ThreadStackItem(ThreadID threadId, ThreadStackReason reason);
	// Thread name getter and setter
	const std::wstring& ThreadName() const;
	void ThreadName(const std::wstring&);
	ThreadID ThreadId() const;
private:
	std::wstring m_ThreadName;
	ThreadID m_ThreadID;
};

// A notification stack item that is used in suspension calculation
class RuntimeSuspensionStackItem : public StackItemBase
{
public:
	RuntimeSuspensionStackItem(COR_PRF_SUSPEND_REASON suspensionReason);
	COR_PRF_SUSPEND_REASON SuspensionReason;
};

// A notification stack item that is used in GC caused suspension calculation
class GarbageCollectionStackItem : public StackItemBase
{
public:
	GarbageCollectionStackItem(COR_PRF_GC_REASON suspensionReason, int maxGenerationCollected);
	GC_REASON GetGCReason() const;
private:
	COR_PRF_GC_REASON m_GCReason;
	int m_MaxGenerationsCollected;

};
