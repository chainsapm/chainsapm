#ifndef COMMONSTRUCT
#define COMMONSTRUCT

#include <cor.h>
#include <corprof.h>
#include <string>
#include <map>
#include <stack>
#include <boost/date_time.hpp>

enum ThreadStackReason {
	ENTER,
	EXIT,
	TAIL
};

enum SuspensionReason
{
	SUSPEND_START,
	SUSPEND_END
};

class ThreadStackItem
{
	
public:
	friend class TimerItem;
	ThreadStackItem(ThreadID, FunctionID, ThreadStackReason, byte* byteData);
	ThreadStackItem();
	~ThreadStackItem();

	ThreadID ItemThreadID();
	FunctionID ItemFunctionID();

	void UpdateItemStackReason(ThreadStackReason);
	const ThreadStackReason& LastReason();
	byte* ItemStackParameters();
	void ItemStackReturnValue(byte* input);

	int Depth();
	void Depth(int depth);

	boost::posix_time::time_duration ItemRunTime();
	boost::posix_time::time_duration GCSuspensionTime();
	boost::posix_time::time_duration OtherSuspensionTime();
	boost::posix_time::time_duration ProfilingOverhead();
	boost::posix_time::ptime ItemStartTime();
private:
	void UpdateLeaveTime();
	boost::posix_time::ptime m_EnterTime;
	boost::posix_time::ptime m_LeaveTime;
	
	boost::posix_time::ptime m_GarbageCollectionTimeStart;
	boost::posix_time::ptime m_GarbageCollectionTimeEnd;

	boost::posix_time::ptime m_RuntimeSuspensionStart;
	boost::posix_time::ptime m_RuntimeSuspensionEnd;


	boost::posix_time::time_duration m_ItemTotal;
	boost::posix_time::time_duration m_GarbageCollectionTotal;
	boost::posix_time::time_duration m_SuspensionTotal;
	boost::posix_time::time_duration m_ProfilingOverheadTotal;

	ThreadID m_ThreadID;
	FunctionID m_FunctionID;

	ThreadStackReason m_LastReason;

	std::vector<COR_PRF_GC_REASON> m_GCReasons;
	std::vector<COR_PRF_SUSPEND_REASON> m_SuspensionReasons;

	byte* m_RawParameterData;
	byte* m_RawReturnData;

	int m_Depth;
};

class ParameterInfo
{
public:
	ParameterInfo();
	~ParameterInfo();

	// Getter and setter for the parameter name
	const std::wstring& ParameterName();
	void ParameterName(const std::wstring&);

	// Getter and setter for the type string
	// This is mainly used as informational data when generating the function signature
	const std::wstring& ParameterTypeString();
	void  ParameterTypeString(const std::wstring&);

	// Getter and setter for the parameter type
	// this is used to extract the value from the args list
	// it is also used to set the format string for the ParameterValue
	const CorElementType& ParameterType();
	void ParameterType(CorElementType);

	// Getter and setter for the parameter value
	// this will really only be called from the Enter function and should
	// not be assumed not null
	const std::wstring& ParameterValue();
	void ParameterValue(std::wstring);



private:
	std::wstring m_ParameterName;
	std::wstring m_ParameterTypeString;
	std::wstring m_ParameterValue;
	CorElementType m_ReturnType;
	size_t m_ParameterSize;
};

class FunctionInfo
{
public:
	FunctionInfo();
	~FunctionInfo();

	// Getter and setter for class name
	const std::wstring& ClassName();
	void ClassName(const std::wstring&);

	// Getter and setter for the function name
	const std::wstring& FunctionName();
	void FunctionName(const std::wstring&);

	// Getter and setter for the function ID
	const FunctionID& FunctionId();
	void FunctionId(FunctionID);

	// Getter for generated signature string
	const std::wstring&  SignatureString();

	const PCCOR_SIGNATURE& SignatureRaw();
	void SignatureRaw(const PCCOR_SIGNATURE&);

	// Getter and setter for the return type
	const std::wstring& ReturnType();
	void ReturnType(const std::wstring&);

	// Getter and setter for type name
	const std::wstring& CallingConvention();
	void CallingConvention(const std::wstring&);

	const std::vector<ParameterInfo>& Parameters();
	void AddParameters(const ParameterInfo& addParameter);

private:
	std::wstring m_ClassName;
	std::wstring m_FunctionName;
	FunctionID m_FunctionID;
	PCCOR_SIGNATURE m_RawSignature;
	std::wstring m_ReturnType;
	std::wstring m_SignatureString;
	std::wstring m_CallingConvention;
	std::vector<ParameterInfo> m_Parameters;
};

class TimerItem
{


public:
	TimerItem();
	TimerItem(COR_PRF_SUSPEND_REASON, SuspensionReason);
	TimerItem(COR_PRF_GC_REASON, SuspensionReason);
	TimerItem(ThreadStackReason stackReason);
	~TimerItem();
	void AddThreadStackItem(ThreadStackItem* stackItem);
private:
	boost::posix_time::ptime startTime;
	boost::posix_time::ptime endTime;
	ThreadStackItem* m_ThreadStackItem;
	COR_PRF_SUSPEND_REASON m_RuntimeSuspendReason = (COR_PRF_SUSPEND_REASON)INT_FAST32_MAX;
	COR_PRF_GC_REASON m_GCSuspendReason = (COR_PRF_GC_REASON)INT_FAST32_MAX;
	ThreadStackReason m_ThreadStackReason = (ThreadStackReason)INT_FAST32_MAX;
	SuspensionReason m_SuspensionAction = (SuspensionReason)INT_FAST32_MAX;

};
#endif