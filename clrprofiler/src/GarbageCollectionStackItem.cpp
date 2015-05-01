#include "stdafx.h"
#include "stackitems.h"


GarbageCollectionStackItem::GarbageCollectionStackItem(COR_PRF_GC_REASON suspensionReason, int maxGenerationCollected) : StackItemBase()
{
	// Set the GC reason
	this->m_GCReason = suspensionReason;
	this->m_MaxGenerationsCollected = m_MaxGenerationsCollected;
}
