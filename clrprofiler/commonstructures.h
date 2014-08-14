#ifndef COMMONSTRUCT
#define COMMONSTRUCT

#include <cor.h>
#include <corprof.h>
#include <string>
#include <map>
#include <stack>
#include <boost/date_time.hpp>

#define NAME_BUFFER_SIZE 2048

enum ThreadStackReason {
	ENTER,
	EXIT,
	TAIL,
	THREAD_START,
	THREAD_END
};

enum SuspensionReason
{
	SUSPEND_START,
	SUSPEND_END
};

struct GC_REASON
{
	GC_REASON(COR_PRF_GC_REASON reason, int maxGenerationCollected)
	{
		this->reason = reason;
		this->maxGenerationCollected = maxGenerationCollected;
	}
	COR_PRF_GC_REASON reason;
	int maxGenerationCollected;
};




#endif