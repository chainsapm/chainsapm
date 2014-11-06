#pragma once
#include "stdafx.h"
#include "TimerItem.h"
#include "stackitems.h"


TimerItem::TimerItem()
{
	QueryPerformanceCounter(&this->startTime);
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
				QueryPerformanceCounter(&this->m_ThreadStackItem->m_GarbageCollectionTimeStart);
				 
			}
			else if (this->m_SuspensionAction == SUSPEND_END)
			{
				QueryPerformanceCounter(&this->m_ThreadStackItem->m_GarbageCollectionTimeEnd);
				m_ThreadStackItem->m_GarbageCollectionTotal +=
					(m_ThreadStackItem->m_GarbageCollectionTimeEnd.QuadPart - m_ThreadStackItem->m_GarbageCollectionTimeStart.QuadPart);
			}
		}
		else if (m_RuntimeSuspendReason != INT_FAST32_MAX)
		{
			if (this->m_SuspensionAction == SUSPEND_START)
			{
				QueryPerformanceCounter(&this->m_ThreadStackItem->m_RuntimeSuspensionStart);
			}
			else if (this->m_SuspensionAction == SUSPEND_END)
			{
				QueryPerformanceCounter(&this->m_ThreadStackItem->m_RuntimeSuspensionEnd);
				m_ThreadStackItem->m_SuspensionTotal +=
					(m_ThreadStackItem->m_RuntimeSuspensionEnd.QuadPart - m_ThreadStackItem->m_RuntimeSuspensionStart.QuadPart);
			}
		}
		else {
			if ((this->m_ThreadStackReason == EXIT) | (this->m_ThreadStackReason == TAIL))
			{
				m_ThreadStackItem->UpdateLeaveTime();
			}
		}

		// This is always last since EVERYTHING is additional overhead
		LARGE_INTEGER overheadTotal;
		QueryPerformanceCounter(&overheadTotal);
		this->m_ThreadStackItem->m_ProfilingOverheadTotal += overheadTotal.QuadPart - this->startTime.QuadPart;

	}

}

void TimerItem::AddThreadStackItem(StackItemBase* stackItem)
{
	this->m_ThreadStackItem = stackItem;
	this->m_ThreadStackItem->UpdateItemStackReason(this->m_ThreadStackReason);

}

