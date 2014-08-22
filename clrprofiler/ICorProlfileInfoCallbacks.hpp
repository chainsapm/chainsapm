#pragma once
#include "stdafx.h"
#include "profilermain.h"

#define NAME_BUFFER_SIZE 2048

// Global implementation of the profiler callback. Will eventually create a structure to hold multiple
// for the SxS operation required for concurrent CLR hosting/
Cprofilermain * g_ProfilerCallback = NULL;
// Global implementation of the 
MetadataHelpers * g_MetadataHelpers = NULL;

// As a function is mapped we want to keep a reference to it's specific details so we can 
// use it again when generating the call stack.
std::map<FunctionID, FunctionInfo> * g_FunctionSet = NULL;
// Holds pointers to StackItemBase polymorphic class. This class is an extensible map of objects
// that describe the state of this thread.
std::map<ThreadID, std::queue<StackItemBase*>> * g_ThreadStackMap = NULL;
// Holds the current depth of the stack based on the Enter / Leave / Tail calls
std::map<ThreadID, volatile unsigned int> * g_ThreadStackDepth = NULL;
// Holds the current sequence of the stack items
std::map<ThreadID, volatile unsigned int> * g_ThreadStackSequence = NULL;
// Holds the current number of functions that we've entered and exited. This will be used to stop
// further additions to the stack. This really won't change performance too drastically, however it
// will allow us to save memory
std::map<ThreadID, volatile unsigned int> * g_ThreadFunctionCount = NULL;
// This map is used to link together two threads as a parent and child. 
std::map<ThreadID, UINT_PTR> * g_ThreadSpawnMap = NULL;

// This is a structure to hold two or more instances of the Cprofilermain class that will
// spawn the ICorProfiler* classes. This is used in instances where a SxS scenario is active
// As of 08/16/2014 we have not implemented this.
std::map<DWORD, Cprofilermain*> * g_InstanceMap = NULL;

// **************************************** TESTING CLASSES ****************************************
// This collection is used to validate that we should be mapping this function. As of right now,
// for testing, we have this in both the mapper function and the ELT functions. Once we're out of
// the testing phase this collection will only be used in the mapper and we can remove the extraneous
// call to the find() method in the hooks.
std::unordered_set<std::wstring> * g_FunctionNameSet = NULL;
// This collection is used to validate that we should be mapping this entire class. As of right now,
// for testing, we have this in both the mapper function and the ELT functions. Once we're out of
// the testing phase this collection will only be used in the mapper and we can remove the extraneous
// call to the find() method in the hooks.
std::unordered_set<std::wstring> * g_ClassNameSet = NULL;
// **************************************** TESTING CLASSES ****************************************

// A naive critical section, as the platform develops we will create a few more CS structures that
// will block for certain operations and we will have to assume other operations are atomic. The more
// we divide the read/write operations up the less locking we will have to do.
CRITICAL_SECTION g_ThreadingCriticalSection;

// Used for testing large amounts of data and threading.
#define ALLMETHODS

EXTERN_C void FunctionEnter2_Wrapper_x64(FunctionID funcId, UINT_PTR clientData, COR_PRF_FRAME_INFO func, COR_PRF_FUNCTION_ARGUMENT_RANGE *argumentInfo);

EXTERN_C void FunctionLeave2_Wrapper_x64(FunctionID funcId, UINT_PTR clientData, COR_PRF_FRAME_INFO func, COR_PRF_FUNCTION_ARGUMENT_RANGE *argumentInfo);

EXTERN_C void FunctionTail2_Wrapper_x64(FunctionID funcId, UINT_PTR clientData, COR_PRF_FRAME_INFO func);

// Static methods for specific profiler callbacks. Namely the Mapper and Enter/Leave/Tail
namespace StaticProfilerMethods
{

