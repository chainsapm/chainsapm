#include "stdafx.h"
#include "ContainerClass.h"
#include "FunctionInfo.h"
#include "ThreadStackItem.h"






BOOL ItemMapping::operator== (const std::wstring &strCompare)
{
	return !(this->ItemName.compare(strCompare));
}

ContainerClass::ContainerClass()
{
	// TODO: Issue #2 Add more containers to properly handle a proper entry point.

	this->g_FunctionSet = new std::map<FunctionID, FunctionInfo*, std::less<FunctionID>, ALLOC_500<std::pair<FunctionID, FunctionInfo*>>>(); // Start with allowing 
	this->g_ThreadStackMap = new std::map<ThreadID, std::deque<StackItemBase*, ALLOC_500<StackItemBase*>>>();
	this->g_EntryPointStackMap = new std::map<LONGLONG, std::deque<StackItemBase*, ALLOC_500<StackItemBase*>>>();
	this->g_ThreadEntrypointID = new std::map<ThreadID, ULONGLONG>();
	this->g_FunctionNameSet = new std::unordered_set < ItemMapping*, Hasher, EqualFn>();
	this->g_ClassNameSet = new std::unordered_set<std::wstring>();
	this->g_ThreadStackDepth = new std::map<ThreadID, volatile unsigned int>();
	this->g_ThreadStackSequence = new std::map<ThreadID, volatile unsigned int>();
	// CRITICAL 1 Thread synchronization for Function Mapper
	InitializeCriticalSection(&this->g_FunctionSetCriticalSection);
	InitializeCriticalSection(&this->g_ThreadingCriticalSection);
	InitializeCriticalSection(&this->g_ThreadStackDepthCriticalSection);
	InitializeCriticalSection(&this->g_ThreadStackSequenceCriticalSection);
}