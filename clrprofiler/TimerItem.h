#pragma once
#include "commonstructures.h"
#include "ThreadStackItem.h"

class TimerItem
{


public:
	TimerItem();
	TimerItem(COR_PRF_SUSPEND_REASON, SuspensionReason);
	TimerItem(COR_PRF_GC_REASON, SuspensionReason);
	TimerItem(ThreadStackReason stackReason);
	~TimerItem();
	void AddThreadStackItem(StackItemBase* stackItem);
private:
	LARGE_INTEGER startTime;
	LARGE_INTEGER endTime;
	StackItemBase* m_ThreadStackItem = NULL;
	COR_PRF_SUSPEND_REASON m_RuntimeSuspendReason = (COR_PRF_SUSPEND_REASON)INT_FAST32_MAX;
	COR_PRF_GC_REASON m_GCSuspendReason = (COR_PRF_GC_REASON)INT_FAST32_MAX;
	ThreadStackReason m_ThreadStackReason = (ThreadStackReason)INT_FAST32_MAX;
	SuspensionReason m_SuspensionAction = (SuspensionReason)INT_FAST32_MAX;

};