	// Mapper function to create the collection of FunctionInfo items
	UINT_PTR __stdcall Mapper1(FunctionID funcId, BOOL *pbHookFunction)
	{
#ifdef ALLMETHODS
		EnterCriticalSection(&g_ThreadingCriticalSection);
		FunctionInfo funcInfo;
		g_MetadataHelpers->GetFunctionInformation(funcId, &funcInfo);
		g_FunctionSet->insert(std::pair<FunctionID, FunctionInfo>(funcId, funcInfo));
		*pbHookFunction = TRUE;
		LeaveCriticalSection(&g_ThreadingCriticalSection);

#else
		// While this method does not cause any updates the one below it does.
		EnterCriticalSection(&g_ThreadingCriticalSection);
		std::map<FunctionID, FunctionInfo>::const_iterator it = g_FunctionSet->find(funcId);
		LeaveCriticalSection(&g_ThreadingCriticalSection);
		if (it == g_FunctionSet->end())
		{

			// funcInfo declared in this block so they are not created if the function is found

			FunctionInfo funcInfo;

			g_MetadataHelpers->GetFunctionInformation(funcId, &funcInfo);
			// These iterator operations should not cause a lock since it's only a read
			// and the find method does not alter the structure.

			// The function mapping happens at the during the class instantiation
			std::unordered_set<std::wstring>::const_iterator findClass =
				g_ClassNameSet->find(funcInfo.ClassName());
			std::unordered_set<std::wstring>::const_iterator findName =
				g_FunctionNameSet->find(funcInfo.FunctionName());
			

			if ((findName != g_FunctionNameSet->end()) | (findClass != g_ClassNameSet->end()))
			{
				// Causes and update to this map and can be called from multiple threads.
				EnterCriticalSection(&g_ThreadingCriticalSection);
				g_FunctionSet->insert(std::pair<FunctionID, FunctionInfo>(funcId, funcInfo));
				
				LeaveCriticalSection(&g_ThreadingCriticalSection);
				*pbHookFunction = TRUE;
			}
			else {
				*pbHookFunction = FALSE;
			}

		}
#endif // ALLMETHODS
		return (UINT_PTR)funcId;

	}

	// Implementation of Mapper2. Just calls Mapper1
	UINT_PTR __stdcall Mapper2(FunctionID funcId, void * clientData, BOOL *pbHookFunction)
	{
		return Mapper1(funcId, pbHookFunction);
	}

	// Enter hook function for creating shadow stacks
	EXTERN_C void FunctionEnter2_CPP_Helper(FunctionID funcId, UINT_PTR clientData,
		COR_PRF_FRAME_INFO func, COR_PRF_FUNCTION_ARGUMENT_INFO *argumentInfo)
	{

		/*
		MSDN Article that describes the ELT methods and what COR flags need to be set.
		http://msdn.microsoft.com/en-us/magazine/cc300553.aspx
		*/

		TimerItem ti(ThreadStackReason::ENTER);
		// As we could be mapping a function while trying to find this guy it's possible
		// we would have a race condition.
		// However we will more than likely stop using this behavior as testing shows we don't need it
#pragma message(__TODO__"Do more testing around this 'feature' to see if we need it.")
		EnterCriticalSection(&g_ThreadingCriticalSection);
		std::map<FunctionID, FunctionInfo>::const_iterator it = g_FunctionSet->find(funcId);
		LeaveCriticalSection(&g_ThreadingCriticalSection);

		if (it != g_FunctionSet->end())
		{
			const FunctionInfo *fi = &it->second;
			ThreadID threadId;

			
			g_MetadataHelpers->GetCurrentThread(&threadId);
#pragma message(__TODO__"Add and or change the critical section to use something more specific.")
			EnterCriticalSection(&g_ThreadingCriticalSection);
			std::map<ThreadID, std::queue<StackItemBase*>>::iterator itStack = g_ThreadStackMap->find(threadId);
			LeaveCriticalSection(&g_ThreadingCriticalSection);

			if (itStack != g_ThreadStackMap->end())
			{
				
				FunctionStackItem tsi(funcId, ThreadStackReason::ENTER, *argumentInfo);
				tsi.Depth(g_ThreadStackDepth->at(threadId));
				tsi.SequenceNumber(g_ThreadStackSequence->at(threadId));
				ti.AddThreadStackItem(&tsi);
				// We *shouldn't* need to lock this as only one thread can act on this list.
				itStack->second.push(new FunctionStackItem(tsi));
				if (itStack->second.size() > 0)
				{
					// use the MS way of incrementing a mapped value
					InterlockedIncrement(&g_ThreadStackDepth->at(threadId));
					InterlockedIncrement(&g_ThreadStackSequence->at(threadId));
					//g_ThreadStackDepth->at(threadId)++;
				}
			}

			// TODO extract argument 
		}
		
		//TODO Implement function callbacks
	}

