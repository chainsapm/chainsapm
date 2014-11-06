#include "stdafx.h"
#include "stackitems.h"


// Generic runtime suspension creation
RuntimeSuspensionStackItem::RuntimeSuspensionStackItem(COR_PRF_SUSPEND_REASON suspensionReason) : StackItemBase()
{
	// Set the suspend reason
	this->m_SuspensionReason = suspensionReason;
}

std::shared_ptr<std::vector<BYTE>> RuntimeSuspensionStackItem::SendPacket()
{
	auto vec = std::vector < BYTE >();
	auto sharedvec = std::make_shared<std::vector<BYTE>>(vec);
	return sharedvec;
}
