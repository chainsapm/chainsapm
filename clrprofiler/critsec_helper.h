#pragma once
class critsec_helper
{
public:
	critsec_helper(CRITICAL_SECTION *critSec);
	~critsec_helper();
	void leave_early();
private:
	CRITICAL_SECTION * m_CritSec;
	BOOL m_LeftEarly;
};