	// Leave hook function for creating shadow stacks
	EXTERN_C void FunctionLeave2_CPP_Helper(FunctionID funcId, UINT_PTR clientData,
		COR_PRF_FRAME_INFO func, COR_PRF_FUNCTION_ARGUMENT_RANGE *argumentRange)
	{
		TimerItem ti(ThreadStackReason::EXIT);
		EnterCriticalSection(&g_ThreadingCriticalSection);
		std::map<FunctionID, FunctionInfo>::const_iterator it = g_FunctionSet->find(funcId);
		LeaveCriticalSection(&g_ThreadingCriticalSection);
		if (it != g_FunctionSet->end())
		{

			ThreadID threadId;

			g_MetadataHelpers->GetCurrentThread(&threadId);
#pragma message(__TODO__"Add and or change the critical section to use something more specific.")
			EnterCriticalSection(&g_ThreadingCriticalSection);
			std::map<ThreadID, std::queue<StackItemBase*>>::iterator itStack = g_ThreadStackMap->find(threadId);
			LeaveCriticalSection(&g_ThreadingCriticalSection);
			if (itStack != g_ThreadStackMap->end())
			{
				if (itStack->second.size() > 0)
				{
					if (g_ThreadStackDepth->at(threadId) > 0)
					{
						InterlockedDecrement(&g_ThreadStackDepth->at(threadId));
					}

				}
				FunctionStackItem* tsi = static_cast<FunctionStackItem*>(itStack->second.back());
				tsi->ReturnValue(*argumentRange);
				ti.AddThreadStackItem(tsi);
			}

			// TODO extract argument 
		}
		//TODO Implement function callbacks
	}

	// Tail hook function for creating shadow stacks
	EXTERN_C void FunctionTail2_CPP_Helper(FunctionID funcId, UINT_PTR clientData,
		COR_PRF_FRAME_INFO func)
	{
		TimerItem ti(ThreadStackReason::TAIL);
		EnterCriticalSection(&g_ThreadingCriticalSection);
		std::map<FunctionID, FunctionInfo>::const_iterator it = g_FunctionSet->find(funcId);
		LeaveCriticalSection(&g_ThreadingCriticalSection);
		if (it != g_FunctionSet->end())
		{
			ThreadID threadId;

			g_MetadataHelpers->GetCurrentThread(&threadId);
#pragma message(__TODO__"Add and or change the critical section to use something more specific.")
			EnterCriticalSection(&g_ThreadingCriticalSection);
			std::map<ThreadID, std::queue<StackItemBase*>>::iterator itStack = g_ThreadStackMap->find(threadId);
			LeaveCriticalSection(&g_ThreadingCriticalSection);
			int depth = 0;
			if (itStack != g_ThreadStackMap->end())
			{
				if (itStack->second.size() > 0)
				{

					InterlockedDecrement(&g_ThreadStackDepth->at(threadId));
				}
				StackItemBase* tsi = itStack->second.back();
				//tsi->ItemStackReturnValue(*argumentRange);
				ti.AddThreadStackItem(tsi);
			}
		}
		// TODO extract argument 
	}
	//TODO Implement function callbacks

}

