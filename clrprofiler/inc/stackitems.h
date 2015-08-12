#pragma once
#include "../../metadatastaticlib/inc/commonstructures.h"


class IStackItem
{
public:
	virtual ~IStackItem() {};
};

// Base item entry for all stack objects. This provides a consistent way to access the data.
class StackItemBase : public IStackItem
{

public:

	StackItemBase();
	StackItemBase(ThreadID thread, ThreadStackReason reason);
	~StackItemBase() {};

	ThreadStackReason& LastReason() const;

	// Gets the current stack depth. Used primarily for stack recreation.
	int Depth() const;
	void Depth(int depth);

	// Gets the sequence number of the stack item. Used for identification when rebuilding the stack.
	int SequenceNumber() const;
	void SequenceNumber(int seqNumber);

	// Gets the ThreadID
	ThreadID ItemThreadID() const;

	// The wall clock time of this item start
	SYSTEMTIME ItemStartTime() const;

	// The wall clock time of this item start
	LARGE_INTEGER ItemProfilingTime() const;

	// Returns the text representation of the agent
	std::wstring& AgentID() const;

	// Returns the reason, or the type of thread stack item
	const ThreadStackReason& Reason();


private:
	SYSTEMTIME								m_StartWallTime;	// Wall clock time
	LARGE_INTEGER							m_EnterTime;	    // GetPerfCounter() time
	ThreadStackReason						m_Reason;			// Reason for creating this stack item
	int										m_Depth;			// Current Stack Depth
	int										m_SequenceNumber;			// Current Stack sequence
	ThreadID								m_Thread;			// Current thread
	std::wstring							m_AgentID;			// Reporting agent

	std::shared_ptr<std::vector<BYTE>> PacketHeader();
};

// Defines a terse version of a function on the stack. The pointers are used to look up more verbose information.
class FunctionStackItem : public StackItemBase
{
public:

	FunctionStackItem(ThreadID threadId, ThreadStackReason reason, FunctionID functionId, UINT_PTR returnValue);
	FunctionStackItem(ThreadID threadId, ThreadStackReason reason, FunctionID functionId, std::shared_ptr<std::vector<UINT_PTR>>& functionArguments);
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
private:
	std::wstring m_ThreadName;
};

// A notification stack item that is used in suspension calculation
class RuntimeSuspensionStackItem : public StackItemBase
{
public:
	RuntimeSuspensionStackItem(COR_PRF_SUSPEND_REASON suspensionReason);
private:
	COR_PRF_SUSPEND_REASON m_SuspensionReason;
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

