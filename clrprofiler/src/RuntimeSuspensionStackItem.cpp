#include "stdafx.h"
#include "stackitems.h"


// Generic runtime suspension creation
RuntimeSuspensionStackItem::RuntimeSuspensionStackItem(COR_PRF_SUSPEND_REASON suspensionReason) : StackItemBase()
{
	// Set the suspend reason
	this->m_SuspensionReason = suspensionReason;
}
