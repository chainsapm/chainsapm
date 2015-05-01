#pragma once

class srw_helper
{
public:
	static enum class LockType
	{
		SHARED,
		EXCLUSIVE
	} SRWLockType;

	srw_helper(const SRWLOCK &srwlock, LockType type);
	~srw_helper();
private:
	SRWLOCK m_SRWLock;
	LockType m_SRWLockType;
	
};

