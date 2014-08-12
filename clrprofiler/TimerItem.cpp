
#pragma once
#include "stdafx.h"
#include "commonstructures.h"






TimerItem::TimerItem()
{
	this->startTime = boost::posix_time::microsec_clock::universal_time();
}

TimerItem::TimerItem(COR_PRF_SUSPEND_REASON reason, SuspensionReason suspensionReason) : TimerItem()
{
	this->m_RuntimeSuspendReason = reason;
	this->m_SuspensionAction = suspensionReason;
}

TimerItem::TimerItem(COR_PRF_GC_REASON reason, SuspensionReason suspensionReason) : TimerItem()
{
	this->m_GCSuspendReason = reason;
	this->m_SuspensionAction = suspensionReason;
}

TimerItem::TimerItem(ThreadStackReason stackReason) : TimerItem()
{
	this->m_ThreadStackReason = stackReason;
}

TimerItem::~TimerItem()
{
	if (m_ThreadStackItem != NULL)
	{
		if (m_GCSuspendReason != INT_FAST32_MAX)
		{
			if (this->m_SuspensionAction == SUSPEND_START)
			{
				m_ThreadStackItem->m_GarbageCollectionTimeStart = boost::posix_time::microsec_clock::universal_time();
			}
			else if (this->m_SuspensionAction == SUSPEND_END)
			{
				m_ThreadStackItem->m_GarbageCollectionTimeEnd = boost::posix_time::microsec_clock::universal_time();
				m_ThreadStackItem->m_GarbageCollectionTotal +=
					(m_ThreadStackItem->m_GarbageCollectionTimeEnd - m_ThreadStackItem->m_GarbageCollectionTimeStart);
			}
		}
		else if (m_RuntimeSuspendReason != INT_FAST32_MAX)
		{
			if (this->m_SuspensionAction == SUSPEND_START)
			{
				m_ThreadStackItem->m_RuntimeSuspensionStart = boost::posix_time::microsec_clock::universal_time();
			}
			else if (this->m_SuspensionAction == SUSPEND_END)
			{
				m_ThreadStackItem->m_RuntimeSuspensionEnd = boost::posix_time::microsec_clock::universal_time();
				m_ThreadStackItem->m_SuspensionTotal +=
					(m_ThreadStackItem->m_RuntimeSuspensionEnd - m_ThreadStackItem->m_RuntimeSuspensionStart);
			}
		}
		else {
			if ((this->m_ThreadStackReason == EXIT) | (this->m_ThreadStackReason == TAIL))
			{
				m_ThreadStackItem->UpdateLeaveTime();
			}
		}

		// This is always last since EVERYTHING is additional overhead
		this->m_ThreadStackItem->m_ProfilingOverheadTotal +=
			(boost::posix_time::microsec_clock::universal_time() - this->startTime);

	}

}

void TimerItem::AddThreadStackItem(ThreadStackItem* stackItem)
{
	this->m_ThreadStackItem = stackItem;
	this->m_ThreadStackItem->UpdateItemStackReason(this->m_ThreadStackReason);

}

