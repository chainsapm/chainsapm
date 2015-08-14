#include "stdafx.h"
#include "critsec_helper.h"


critsec_helper::critsec_helper(CRITICAL_SECTION *critSec)
{
	this->m_CritSec = critSec;
	EnterCriticalSection(this->m_CritSec);
}


critsec_helper::~critsec_helper()
{
	if (m_LeftEarly == FALSE)
	{
		LeaveCriticalSection(this->m_CritSec);
	}
}

void critsec_helper::leave_early()
{
	if (m_LeftEarly == FALSE)
	{
		LeaveCriticalSection(this->m_CritSec);
		this->m_LeftEarly = TRUE;
	}
}