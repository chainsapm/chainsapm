#include "stdafx.h"
#include "srw_helper.h"


srw_helper::srw_helper(const SRWLOCK &srwlock, LockType type)
{
	this->m_SRWLock = srwlock;
	switch (type)
	{
	case srw_helper::LockType::SHARED:
		AcquireSRWLockShared(&this->m_SRWLock);
		break;
	case srw_helper::LockType::EXCLUSIVE:
		AcquireSRWLockExclusive(&this->m_SRWLock);
		break;
	default:
		break;
	}
	
}


srw_helper::~srw_helper()
{
	switch (this->m_SRWLockType)
	{
	case srw_helper::LockType::SHARED:
		ReleaseSRWLockShared(&this->m_SRWLock);
		break;
	case srw_helper::LockType::EXCLUSIVE:
		ReleaseSRWLockExclusive(&this->m_SRWLock);
		break;
	default:
		break;
	}
}
