#include <cor.h>
#include <corprof.h>

#pragma once
class CorProfilerCallbackImplementation
	: public ICorProfilerCallback5
	
{
public:
	CorProfilerCallbackImplementation();
	~CorProfilerCallbackImplementation();
};

