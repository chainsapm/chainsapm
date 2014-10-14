#pragma once
#include "commonstructures.h"

// Base item entry for all stack objects. This provides a consistent way to access the data.
class StackItemBase
{

public:

	StackItemBase();
	StackItemBase(int depth, int sequence, ThreadID thread, ThreadStackReason reason);
	~StackItemBase();

	virtual  ThreadStackReason& LastReason() const;

	// Gets the current stack depth. Used primarily for stack recreation.
	virtual int Depth() const;

	// Gets the sequence number of the stack item. Used for identification when rebuilding the stack.
	virtual int SequenceNumber() const;

	// Gets the ThreadID
	virtual ThreadID ItemThreadID() const;

	// The wall clock time of this item start
	virtual SYSTEMTIME ItemStartTime() const;

	// The wall clock time of this item start
	virtual LARGE_INTEGER ItemProfilingTime() const;

	// Returns the text representation of the agent
	std::wstring& AgentID() const;


protected:



	SYSTEMTIME			m_StartWallTime;	// Wall clock time
	LARGE_INTEGER		m_ProfilingTime;	// GetPerfCounter() time
	ThreadStackReason	m_Reason;			// Reason for creating this stack item
	int					m_Depth;			// Current Stack Depth
	int					m_Sequence;			// Current Stack sequence
	ThreadID			m_Thread;			// Current thread
	std::wstring		m_AgentID;			// Reporting agent
};

// Defines a terse version of a function on the stack. The pointers are used to look up more verbose information.
class FunctionStackItem : public StackItemBase
{
public:

	FunctionStackItem(FunctionID, ThreadStackReason, std::shared_ptr<std::vector<UINT_PTR>>& byteData);
	~FunctionStackItem();
	// Array of parameters
	const std::shared_ptr<std::vector<UINT_PTR>>& ItemStackParameters() const;

	void ReturnValue(const UINT_PTR& input);
	UINT_PTR ReturnValue() const;
	// Count of parameters
	ULONG ParameterCount() const;
	FunctionID FunctionId() const;
private:
	COR_PRF_FUNCTION_ARGUMENT_INFO m_ParameterInfo;
	COR_PRF_FUNCTION_ARGUMENT_RANGE * m_ParameterRanges;
	UINT_PTR m_ReturnData;
	//UINT_PTR * m_ParameterValues; // TODO validate this works, Created a max item here, but we should be a bit more dynamic maybe?
	std::shared_ptr<std::vector<UINT_PTR>> m_ParameterValues;
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
