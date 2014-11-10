#include "stdafx.h"
#include "stackitems.h"

//
FunctionStackItem::FunctionStackItem(int depth, int sequence, ThreadID threadId, 
	ThreadStackReason reason, FunctionID funcId, std::shared_ptr<std::vector<UINT_PTR>>& byteData) 
	: FunctionStackItem(depth, sequence, threadId, reason, funcId, NULL)
{
	this->m_ParameterValues.swap(byteData);
}

FunctionStackItem::FunctionStackItem(int depth, int sequence, ThreadID threadId,
	ThreadStackReason reason, FunctionID funcId, UINT_PTR returnValue)
	: StackItemBase(depth, sequence, threadId, reason)
{
	this->m_FunctionID = funcId;
	this->m_ReturnData = returnValue;
}


FunctionStackItem::~FunctionStackItem()
{
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

std::shared_ptr<std::vector<BYTE>> FunctionStackItem::SendPacket()
{
	auto vec = std::vector < BYTE >();
	auto sharedvec = std::make_shared<std::vector<BYTE>>(vec);
	return sharedvec;
}