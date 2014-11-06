#include "stdafx.h"
#include "stackitems.h"


GarbageCollectionStackItem::GarbageCollectionStackItem(COR_PRF_GC_REASON suspensionReason, int maxGenerationCollected) : StackItemBase()
{
	// Set the GC reason
	this->m_GCReason = suspensionReason;
	this->m_MaxGenerationsCollected = m_MaxGenerationsCollected;
}

std::shared_ptr<std::vector<BYTE>> GarbageCollectionStackItem::SendPacket()
{
	auto vec = std::vector < BYTE >();
	auto sharedvec = std::make_shared<std::vector<BYTE>>(vec);
	return sharedvec;
}