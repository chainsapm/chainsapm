#pragma once
#include "stdafx.h"
#include "commonstructures.h"



#define NAME_BUFFER_SIZE 2048

ParameterInfo::ParameterInfo()
{

}

ParameterInfo::~ParameterInfo()
{

}

const std::wstring& ParameterInfo::ParameterName()
{
	return this->m_ParameterName;
}

void ParameterInfo::ParameterName(const std::wstring& string)
{
	this->m_ParameterName.assign(string);
}

const std::wstring& ParameterInfo::ParameterTypeString()
{
	return this->m_ParameterTypeString;
}

void ParameterInfo::ParameterTypeString(const std::wstring& paramTypeString)
{
	this->m_ParameterTypeString.assign(paramTypeString);
}

const CorElementType& ParameterInfo::ParameterType()
{
	return this->m_ReturnType;
}

void ParameterInfo::ParameterType(CorElementType elementType)
{
	this->m_ReturnType = elementType;
}


FunctionInfo::FunctionInfo()
{

}

FunctionInfo::~FunctionInfo()
{

}

const std::wstring& FunctionInfo::ClassName()
{
	return this->m_ClassName;
}

void FunctionInfo::ClassName(const std::wstring& className)
{
	this->m_ClassName.assign(className);
}

const std::wstring& FunctionInfo::FunctionName()
{
	return this->m_FunctionName;
}

void FunctionInfo::FunctionName(const std::wstring& functionName)
{
	this->m_FunctionName.assign(functionName);
}

const FunctionID& FunctionInfo::FunctionId()
{
	return this->m_FunctionID;
}

void FunctionInfo::FunctionId(FunctionID funcID)
{
	this->m_FunctionID = funcID;
}

const std::wstring&  FunctionInfo::SignatureString()
{
	if (this->m_SignatureString.empty())
	{
		this->m_SignatureString.append(this->m_ReturnType);
		this->m_SignatureString.append(TEXT(" "));
		this->m_SignatureString.append(this->m_ClassName);
		this->m_SignatureString.append(TEXT("::"));
		this->m_SignatureString.append(this->m_FunctionName);
		this->m_SignatureString.append(TEXT("("));
		for (std::vector<ParameterInfo>::iterator it = m_Parameters.begin(); it != this->m_Parameters.end(); it++)
		{

			this->m_SignatureString.append(it->ParameterTypeString());
			this->m_SignatureString.append(TEXT(" "));
			this->m_SignatureString.append(it->ParameterName());
			if (it + 1 != m_Parameters.end())
			{
				this->m_SignatureString.append(TEXT(", "));
			}
		}
		this->m_SignatureString.append(TEXT(")"));
	}
	return this->m_SignatureString;
}

const std::wstring& FunctionInfo::ReturnType()
{
	return this->m_ReturnType;
}

void FunctionInfo::ReturnType(const std::wstring& returnType)
{
	this->m_ReturnType.assign(returnType);
}

const std::wstring& FunctionInfo::CallingConvention()
{
	return this->m_CallingConvention;
}

void FunctionInfo::CallingConvention(const std::wstring& callingConvention)
{
	this->m_CallingConvention.assign(callingConvention);
}

const PCCOR_SIGNATURE& FunctionInfo::SignatureRaw()
{
	return this->m_RawSignature;
}

void FunctionInfo::SignatureRaw(const PCCOR_SIGNATURE& rawSig)
{
	this->m_RawSignature = rawSig;
}

const std::vector<ParameterInfo>& FunctionInfo::Parameters()
{
	return this->m_Parameters;
}

void FunctionInfo::AddParameters(const ParameterInfo& addParameter)
{
	this->m_Parameters.push_back(addParameter);
}

ThreadStackItem::ThreadStackItem()
{

}
ThreadStackItem::ThreadStackItem(ThreadID tid, FunctionID funcId, ThreadStackReason reason, byte* byteData)
{
	this->m_ThreadID = tid;
	this->m_FunctionID = funcId;
	this->m_LastReason = reason;
	this->m_EnterTime = boost::posix_time::microsec_clock::universal_time();
}


ThreadStackItem::~ThreadStackItem()
{

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

void ThreadStackItem::UpdateItemStackReason(ThreadStackReason)
{

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

TimerItem::TimerItem()
{
	this->startTime = boost::posix_time::microsec_clock::universal_time();
}

TimerItem::TimerItem(COR_PRF_SUSPEND_REASON reason, SuspensionReason suspensionReason) : TimerItem()
{
	this->m_RuntimeSuspendReason = reason;
	this->m_SuspensionAction = suspensionReason;
}

TimerItem::TimerItem(COR_PRF_GC_REASON reason, SuspensionReason suspensionReason) : TimerItem()
{
	this->m_GCSuspendReason = reason;
	this->m_SuspensionAction = suspensionReason;
}

TimerItem::TimerItem(ThreadStackReason stackReason) : TimerItem()
{
	this->m_ThreadStackReason = stackReason;
}

TimerItem::~TimerItem()
{
	if (m_ThreadStackItem != NULL)
	{
		if (m_GCSuspendReason != INT_FAST32_MAX)
		{
			if (this->m_SuspensionAction = SUSPEND_START)
			{
				m_ThreadStackItem->m_GarbageCollectionTimeStart = boost::posix_time::microsec_clock::universal_time();
			}
			else if (this->m_SuspensionAction = SUSPEND_END)
			{
				m_ThreadStackItem->m_GarbageCollectionTimeEnd = boost::posix_time::microsec_clock::universal_time();
				m_ThreadStackItem->m_GarbageCollectionTotal +=
					(m_ThreadStackItem->m_GarbageCollectionTimeEnd - m_ThreadStackItem->m_GarbageCollectionTimeStart);
			}
		}
		else if (m_RuntimeSuspendReason != INT_FAST32_MAX)
		{
			if (this->m_SuspensionAction = SUSPEND_START)
			{
				m_ThreadStackItem->m_RuntimeSuspensionStart = boost::posix_time::microsec_clock::universal_time();
			}
			else if (this->m_SuspensionAction = SUSPEND_END)
			{
				m_ThreadStackItem->m_RuntimeSuspensionEnd = boost::posix_time::microsec_clock::universal_time();
				m_ThreadStackItem->m_SuspensionTotal += 
					(m_ThreadStackItem->m_RuntimeSuspensionEnd - m_ThreadStackItem->m_RuntimeSuspensionStart);
			}
		}
		else {
			if (this->m_ThreadStackReason = EXIT)
			{
				m_ThreadStackItem->UpdateLeaveTime();
			}
		}

		// This is always last since EVERYTHING is additional overhead
		this->m_ThreadStackItem->m_ProfilingOverheadTotal += 
			(boost::posix_time::microsec_clock::universal_time() - this->startTime);

	}
	
}

void TimerItem::AddThreadStackItem(ThreadStackItem* stackItem)
{
	this->m_ThreadStackItem = stackItem;
	
}

