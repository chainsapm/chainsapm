// profilermain.cpp : Implementation of Cprofilermain
#pragma once
#include "stdafx.h"
#include "ContainerClass.h"
#include "profilermain.h"
#include "srw_helper.h"
#include "critsec_helper.h"
#include "webengine4helper.h"
#include "networkclient.h"
#include "Commands.h"
#include <future>



//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Definition of static members of the Cprofilermain class
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//MetadataHelpers *  Cprofilermain::g_MetadataHelpers;
//
//
//std::map<FunctionID, FunctionInfo> * Cprofilermain::g_FunctionSet = new std::map<FunctionID, FunctionInfo>();
//std::map<ThreadID, std::queue<StackItemBase*>> * Cprofilermain::g_ThreadStackMap = new std::map<ThreadID, std::queue<StackItemBase*>>();
//std::map<ThreadID, volatile unsigned int> * Cprofilermain::g_ThreadStackDepth = new std::map<ThreadID, volatile unsigned int>();
//std::map<ThreadID, volatile unsigned int> * Cprofilermain::g_ThreadStackSequence = new std::map<ThreadID, volatile unsigned int>();
//std::map<ThreadID, volatile unsigned int> * Cprofilermain::g_ThreadFunctionCount = new std::map<ThreadID, volatile unsigned int>();
//
//
//std::unordered_set<std::wstring> * Cprofilermain::g_FunctionNameSet = new std::unordered_set<std::wstring>();
//std::unordered_set<std::wstring> * Cprofilermain::g_ClassNameSet = new std::unordered_set<std::wstring>();
//
//std::map<ThreadID, UINT_PTR> * Cprofilermain::g_ThreadSpawnMap = NULL;
//
//CRITICAL_SECTION Cprofilermain::g_ThreadingCriticalSection;
//CRITICAL_SECTION Cprofilermain::g_ThreadStackSequenceCriticalSection;
//CRITICAL_SECTION Cprofilermain::g_ThreadStackDepthCriticalSection;
//CRITICAL_SECTION Cprofilermain::g_FunctionSetCriticalSection;

std::map<UINT_PTR, Cprofilermain*> * Cprofilermain::g_StaticContainerClass = new std::map<UINT_PTR, Cprofilermain*>();
CRITICAL_SECTION Cprofilermain::g_StaticContainerClassCritSec;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Remove the separator (,) and digit grouping on numbers like 1,000,000
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct no_separator : std::numpunct < char > {
protected:
	virtual std::string do_grouping() const
	{
		return "\000";
	} // groups of 0 (disable)
};



//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Global methods for specific profiler callbacks. Namely the Mapper and Enter/Leave/Tail
//////////////////////////////////////////////////////////////////////////////////////////////////////////////


// Enter hook function for creating shadow stacks
void FunctionEnter2_CPP_STUB(FunctionID funcId, UINT_PTR clientData,
	COR_PRF_FRAME_INFO func, COR_PRF_FUNCTION_ARGUMENT_INFO *argumentInfo)
{
	Cprofilermain * pContainerClass = (Cprofilermain*)clientData;
	if (funcId == clientData)
	{
		pContainerClass = Cprofilermain::g_StaticContainerClass->at(0x0);
	}

	pContainerClass->FunctionEnterHook2(funcId, clientData, func, argumentInfo);
	//pContainerClass->FunctionEnterHook2(funcId, clientData, func, argumentInfo);
}

// Leave hook function for creating shadow stacks
void FunctionLeave2_CPP_STUB(FunctionID funcId, UINT_PTR clientData,
	COR_PRF_FRAME_INFO func, COR_PRF_FUNCTION_ARGUMENT_RANGE *argumentRange)
{
	Cprofilermain * pContainerClass = (Cprofilermain*)clientData;
	if (funcId == clientData)
	{
		pContainerClass = Cprofilermain::g_StaticContainerClass->at(0x0);
	}

	pContainerClass->FunctionLeaveHook2(funcId, clientData, func, argumentRange);
}

// Tail hook function for creating shadow stacks
void FunctionTail2_CPP_STUB(FunctionID funcId, UINT_PTR clientData,
	COR_PRF_FRAME_INFO func)
{

	Cprofilermain * pContainerClass = (Cprofilermain*)clientData;
	if (funcId == clientData)
	{
		pContainerClass = Cprofilermain::g_StaticContainerClass->at(0x0);
	}

	pContainerClass->FunctionTailHook2(funcId, clientData, func);
}




#if X64
#else

// @@TODO: Change the declaration to match the function you're implementing
void __declspec(naked) __stdcall FunctionLeave2_x86(FunctionID id, UINT_PTR clientData, COR_PRF_FRAME_INFO frame, COR_PRF_FUNCTION_ARGUMENT_RANGE* retvalRange)
{
	__asm
	{
#ifdef DEBUG
		// Set up EBP Frame (easy debugging)
		// Turned off in release mode to save cycles
		push ebp
			mov ebp, esp
#endif

			// Make space for locals.
			sub esp, __LOCAL_SIZE

			// Save all registers.
			//
			// Technically you need only save what you use, but if you're working in C
			// rather than assembly that's hard to predict. Saving them all is safer.
			// If you really need to save the minimal set, code the whole darn thing in 
			// assembly so you can control the register usage.
			//
			// Pushing ESP again is redundant (and in DEBUG pushing EBP is too), but using
			// pushad is cleaner and saves some clock time over pushing individual registers.
			// You can push the individual registers if you want; doing so will
			// save a couple of DWORDs of stack space, perhaps at the expense of
			// a clock cycle.
			pushad

			// Check if there's anything on the FP stack.
			//
			// Again technically you need only save what you use. You might think that
			// FP regs are not commonly used in the kind of code you'd write in these,
			// but there are common cases that might interfere. For example, in the 8.0 MS CRT, 
			// memcpy clears the FP stack.
			//
			// @@TODO: In CLR versions 1.x and 2.0, everything from here to NoSaveFPReg
			// is only strictly necessary for FunctionLeave and FunctionLeave2.
			// Of course that may change in future releases, so use this code for all of your
			// enter/leave function hooks if you want to avoid breaking.
			fstsw   ax
			and     ax, 3800h		// Check the top-of-fp-stack bits
			cmp     ax, 0			// If non-zero, we have something to save
			jnz     SaveFPReg
			push    0				// otherwise, mark that there is no float value
			jmp     NoSaveFPReg
		SaveFPReg :
		sub     esp, 8			// Make room for the FP value
			fstp    qword ptr[esp] // Copy the FP value to the buffer as a double
			push    1				// mark that a float value is present
		NoSaveFPReg :
	}

	::FunctionLeave2_CPP_STUB(id, clientData, frame, retvalRange);

	__asm
	{
		// Now see if we have to restore any floating point registers
		// @@TODO: In CLR versions 1.x and 2.0, everything from here to 
		// RestoreFPRegsDone is only strictly necessary for FunctionLeave and FunctionLeave2
		// Of course that may change in future releases, so use this code for all of your
		// enter/leave function hooks if you want to avoid breaking.
		cmp[esp], 0			// Check the flag
			jz      NoRestoreFPRegs		// If zero, no FP regs
		RestoreFPRegs :
		fld     qword ptr[esp + 4]	// Restore FP regs
			add    esp, 12				// Move ESP past the storage space
			jmp   RestoreFPRegsDone
		NoRestoreFPRegs :
		add     esp, 4				// Move ESP past the flag
		RestoreFPRegsDone :

						  // Restore other registers
						  popad

						  // Pop off locals
						  add esp, __LOCAL_SIZE

#ifdef DEBUG
						  // Restore EBP
						  // Turned off in release mode to save cycles
						  pop ebp
#endif

						  // stdcall: Callee cleans up args from stack on return
						  // @@TODO: Change this line to match the parameters to your function!
						  ret SIZE id + SIZE clientData + SIZE frame + SIZE retvalRange
	}
}

// @@TODO: Change the declaration to match the function you're implementing
void __declspec(naked) __stdcall FunctionEnter2_x86(FunctionID funcId, UINT_PTR clientData,
	COR_PRF_FRAME_INFO func, COR_PRF_FUNCTION_ARGUMENT_INFO *argumentInfo)
{
	__asm
	{
#ifdef DEBUG
		// Set up EBP Frame (easy debugging)
		// Turned off in release mode to save cycles
		push ebp
			mov ebp, esp
#endif

			// Make space for locals.
			sub esp, __LOCAL_SIZE

			// Save all registers.
			//
			// Technically you need only save what you use, but if you're working in C
			// rather than assembly that's hard to predict. Saving them all is safer.
			// If you really need to save the minimal set, code the whole darn thing in 
			// assembly so you can control the register usage.
			//
			// Pushing ESP again is redundant (and in DEBUG pushing EBP is too), but using
			// pushad is cleaner and saves some clock time over pushing individual registers.
			// You can push the individual registers if you want; doing so will
			// save a couple of DWORDs of stack space, perhaps at the expense of
			// a clock cycle.
			pushad

			// Check if there's anything on the FP stack.
			//
			// Again technically you need only save what you use. You might think that
			// FP regs are not commonly used in the kind of code you'd write in these,
			// but there are common cases that might interfere. For example, in the 8.0 MS CRT, 
			// memcpy clears the FP stack.
			//
			// @@TODO: In CLR versions 1.x and 2.0, everything from here to NoSaveFPReg
			// is only strictly necessary for FunctionLeave and FunctionLeave2.
			// Of course that may change in future releases, so use this code for all of your
			// enter/leave function hooks if you want to avoid breaking.
			fstsw   ax
			and     ax, 3800h		// Check the top-of-fp-stack bits
			cmp     ax, 0			// If non-zero, we have something to save
			jnz     SaveFPReg
			push    0				// otherwise, mark that there is no float value
			jmp     NoSaveFPReg
		SaveFPReg :
		sub     esp, 8			// Make room for the FP value
			fstp    qword ptr[esp] // Copy the FP value to the buffer as a double
			push    1				// mark that a float value is present
		NoSaveFPReg :
	}

	FunctionEnter2_CPP_STUB(funcId, clientData, func, argumentInfo);

	__asm
	{
		// Now see if we have to restore any floating point registers
		// @@TODO: In CLR versions 1.x and 2.0, everything from here to 
		// RestoreFPRegsDone is only strictly necessary for FunctionLeave and FunctionLeave2
		// Of course that may change in future releases, so use this code for all of your
		// enter/leave function hooks if you want to avoid breaking.
		cmp[esp], 0			// Check the flag
			jz      NoRestoreFPRegs		// If zero, no FP regs
		RestoreFPRegs :
		fld     qword ptr[esp + 4]	// Restore FP regs
			add    esp, 12				// Move ESP past the storage space
			jmp   RestoreFPRegsDone
		NoRestoreFPRegs :
		add     esp, 4				// Move ESP past the flag
		RestoreFPRegsDone :

						  // Restore other registers
						  popad

						  // Pop off locals
						  add esp, __LOCAL_SIZE

#ifdef DEBUG
						  // Restore EBP
						  // Turned off in release mode to save cycles
						  pop ebp
#endif

						  // stdcall: Callee cleans up args from stack on return
						  // @@TODO: Change this line to match the parameters to your function!
						  ret SIZE funcId + SIZE clientData + SIZE func + SIZE argumentInfo
	}
}


// @@TODO: Change the declaration to match the function you're implementing
void __declspec(naked) __stdcall FunctionTail2_x86(FunctionID funcId, UINT_PTR clientData,
	COR_PRF_FRAME_INFO func)
{
	__asm
	{
#ifdef DEBUG
		// Set up EBP Frame (easy debugging)
		// Turned off in release mode to save cycles
		push ebp
			mov ebp, esp
#endif

			// Make space for locals.
			sub esp, __LOCAL_SIZE

			// Save all registers.
			//
			// Technically you need only save what you use, but if you're working in C
			// rather than assembly that's hard to predict. Saving them all is safer.
			// If you really need to save the minimal set, code the whole darn thing in 
			// assembly so you can control the register usage.
			//
			// Pushing ESP again is redundant (and in DEBUG pushing EBP is too), but using
			// pushad is cleaner and saves some clock time over pushing individual registers.
			// You can push the individual registers if you want; doing so will
			// save a couple of DWORDs of stack space, perhaps at the expense of
			// a clock cycle.
			pushad

			// Check if there's anything on the FP stack.
			//
			// Again technically you need only save what you use. You might think that
			// FP regs are not commonly used in the kind of code you'd write in these,
			// but there are common cases that might interfere. For example, in the 8.0 MS CRT, 
			// memcpy clears the FP stack.
			//
			// @@TODO: In CLR versions 1.x and 2.0, everything from here to NoSaveFPReg
			// is only strictly necessary for FunctionLeave and FunctionLeave2.
			// Of course that may change in future releases, so use this code for all of your
			// enter/leave function hooks if you want to avoid breaking.
			fstsw   ax
			and     ax, 3800h		// Check the top-of-fp-stack bits
			cmp     ax, 0			// If non-zero, we have something to save
			jnz     SaveFPReg
			push    0				// otherwise, mark that there is no float value
			jmp     NoSaveFPReg
		SaveFPReg :
		sub     esp, 8			// Make room for the FP value
			fstp    qword ptr[esp] // Copy the FP value to the buffer as a double
			push    1				// mark that a float value is present
		NoSaveFPReg :
	}

	FunctionTail2_CPP_STUB(funcId, clientData, func);

	__asm
	{
		// Now see if we have to restore any floating point registers
		// @@TODO: In CLR versions 1.x and 2.0, everything from here to 
		// RestoreFPRegsDone is only strictly necessary for FunctionLeave and FunctionLeave2
		// Of course that may change in future releases, so use this code for all of your
		// enter/leave function hooks if you want to avoid breaking.
		cmp[esp], 0			// Check the flag
			jz      NoRestoreFPRegs		// If zero, no FP regs
		RestoreFPRegs :
		fld     qword ptr[esp + 4]	// Restore FP regs
			add    esp, 12				// Move ESP past the storage space
			jmp   RestoreFPRegsDone
		NoRestoreFPRegs :
		add     esp, 4				// Move ESP past the flag
		RestoreFPRegsDone :

						  // Restore other registers
						  popad

						  // Pop off locals
						  add esp, __LOCAL_SIZE

#ifdef DEBUG
						  // Restore EBP
						  // Turned off in release mode to save cycles
						  pop ebp
#endif

						  // stdcall: Callee cleans up args from stack on return
						  // @@TODO: Change this line to match the parameters to your function!
						  ret SIZE funcId + SIZE clientData + SIZE func
	}
}
#endif


DWORD WINAPI MyThreadFunction(LPVOID lpParam)
{
	INT counter = 1;
	Cprofilermain * pCprof = (Cprofilermain*)lpParam;
	while (true)
	{
		pCprof->DequeItems();
		Sleep(500);
	}
	return S_OK;
}

STDMETHODIMP Cprofilermain::DequeItems()
{

	while (!this->m_Container->g_BigStack->empty())
	{
		critsec_helper cshT(&this->m_Container->g_ThreadingCriticalSection);
		this->m_Container->g_BigStack->front();
		this->m_Container->g_BigStack->pop();
	}
	return S_OK;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Constructor and Destructor
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

Cprofilermain::Cprofilermain()
{
	if (!TryEnterCriticalSection(&Cprofilermain::g_StaticContainerClassCritSec))
	{
		InitializeCriticalSection(&Cprofilermain::g_StaticContainerClassCritSec);
	}
	else {
		LeaveCriticalSection(&Cprofilermain::g_StaticContainerClassCritSec);
	}

	// TODO: Generate unique and repeatable identifier for this CLR agent.
	// We should be able to reliably create a hash or some idetifier that will distinguish the application.
	// For example, the w3wp process should include the application pool name.

	this->SetProcessName();
	// TODO: Make the act of selectively attaching to a process more dynamic.
	// TODO: Consider using an ini file.
	if ((this->m_ProcessName.compare(L"w3wp.exe") == 0)
		|| (this->m_ProcessName.compare(L"HelloWorldTestHarness.exe") == 0)
		|| (this->m_ProcessName.compare(L"iisexpress.exe") == 0))
	{
		// No reason to execute this code if the process is not what we're looking for.
		this->m_Container = new ContainerClass();
		this->m_ProcessId = GetCurrentProcessId();
		this->AddCommonFunctions();
		auto m_NetworkClient = new NetworkClient(this, TEXT("localhost"), TEXT("8080"));
		// CRITICAL 1 Research this critical section in the profiler main constructor.
		InitializeCriticalSection(&this->m_Container->g_ThreadingCriticalSection);
	}

	DWORD tID = 0;
	// CRITICAL 9 Remove this log writer thread.
	HANDLE tHandle = CreateThread(
		NULL,                   // default security attributes
		0,                      // use default stack size  
		MyThreadFunction,       // thread function name
		this,					// argument to thread function 
		0,                      // use default creation flags 
		&tID);
}

Cprofilermain::~Cprofilermain()
{
	WriteLogFile(0);
	/*delete g_FunctionNameSet;
	delete g_FunctionSet;
	delete g_ThreadStackMap;
	delete g_MetadataHelpers;*/
	if (m_pICorProfilerInfo != NULL)
	{
		m_pICorProfilerInfo.reset();
	}
	if (m_pICorProfilerInfo2 != NULL)
	{
		m_pICorProfilerInfo2.reset();
	}
	if (m_pICorProfilerInfo3 != NULL)
	{
		m_pICorProfilerInfo3.reset();
	}
	if (m_pICorProfilerInfo4 != NULL)
	{
		m_pICorProfilerInfo4.reset();
	}
	this->Shutdown();
	// CRITICAL 1 Research this critical section in the profiler main destructor.
	DeleteCriticalSection(&this->m_Container->g_ThreadingCriticalSection);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// These are helper methods and have nothing to do with the core profiling.
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Cprofilermain::AddCommonFunctions()
{
	// TODO: Create a more dynamic way to add classes and functions to the list of things to be mapped.
	/*g_ClassNameSet->insert(TEXT("System.Threading.Thread"));
   g_ClassNameSet->insert(TEXT("System.Threading.ThreadStart"));
   g_ClassNameSet->insert(TEXT("System.Threading.ThreadHelper"));*/

	//this->m_Container->g_FunctionNameSet->insert(TEXT("ProcessRequest"));

	ItemMapping *newMapping2 = new ItemMapping();
	ItemMapping *newMapping3 = new ItemMapping();
	ItemMapping *newMapping4 = new ItemMapping();
	ItemMapping *newMapping5 = new ItemMapping();
	ItemMapping *newMapping6 = new ItemMapping();
	ItemMapping *newMapping7 = new ItemMapping();
	ItemMapping *newMapping8 = new ItemMapping();



	newMapping2->ItemName = TEXT("ProcessRequest");
	newMapping2->compare = STRINGCOMPARE::CONTAINS;
	this->m_Container->g_FunctionNameSet->insert(newMapping2);

	newMapping3->ItemName = TEXT("Thread");
	newMapping3->compare = STRINGCOMPARE::CONTAINS;
	this->m_Container->g_FunctionNameSet->insert(newMapping3);

	newMapping4->ItemName = TEXT("Connect");
	newMapping4->compare = STRINGCOMPARE::CONTAINS;
	this->m_Container->g_FunctionNameSet->insert(newMapping4);

	newMapping5->ItemName = TEXT("WriteStream");
	newMapping5->compare = STRINGCOMPARE::CONTAINS;
	this->m_Container->g_FunctionNameSet->insert(newMapping5);

	newMapping6->ItemName = TEXT("Headers");
	newMapping6->compare = STRINGCOMPARE::CONTAINS;
	this->m_Container->g_FunctionNameSet->insert(newMapping6);

	newMapping7->ItemName = TEXT("AddNumbers");
	newMapping7->compare = STRINGCOMPARE::CONTAINS;
	this->m_Container->g_FunctionNameSet->insert(newMapping7);

	newMapping8->ItemName = TEXT("WriteLine");
	newMapping8->compare = STRINGCOMPARE::CONTAINS;
	this->m_Container->g_FunctionNameSet->insert(newMapping8);



	/*	this->m_Container->g_FunctionNameSet->insert(TEXT("ThreadStart"));
		this->m_Container->g_FunctionNameSet->insert(TEXT("Start"))*/;

}

void Cprofilermain::WriteLogFile(int fileNum)
{
	//// TODO: Adapt some of this code into the server module to recreate a thread stack.
	////std::string fileName = ;
	//std::wstring fileName(str(boost::wformat(L"C:\\logfiles\\%d_%d_%s.log") % fileNum % m_ProcessId % m_ProcessName));
	//std::wofstream outFile(fileName);
	//if (outFile)
	//{
	//	//std::map<ThreadID, std::queue<StackItemBase*>>::iterator it;
	//	std::locale loc("");
	//	UINT_PTR highPart;
	//	UINT_PTR lowPart;
	//	UINT highPartParam;
	//	UINT lowPartParam;
	//	UINT highPartReturn;
	//	UINT lowPartReturn;
	//	outFile.imbue(std::locale(loc, new no_separator()));
	//	std::map<FunctionID, FunctionInfo*>::iterator itFunc;
	//	std::wstring spaces;
	//	std::wstring separator(80, '=');
	//	outFile << separator << std::endl;
	//	outFile << this->m_ProcessName << L" " << boost::wformat(L"%u") % this->m_ProcessId << std::endl;
	//	outFile << separator << std::endl;
	//	outFile << std::endl;
	//	for (auto it = this->m_Container->g_ThreadStackMap->begin();
	//		it != this->m_Container->g_ThreadStackMap->end();
	//		it++)
	//	{
	//		int depth = 0;
	//		int previousDepth = 0;
	//		highPart = (0xFFFFFFFF00000000 & it->first) >> 32;
	//		lowPart = 0x00000000FFFFFFFF & it->first;


	//		EnterCriticalSection(&this->m_Container->g_ThreadingCriticalSection);
	//		std::deque<StackItemBase*>::const_iterator constIt = it->second.cbegin();
	//		LeaveCriticalSection(&this->m_Container->g_ThreadingCriticalSection);

	//		ThreadStackItem* threadStackItem = NULL;
	//		try
	//		{
	//			threadStackItem = dynamic_cast<ThreadStackItem*>(*constIt);
	//		}
	//		catch (std::bad_cast* e)
	//		{

	//			outFile << e->what();
	//		}
	//		outFile << separator << std::endl;
	//		if (threadStackItem != NULL)
	//		{
	//			outFile << "Thread: " << threadStackItem->ThreadName() << boost::wformat(TEXT(" (0x%08x`%08x)")) % highPart % lowPart << std::endl;
	//		}
	//		else {
	//			outFile << "Thread: " << boost::wformat(TEXT("0x%08x`%08x")) % highPart % lowPart << std::endl;
	//		}
	//		outFile << separator << std::endl;
	//		outFile << std::endl;
	//		ULONGLONG totalTime = 0;
	//		ULONGLONG profilerTime = 0;

	//		while (constIt != it->second.cend())
	//		{

	//			FunctionStackItem* testItemConverted = NULL;
	//			try
	//			{
	//				testItemConverted = dynamic_cast<FunctionStackItem*>(*constIt);
	//			}
	//			catch (std::bad_cast* e)
	//			{
	//				outFile << e->what();
	//			}

	//			if (testItemConverted != NULL)
	//			{
	//				itFunc = this->m_Container->g_FunctionSet->find(testItemConverted->FunctionId());

	//				if (&itFunc != NULL && itFunc != this->m_Container->g_FunctionSet->end())
	//				{
	//					if ((*constIt)->Depth() >= 0)
	//					{
	//						depth = (*constIt)->Depth();
	//					}
	//					spaces.swap(std::wstring(depth * 2, ' '));
	//					outFile << spaces << itFunc->second->SignatureString();
	//					if ((*constIt)->LastReason() == TAIL)
	//					{
	//						outFile << _T(" !!TAIL CALL!! ");
	//					}
	//					outFile << std::endl;
	//					if (testItemConverted->ParameterCount() != 0)
	//					{
	//						outFile << spaces;
	//						int i = 0;
	//						int paramNumber = 1;
	//						for (auto &param : *testItemConverted->ItemStackParameters())
	//						{
	//							highPartParam = (0xFFFFFFFF00000000 & param) >> 32;
	//							lowPartParam = 0x00000000FFFFFFFF & param;
	//							if (itFunc->second->IsStatic() == FALSE && i == 0)
	//							{
	//								outFile << "Class Pointer: ";
	//								paramNumber--;
	//							}
	//							else {
	//								outFile << "Parameter " << paramNumber << ": ";

	//							}
	//							outFile << boost::wformat(TEXT("0x%08x`%08x")) % highPartParam % lowPartParam << " ";
	//							++paramNumber;
	//						}
	//						outFile << std::endl;
	//					}
	//					highPartReturn = (0xFFFFFFFF00000000 & testItemConverted->ReturnValue()) >> 32;
	//					lowPartReturn = 0x00000000FFFFFFFF & testItemConverted->ReturnValue();
	//					outFile << spaces << "Return: " << boost::wformat(TEXT("0x%08x`%08x")) % highPartReturn % lowPartReturn << std::endl;
	//					outFile << spaces << boost::wformat(TEXT("Total time: %uus\tProfiling Time: %uus")) % (*constIt)->ItemRunTime() % (*constIt)->ProfilingOverhead() << std::endl;
	//					totalTime += (*constIt)->ItemRunTime();
	//					profilerTime += (*constIt)->ProfilingOverhead();
	//				}
	//			}

	//			constIt++;
	//		};
	//		outFile << separator << std::endl;
	//		outFile << boost::wformat(TEXT("Total time: %uus\tProfiling Time: %uus")) % totalTime % profilerTime << std::endl;
	//		outFile << separator << std::endl;

	//	}
	//}
}

void Cprofilermain::SetProcessName()
{
	WCHAR imageName[MAX_PATH];
	GetModuleFileName(NULL, imageName, MAX_PATH);
	std::wstringstream stringStream(imageName);
	std::wstring lastItem;
	for (std::wstring item; std::getline(stringStream, item, L'\\');)
	{
		lastItem.assign(item);
	}
	this->m_ProcessName.assign(lastItem);
}

STDMETHODIMP Cprofilermain::SetMask()
{
	/*
	COR_PRF_MONITOR_NONE = 0,
	COR_PRF_MONITOR_FUNCTION_UNLOADS = 0x1,
	COR_PRF_MONITOR_CLASS_LOADS = 0x2,
	COR_PRF_MONITOR_MODULE_LOADS = 0x4,
	COR_PRF_MONITOR_ASSEMBLY_LOADS = 0x8,
	COR_PRF_MONITOR_APPDOMAIN_LOADS = 0x10,
	COR_PRF_MONITOR_JIT_COMPILATION = 0x20,
	COR_PRF_MONITOR_EXCEPTIONS = 0x40,
	COR_PRF_MONITOR_GC = 0x80,
	COR_PRF_MONITOR_OBJECT_ALLOCATED = 0x100,
	COR_PRF_MONITOR_THREADS = 0x200,
	COR_PRF_MONITOR_REMOTING = 0x400,
	COR_PRF_MONITOR_CODE_TRANSITIONS = 0x800,
	COR_PRF_MONITOR_ENTERLEAVE = 0x1000,
	COR_PRF_MONITOR_CCW = 0x2000,
	COR_PRF_MONITOR_REMOTING_COOKIE = (0x4000 | COR_PRF_MONITOR_REMOTING),
	COR_PRF_MONITOR_REMOTING_ASYNC = (0x8000 | COR_PRF_MONITOR_REMOTING),
	COR_PRF_MONITOR_SUSPENDS = 0x10000,
	COR_PRF_MONITOR_CACHE_SEARCHES = 0x20000,
	COR_PRF_ENABLE_REJIT = 0x40000,
	COR_PRF_ENABLE_INPROC_DEBUGGING = 0x80000,
	COR_PRF_ENABLE_JIT_MAPS = 0x100000,
	COR_PRF_DISABLE_INLINING = 0x200000,
	COR_PRF_DISABLE_OPTIMIZATIONS = 0x400000,
	COR_PRF_ENABLE_OBJECT_ALLOCATED = 0x800000,
	COR_PRF_MONITOR_CLR_EXCEPTIONS = 0x1000000,
	COR_PRF_MONITOR_ALL = 0x107ffff,
	COR_PRF_ENABLE_FUNCTION_ARGS = 0x2000000,
	COR_PRF_ENABLE_FUNCTION_RETVAL = 0x4000000,
	COR_PRF_ENABLE_FRAME_INFO = 0x8000000,
	COR_PRF_ENABLE_STACK_SNAPSHOT = 0x10000000,
	COR_PRF_USE_PROFILE_IMAGES = 0x20000000,
	COR_PRF_DISABLE_TRANSPARENCY_CHECKS_UNDER_FULL_TRUST = 0x40000000,
	COR_PRF_DISABLE_ALL_NGEN_IMAGES = 0x80000000,
	COR_PRF_ALL = 0x8fffffff,
	COR_PRF_REQUIRE_PROFILE_IMAGE = ((COR_PRF_USE_PROFILE_IMAGES | COR_PRF_MONITOR_CODE_TRANSITIONS) | COR_PRF_MONITOR_ENTERLEAVE),
	COR_PRF_ALLOWABLE_AFTER_ATTACH = ((((((((COR_PRF_MONITOR_THREADS | COR_PRF_MONITOR_MODULE_LOADS) | COR_PRF_MONITOR_ASSEMBLY_LOADS) | COR_PRF_MONITOR_APPDOMAIN_LOADS) | COR_PRF_ENABLE_STACK_SNAPSHOT) | COR_PRF_MONITOR_GC) | COR_PRF_MONITOR_SUSPENDS) | COR_PRF_MONITOR_CLASS_LOADS) | COR_PRF_MONITOR_JIT_COMPILATION),
	COR_PRF_MONITOR_IMMUTABLE = (((((((((((((((COR_PRF_MONITOR_CODE_TRANSITIONS | COR_PRF_MONITOR_REMOTING) | COR_PRF_MONITOR_REMOTING_COOKIE) | COR_PRF_MONITOR_REMOTING_ASYNC) | COR_PRF_ENABLE_REJIT) | COR_PRF_ENABLE_INPROC_DEBUGGING) | COR_PRF_ENABLE_JIT_MAPS) | COR_PRF_DISABLE_OPTIMIZATIONS) | COR_PRF_DISABLE_INLINING) | COR_PRF_ENABLE_OBJECT_ALLOCATED) | COR_PRF_ENABLE_FUNCTION_ARGS) | COR_PRF_ENABLE_FUNCTION_RETVAL) | COR_PRF_ENABLE_FRAME_INFO) | COR_PRF_USE_PROFILE_IMAGES) | COR_PRF_DISABLE_TRANSPARENCY_CHECKS_UNDER_FULL_TRUST) | COR_PRF_DISABLE_ALL_NGEN_IMAGES)
	*/
	// set the event mask 

	// TODO: Add a more dynamic way to set the event mask.
	// TODO: Make note of the events that can be turned off an on, on the fly.

	DWORD eventMask = (DWORD)(
		COR_PRF_MONITOR_APPDOMAIN_LOADS
		//| COR_PRF_MONITOR_ENTERLEAVE
		//| COR_PRF_ENABLE_FRAME_INFO
		//| COR_PRF_ENABLE_FUNCTION_ARGS
		//| COR_PRF_ENABLE_FUNCTION_RETVAL
		| COR_PRF_MONITOR_THREADS
		| COR_PRF_MONITOR_GC
		| COR_PRF_MONITOR_SUSPENDS
		| COR_PRF_MONITOR_EXCEPTIONS
		| COR_PRF_MONITOR_CLR_EXCEPTIONS
		| COR_PRF_MONITOR_MODULE_LOADS
		| COR_PRF_MONITOR_ASSEMBLY_LOADS
		| COR_PRF_MONITOR_APPDOMAIN_LOADS
		| COR_PRF_ENABLE_REJIT
		| COR_PRF_DISABLE_ALL_NGEN_IMAGES
		| COR_PRF_MONITOR_JIT_COMPILATION);
	switch (this->m_HighestProfileInfo)
	{
	case 1:
		return m_pICorProfilerInfo->SetEventMask(eventMask);
	case 2:
		return m_pICorProfilerInfo2->SetEventMask(eventMask);
	case 3:
		return m_pICorProfilerInfo3->SetEventMask(eventMask);
	case 4:
		return m_pICorProfilerInfo4->SetEventMask(eventMask);
	default:
		return 0;
	}

}

STDMETHODIMP Cprofilermain::GetFullMethodName(FunctionID functionID, std::wstring &methodName)
{
	// CRITICAL 8 Move this to the metadata helpers class.
	IMetaDataImport* pIMetaDataImport = 0;
	HRESULT hr = S_OK;
	mdToken funcToken = 0;
	WCHAR szFunction[NAME_BUFFER_SIZE];
	WCHAR szClass[NAME_BUFFER_SIZE];
	WCHAR wszMethod[NAME_BUFFER_SIZE];

	// get the token for the function which we will use to get its name
	switch (this->m_HighestProfileInfo)
	{
	case 1:
		hr = this->m_pICorProfilerInfo->GetTokenAndMetaDataFromFunction(functionID, IID_IMetaDataImport, (LPUNKNOWN *)&pIMetaDataImport, &funcToken);
		break;
	case 2:
		hr = this->m_pICorProfilerInfo2->GetTokenAndMetaDataFromFunction(functionID, IID_IMetaDataImport, (LPUNKNOWN *)&pIMetaDataImport, &funcToken);
		break;
	case 3:
		hr = this->m_pICorProfilerInfo3->GetTokenAndMetaDataFromFunction(functionID, IID_IMetaDataImport, (LPUNKNOWN *)&pIMetaDataImport, &funcToken);
		break;
	case 4:
		hr = this->m_pICorProfilerInfo4->GetTokenAndMetaDataFromFunction(functionID, IID_IMetaDataImport, (LPUNKNOWN *)&pIMetaDataImport, &funcToken);
		break;
	default:
		return E_FAIL;
	}
	//hr = this->m_pICorProfilerInfo2->GetTokenAndMetaDataFromFunction(functionID, IID_IMetaDataImport, (LPUNKNOWN *)&pIMetaDataImport, &funcToken);
	if (SUCCEEDED(hr))
	{
		mdTypeDef classTypeDef;
		ULONG cchFunction;
		ULONG cchClass;

		// retrieve the function properties based on the token
		hr = pIMetaDataImport->GetMethodProps(funcToken, &classTypeDef, szFunction, NAME_BUFFER_SIZE, &cchFunction, 0, 0, 0, 0, 0);
		if (SUCCEEDED(hr))
		{
			// get the function name
			hr = pIMetaDataImport->GetTypeDefProps(classTypeDef, szClass, NAME_BUFFER_SIZE, &cchClass, 0, 0);
			if (SUCCEEDED(hr))
			{
				// create the fully qualified name
				_snwprintf_s<NAME_BUFFER_SIZE>(wszMethod, NAME_BUFFER_SIZE, L"%s.%s", szClass, szFunction);
				methodName.assign(std::wstring(wszMethod));
			}
		}
		// release our reference to the metadata
		pIMetaDataImport->Release();
	}

	return hr;
}

STDMETHODIMP Cprofilermain::GetFuncArgs(FunctionID functionID, COR_PRF_FRAME_INFO frameinfo)
{
	// CRITICAL 8 Move this to the metadata helpers class.
	HRESULT hr = S_OK;
	mdToken funcToken = 0;
	ClassID classID;
	ModuleID modId;
	mdToken token;
	ULONG32 typeArgsMax = 1024;
	ULONG32 typeArgsOut = 0;
	ClassID typeArgs[1024];


	switch (this->m_HighestProfileInfo)
	{
	case 1:
		return E_FAIL;
	case 2:
		hr = this->m_pICorProfilerInfo2->GetFunctionInfo2(
			functionID,
			frameinfo,
			&classID,
			&modId,
			&token,
			typeArgsMax,
			&typeArgsOut,
			typeArgs);
		break;
	case 3:
		hr = this->m_pICorProfilerInfo3->GetFunctionInfo2(
			functionID,
			frameinfo,
			&classID,
			&modId,
			&token,
			typeArgsMax,
			&typeArgsOut,
			typeArgs);
		break;
	case 4:
		hr = this->m_pICorProfilerInfo4->GetFunctionInfo2(
			functionID,
			frameinfo,
			&classID,
			&modId,
			&token,
			typeArgsMax,
			&typeArgsOut,
			typeArgs);
		break;
	default:
		return E_FAIL;
	}
	// get the token for the function which we will use to get its name


	return hr;
}




//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Possibly we will eventually move all of these .NET profiling methods to base classes for clarity
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

STDMETHODIMP Cprofilermain::Initialize(IUnknown *pICorProfilerInfoUnk)
{
	// We're using this as a quick exit so the profiler doesn't actually load.
	if ((this->m_ProcessName.compare(L"w3wp.exe") == 0)
		|| (this->m_ProcessName.compare(L"HelloWorldTestHarness.exe") == 0)
		|| (this->m_ProcessName.compare(L"iisexpress.exe") == 0))
	{


		// get the ICorProfilerInfo interface

		HRESULT hr;
		this->m_HighestProfileInfo = 1; // If we don't fail, start off by assuming we're at the highest version we support

		hr = pICorProfilerInfoUnk->QueryInterface(IID_ICorProfilerInfo, (LPVOID*)&this->m_pICorProfilerInfo);
		if (FAILED(hr))
			return E_FAIL;

		// determine if this object implements ICorProfilerInfo2
		hr = pICorProfilerInfoUnk->QueryInterface(IID_ICorProfilerInfo2, (LPVOID*)&this->m_pICorProfilerInfo2);
		if (FAILED(hr))
		{
			// we still want to work if this call fails, might be an older .NET version
			this->m_pICorProfilerInfo2 = nullptr;

		}
		else {
			this->m_HighestProfileInfo = 2;
		}

		// determine if this object implements ICorProfilerInfo3
		hr = pICorProfilerInfoUnk->QueryInterface(IID_ICorProfilerInfo3, (LPVOID*)&this->m_pICorProfilerInfo3);
		if (FAILED(hr))
		{
			// we still want to work if this call fails, might be an older .NET version
			this->m_pICorProfilerInfo3 = nullptr;

		}
		else {
			this->m_HighestProfileInfo = 3;
		}

		// determine if this object implements ICorProfilerInfo4
		hr = pICorProfilerInfoUnk->QueryInterface(IID_ICorProfilerInfo4, (LPVOID*)&this->m_pICorProfilerInfo4);
		if (FAILED(hr))
		{
			// we still want to work if this call fails, might be an older .NET version
			this->m_pICorProfilerInfo4 = nullptr;

		}
		else {
			this->m_HighestProfileInfo = 4;
		}

		switch (this->m_HighestProfileInfo)
		{
		case 1:
			this->m_Container->g_MetadataHelpers = new MetadataHelpers(this->m_pICorProfilerInfo, this->m_Container);
			break;
		case 2:
			this->m_Container->g_MetadataHelpers = new MetadataHelpers(this->m_pICorProfilerInfo2, this->m_Container);
			break;
		case 3:
			this->m_Container->g_MetadataHelpers = new MetadataHelpers(this->m_pICorProfilerInfo3, this->m_Container);
			break;
		case 4:
			this->m_Container->g_MetadataHelpers = new MetadataHelpers(this->m_pICorProfilerInfo4, this->m_Container);
			break;
		default:
			return 0;
		}

		UINT_PTR * clientData = new UINT_PTR(0xDEADBEEF); // We should never see this in our map. This is basically a bounds checker.

		if (m_pICorProfilerInfo2 != NULL)
		{
			clientData = new UINT_PTR(0x0);; // Obviously we're not using any 
			m_pICorProfilerInfo2->SetFunctionIDMapper((FunctionIDMapper*)&Cprofilermain::Mapper1);
#ifdef _WIN64 
			m_pICorProfilerInfo2->SetEnterLeaveFunctionHooks2((FunctionEnter2*)&FunctionEnter2_Wrapper_x64, (FunctionLeave2*)&FunctionLeave2_Wrapper_x64, (FunctionTailcall2*)&FunctionTail2_Wrapper_x64);
			// TODO: Implement the x86 versions of the Enter/Tail/Leave function hooks.
#else
			m_pICorProfilerInfo2->SetEnterLeaveFunctionHooks2((FunctionEnter2*)&FunctionEnter2_x86, (FunctionLeave2*)&FunctionLeave2_x86, (FunctionTailcall2*)&FunctionTail2_x86);
#endif
		}
		if (m_pICorProfilerInfo3 != NULL)
		{
			// .NET40
			clientData = new UINT_PTR(40);
			m_pICorProfilerInfo3->SetFunctionIDMapper2((FunctionIDMapper2*)&Cprofilermain::Mapper2, this);
#ifdef _WIN64 

			m_pICorProfilerInfo3->SetEnterLeaveFunctionHooks2((FunctionEnter2*)&FunctionEnter2_Wrapper_x64, (FunctionLeave2*)&FunctionLeave2_Wrapper_x64, (FunctionTailcall2*)&FunctionTail2_Wrapper_x64);
			//m_pICorProfilerInfo3->SetEnterLeaveFunctionHooks2((FunctionEnter3*)&FunctionEnter2_Wrapper_x64, (FunctionLeave3*)&FunctionLeave2_Wrapper_x64, (FunctionTailcall3*)&FunctionTail2_Wrapper_x64);
			// TODO: Implement the x86 versions of the Enter/Tail/Leave function hooks.
#else

			m_pICorProfilerInfo2->SetEnterLeaveFunctionHooks2((FunctionEnter2*)&FunctionEnter2_x86, (FunctionLeave2*)&FunctionLeave2_x86, (FunctionTailcall2*)&FunctionTail2_x86);
			//m_pICorProfilerInfo3->SetEnterLeaveFunctionHooks3();
#endif
		}
		if (m_pICorProfilerInfo4 != NULL)
		{
			// .NET45
			clientData = new UINT_PTR(45);
			m_pICorProfilerInfo4->SetFunctionIDMapper2((FunctionIDMapper2*)&Cprofilermain::Mapper2, this);

#ifdef _WIN64 
			m_pICorProfilerInfo4->SetEnterLeaveFunctionHooks2((FunctionEnter2*)&FunctionEnter2_Wrapper_x64, (FunctionLeave2*)&FunctionLeave2_Wrapper_x64, (FunctionTailcall2*)&FunctionTail2_Wrapper_x64);
			//m_pICorProfilerInfo4->SetEnterLeaveFunctionHooks2((FunctionEnter3*)&FunctionEnter2_Wrapper_x64, (FunctionLeave3*)&FunctionLeave2_Wrapper_x64, (FunctionTailcall3*)&FunctionTail2_Wrapper_x64);
			// TODO: Implement the x86 versions of the Enter/Tail/Leave function hooks.
#else
			m_pICorProfilerInfo2->SetEnterLeaveFunctionHooks2((FunctionEnter2*)&FunctionEnter2_x86, (FunctionLeave2*)&FunctionLeave2_x86, (FunctionTailcall2*)&FunctionTail2_x86);
			//m_pICorProfilerInfo4->SetEnterLeaveFunctionHooks3();
#endif
		}
		Cprofilermain::g_StaticContainerClass->insert(std::pair<UINT_PTR, Cprofilermain*>(0x0, this));


		SetMask();



		return S_OK;
	}
	else {
		return E_FAIL;
	}
}

STDMETHODIMP Cprofilermain::AppDomainCreationStarted(AppDomainID appDomainId)
{
	return S_OK;
}

STDMETHODIMP Cprofilermain::ThreadCreated(ThreadID threadId)
{


	std::shared_ptr<ThreadStackItem> firstItem = std::make_shared<ThreadStackItem>(0, 0, threadId, THREAD_START);


	{ // Critsec block for thread depth start
		critsec_helper csh(&this->m_Container->g_ThreadStackDepthCriticalSection);
		this->m_Container->g_ThreadStackDepth->insert(std::pair<ThreadID, volatile unsigned int>(threadId, 0));
	}

	{ // Critsec block for thread sequence start
		critsec_helper csh(&this->m_Container->g_ThreadStackSequenceCriticalSection);
		this->m_Container->g_ThreadStackSequence->insert(std::pair<ThreadID, volatile unsigned int>(threadId, 0));
	}

	{ // Critsec block for thread insert start
		critsec_helper csh(&this->m_Container->g_ThreadingCriticalSection);
		this->m_Container->g_BigStack->push(firstItem);
	}  // Crit
	return S_OK;
}

STDMETHODIMP Cprofilermain::ThreadDestroyed(ThreadID threadId)
{
	{ // Critsec block for thread depth start
		critsec_helper csh(&this->m_Container->g_ThreadStackDepthCriticalSection);
		auto itStack = this->m_Container->g_ThreadStackMap->find(threadId);
		if (itStack != this->m_Container->g_ThreadStackMap->end())
		{
			// The front item should ALWAYS be the thread stack start item
			// If it's not some how something inserted a TSI before the top item and that is not likely.
			// Create 
			// lastTimer.AddThreadStackItem(this->m_Container->g_ThreadStackMap->at(threadId).back());
		}
	}
	//WriteLogFile();
	return S_OK;
}

STDMETHODIMP Cprofilermain::ThreadNameChanged(ThreadID threadId, ULONG cchName, _In_reads_opt_(cchName) WCHAR name[])
{
	//MAINCSENTER;
	{ // Critsec block for thread depth start
		critsec_helper csh(&this->m_Container->g_ThreadStackDepthCriticalSection);
		auto itStack = this->m_Container->g_ThreadStackMap->find(threadId);
		if (itStack != this->m_Container->g_ThreadStackMap->end())
		{
			// The front item should ALWAYS be the thread stack start item
			// If it's not some how something inserted a TSI before the top item and that is not likely.
			std::dynamic_pointer_cast<ThreadStackItem>(this->m_Container->g_ThreadStackMap->at(threadId)[0])->ThreadName(name);
		} // Critsec block for thread depth start
	}
	return S_OK;
}

STDMETHODIMP Cprofilermain::ModuleLoadFinished(ModuleID moduleId, HRESULT hrStatus)
{

	LPCBYTE baseAddress = new byte;
	wchar_t *stringName = NULL;
	ULONG cNameSize = 0;
	ULONG pcchNameSize = 0;
	AssemblyID asemId = {};
	this->m_pICorProfilerInfo2->GetModuleInfo(moduleId, &baseAddress, cNameSize, &pcchNameSize, stringName, &asemId);
	if (pcchNameSize > 0)
	{
		stringName = new wchar_t[pcchNameSize];
		cNameSize = pcchNameSize;
		this->m_pICorProfilerInfo2->GetModuleInfo(moduleId, &baseAddress, cNameSize, &pcchNameSize, stringName, &asemId);
		OFSTRUCT ofStruct = {};
		auto hFile = CreateFile(stringName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		LARGE_INTEGER li = {};
		GetFileSizeEx(hFile, &li);
		char * buffer = new char[li.LowPart] {0};
		DWORD bytesRead = 0;
		BOOL fileError = ReadFile(hFile, buffer, li.LowPart, &bytesRead, NULL);
		auto err = GetLastError();
		if (fileError == FALSE)
		{
			WSABUF wsaB = {};
			wsaB.buf = buffer;
		}
	}
	return S_OK;
}

STDMETHODIMP Cprofilermain::ModuleLoadStarted(ModuleID moduleId)
{
	return S_OK;
}

STDMETHODIMP Cprofilermain::ClassLoadFinished(ClassID classId, HRESULT hrStatus)
{
	/*ModuleID classModuleId;
	mdTypeDef classTypeDef;
	this->m_pICorProfilerInfo2->GetClassIDInfo(classId, &classModuleId, &classTypeDef);
	g_MetadataHelpers->InjectFieldToModule(classModuleId, classTypeDef, std::wstring(L"test"));*/
	return S_OK;
}

STDMETHODIMP Cprofilermain::RuntimeThreadSuspended(ThreadID threadId)
{
	// For all other runtime suspensions we'd like to know

	{ // Critsec block for thread depth start
		critsec_helper csh(&this->m_Container->g_ThreadingCriticalSection);
		auto itStack = this->m_Container->g_ThreadStackMap->find(threadId);

		if (itStack != this->m_Container->g_ThreadStackMap->end())
		{
			// TODO Implement a stack item insertion for the buffer
			//ti.AddThreadStackItem(itStack->second.back());
		}
	}
	return S_OK;
}

STDMETHODIMP Cprofilermain::RuntimeThreadResumed(ThreadID threadId)
{
	{ // Critsec block for thread depth start
		critsec_helper csh(&this->m_Container->g_ThreadingCriticalSection);
		auto itStack = this->m_Container->g_ThreadStackMap->find(threadId);

		if (itStack != this->m_Container->g_ThreadStackMap->end())
		{
			// TODO Implement a stack item insertion for the buffer
			// ti.AddThreadStackItem(itStack->second.back());
		}
	}
	return S_OK;
}

STDMETHODIMP Cprofilermain::GarbageCollectionStarted(int cGenerations, BOOL generationCollected[], COR_PRF_GC_REASON reason)
{
	m_CurrentGCReason = reason;
	return S_OK;
}

STDMETHODIMP Cprofilermain::GarbageCollectionFinished(void)
{
	return S_OK;
}

STDMETHODIMP Cprofilermain::RuntimeSuspendStarted(COR_PRF_SUSPEND_REASON suspendReason)
{
	m_IsRuntimeSuspended = TRUE;
	m_CurrentSuspendReason = suspendReason;
	return S_OK;
}

STDMETHODIMP Cprofilermain::RuntimeSuspendFinished(void)
{
	m_IsRuntimeSuspended = FALSE;
	return S_OK;
}

STDMETHODIMP Cprofilermain::RuntimeSuspendAborted(void)
{
	m_IsRuntimeSuspended = FALSE;
	return S_OK;
}

STDMETHODIMP Cprofilermain::RuntimeResumeStarted(void)
{
	return S_OK;
}

STDMETHODIMP Cprofilermain::RuntimeResumeFinished(void)
{
	return S_OK;
}

STDMETHODIMP Cprofilermain::JITCompilationStarted(FunctionID functionId, BOOL fIsSafeToBlock)
{
	ClassID classId;
	ModuleID moduleId;
	mdToken mdToken;
	this->m_pICorProfilerInfo->GetFunctionInfo(functionId, &classId, &moduleId, &mdToken);
	std::wstring s;
	Cprofilermain::GetFullMethodName(functionId, s);
	
	return S_OK;

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Class level static function hooks, cleaning up globals implementation
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Cprofilermain::FunctionEnterHook2(FunctionID funcId, UINT_PTR clientData,
	COR_PRF_FRAME_INFO func, COR_PRF_FUNCTION_ARGUMENT_INFO *argumentInfo)
{


	// CRITICAL 3 Create a threadpool and call it from these functions.
	// Make copies of the function parameters and pass them in as objects to the TP.
	// Only use 2 threads, we should allow queueing to take place.

	//// CRITICAL 2 Remove this code from this function block to speed up call time
	//webengine4helper &helper = webengine4helper::createhelper();

	//UINT_PTR reqInfo = *(UINT_PTR*)(argumentInfo->ranges[1].startAddress);
	////UINT_PTR reqInfo = (UINT_PTR)(argumentInfo->ranges->startAddress + argumentInfo->ranges->length);
	//UINT_PTR contType;
	//UINT_PTR totalLen;
	//UINT_PTR pathTranslated;
	//UINT_PTR cchPathTranslated;
	//UINT_PTR cacheURL;
	//UINT_PTR cchCacheURL;
	//UINT_PTR urlMax;
	//UINT_PTR cookedUrl;

	//helper.GetRequestBasics(
	//	reqInfo,
	//	&contType,
	//	&totalLen,
	//	&pathTranslated,
	//	&cchPathTranslated,
	//	&cacheURL,
	//	&cchCacheURL,
	//	&urlMax,
	//	&cookedUrl);

	//_HTTP_COOKED_URL * cooked = (_HTTP_COOKED_URL*)cookedUrl;

	//helper.SetUnkHeader(reqInfo, false, true, "X-chainsAPM", "JamesDavis", 11);
	/*
	MSDN Article that describes the ELT methods and what COR flags need to be set.
	http://msdn.microsoft.com/en-us/magazine/cc300553.aspx
	*/
	// Create a timer item that will calculate how long we spent trying to gather data




	// As we could be mapping a function while trying to find this guy it's possible
	// we would have a race condition.
	// However we will more than likely stop using this behavior as testing shows we don't need it
	//#pragma message(__TODO__"Do more testing around this 'feature' to see if we need it.")
	/*EnterCriticalSection(&g_ThreadingCriticalSection);
	std::map<FunctionID, FunctionInfo>::const_iterator it = g_FunctionSet->find(funcId);
	LeaveCriticalSection(&g_ThreadingCriticalSection);*/

	/*	if (it != g_FunctionSet->end())
	{*/
	//const FunctionInfo *fi = &it->second;

	ThreadID threadId;
	{
		critsec_helper csh(&this->m_Container->g_MetaDataCriticalSection);
		this->m_Container->g_MetadataHelpers->GetCurrentThread(&threadId);
	}

	// In order to stay async we need to copy this data before passing it off to a potentially blocking
	// operation, such as adding items to our global vectors.

	auto argumentsPtrRaw = new std::vector<UINT_PTR>();
	if (argumentInfo->numRanges != 0)
	{
		COR_PRF_FUNCTION_ARGUMENT_RANGE data;
		for (ULONG parameterCount = 0; parameterCount < argumentInfo->numRanges; parameterCount++)
		{
			data
				= COR_PRF_FUNCTION_ARGUMENT_RANGE((COR_PRF_FUNCTION_ARGUMENT_RANGE)argumentInfo->ranges[parameterCount]);

			if (data.startAddress != NULL)
			{
				argumentsPtrRaw->push_back(*(UINT_PTR*)data.startAddress);
			}
		}
	}

	critsec_helper cshFirst(&this->m_Container->g_FunctionSetCriticalSection);
	auto functionInfoFirst = this->m_Container->g_FunctionSet->find(funcId);
	cshFirst.leave_early(); // Leaving CS early because we don't need to worry about it
	BOOL isEntry = functionInfoFirst->second->IsEntryPoint();
	LONGLONG incremented = 0;
	if (isEntry)
	{
		incremented = InterlockedIncrement64(&this->m_Container->currentEntryPointCounter);;
	}
	// Increment the values in a way that is predictable and repeatable. The original approach was to determine the exact level per thread.
	int threadDepth = InterlockedIncrement(&this->m_Container->g_ThreadStackDepth->at(threadId));
	int threadSequence = InterlockedIncrement(&this->m_Container->g_ThreadStackSequence->at(threadId));


	std::async(std::launch::async, [&, funcId, threadId, isEntry, incremented, threadDepth, threadSequence, argumentsPtrRaw]  {
		// We are not completely doing the right thing here as I should use a C++11 contract to pass the shared_ptr
		auto arguments = std::make_shared<std::vector<UINT_PTR >>();
		arguments.reset(argumentsPtrRaw); // This however assigns the pointer to this shared_ptr and should live on.


		critsec_helper csh(&this->m_Container->g_FunctionSetCriticalSection);
		auto it = this->m_Container->g_FunctionSet->find(funcId);
		csh.leave_early();

		if (it != this->m_Container->g_FunctionSet->end())
		{

			// Make a copy of the return value for the potential blocking operation.
			auto itStack = this->m_Container->g_BigStack;
			auto  tsi = std::make_shared<FunctionStackItem>(threadDepth, threadSequence, threadId, ThreadStackReason::ENTER, funcId, arguments);
			// TODO add in function to add to network buffer
			// We will  need to lock this as only one thread can act on this list.
			{
				critsec_helper cshT(&this->m_Container->g_ThreadingCriticalSection);
				itStack->push(tsi);
			}

		}
	});
}

void Cprofilermain::FunctionLeaveHook2(FunctionID funcId, UINT_PTR clientData,
	COR_PRF_FRAME_INFO func, COR_PRF_FUNCTION_ARGUMENT_RANGE *argumentRange)
{
	// CRITICAL 3 Create a threadpool and call it from these functions.
	// Make copies of the function parameters and pass them in as objects to the TP.
	// Only use 2 threads, we should allow queueing to take place.


	{ // Critsec block for thread find start
		critsec_helper cshFirst(&this->m_Container->g_MetaDataCriticalSection);
		// Leaving CS early because we don't need to worry about it
		ThreadID threadId;
		this->m_Container->g_MetadataHelpers->GetCurrentThread(&threadId);
		UINT_PTR returnVal = NULL;
		int  threadDepth = 0;
		int threadSequence = InterlockedIncrement(&this->m_Container->g_ThreadStackSequence->at(threadId));

		if (argumentRange->startAddress != NULL)
		{
			returnVal = *(UINT_PTR*)argumentRange->startAddress;
		}

		if (this->m_Container->g_ThreadStackDepth->at(threadId) > 0)
		{
			threadDepth = InterlockedDecrement(&this->m_Container->g_ThreadStackDepth->at(threadId));
		}

		std::async(std::launch::async, [&, funcId, threadId, threadDepth, threadSequence, returnVal]{
			critsec_helper csh(&this->m_Container->g_FunctionSetCriticalSection);
			auto it = this->m_Container->g_FunctionSet->find(funcId);
			csh.leave_early();

			if (it != this->m_Container->g_FunctionSet->end())
			{
				// Make a copy of the return value for the potential blocking operation.
				auto itStack = this->m_Container->g_BigStack;
				auto  tsi = std::make_shared<FunctionStackItem>(threadDepth, threadSequence, threadId, ThreadStackReason::EXIT, funcId, returnVal);
				{
					critsec_helper cshT(&this->m_Container->g_ThreadingCriticalSection);
					itStack->push(tsi);
				}
			}
		});
	}
}

void Cprofilermain::FunctionTailHook2(FunctionID funcId, UINT_PTR clientData,
	COR_PRF_FRAME_INFO func)
{
	// CRITICAL 3 Create a threadpool and call it from these functions.
	// Make copies of the function parameters and pass them in as objects to the TP.
	// Only use 2 threads, we should allow queueing to take place.

	{ // Critsec block for thread find start
		critsec_helper cshFirst(&this->m_Container->g_MetaDataCriticalSection);
		ThreadID threadId;
		this->m_Container->g_MetadataHelpers->GetCurrentThread(&threadId);
		cshFirst.leave_early(); // Leaving CS early because we don't need to worry about it
		int  threadDepth = 0;
		int threadSequence = InterlockedIncrement(&this->m_Container->g_ThreadStackSequence->at(threadId));
		if (this->m_Container->g_ThreadStackDepth->at(threadId) > 0)
		{
			threadDepth = InterlockedDecrement(&this->m_Container->g_ThreadStackDepth->at(threadId));
		}
		std::async(std::launch::async, [&, funcId, threadId, threadDepth, threadSequence]{
				{

					auto itStack = this->m_Container->g_BigStack;
					auto  tsi = std::make_shared<FunctionStackItem>(threadDepth, threadSequence, threadId, ThreadStackReason::EXIT, funcId, NULL);
					{
						critsec_helper cshT(&this->m_Container->g_ThreadingCriticalSection);
						itStack->push(tsi);
					}
				}
		});

	}
	// TODO extract argument 
}

// Mapper function to create the collection of FunctionInfo items and set functions to be mapped.
UINT_PTR __stdcall Cprofilermain::Mapper1(FunctionID funcId, BOOL *pbHookFunction)
{
	// Find the Cprofiler class from the static container
	Cprofilermain * pContainerClass = g_StaticContainerClass->at(0x0);
	return pContainerClass->MapFunction(funcId, 0x0, pbHookFunction);
}

// Implementation of Mapper2. Create the collection of FunctionInfo items and set functions to be mapped.
UINT_PTR __stdcall Cprofilermain::Mapper2(FunctionID funcId, UINT_PTR clientData, BOOL *pbHookFunction)
{
	Cprofilermain * pContainerClass = (Cprofilermain*)clientData;
	if (funcId == clientData)
	{
		pContainerClass = Cprofilermain::g_StaticContainerClass->at(0x0);
	}
	return pContainerClass->MapFunction(funcId, clientData, pbHookFunction);
}

#define ALLMETHODS
UINT_PTR Cprofilermain::MapFunction(FunctionID funcId, UINT_PTR clientData, BOOL *pbHookFunction)
{
#ifdef ALLMETHODS

	FunctionInfo *funcInfo = new FunctionInfo();
	this->m_Container->g_MetadataHelpers->GetFunctionInformation(funcId, funcInfo);
	funcInfo->IsEntryPoint(TRUE);
	{ // Critsec block for thread depth start

		critsec_helper csh(&this->m_Container->g_FunctionSetCriticalSection);
		this->m_Container->g_FunctionSet->insert(std::pair<FunctionID, FunctionInfo*>(funcId, funcInfo));

	}
	*pbHookFunction = TRUE;

#else
	// While this method does not cause any upthe one below it does.
	critsec_helper csh(&this->m_Container->g_FunctionSetCriticalSection);
	auto it = this->m_Container->g_FunctionSet->find(funcId);
	csh.leave_early();
	if (it == this->m_Container->g_FunctionSet->end())
	{

		// funcInfo declared in this block so they are not created if the function is found

		FunctionInfo *funcInfo = new FunctionInfo();

		this->m_Container->g_MetadataHelpers->GetFunctionInformation(funcId, funcInfo);
		// These iterator operations should not cause a lock since it's only a read
		// and the find method does not alter the structure.

		// The function mapping happens at the during the class instantiation
		auto findClass =
			this->m_Container->g_ClassNameSet->find(funcInfo->ClassName());
		ItemMapping itemMap;
		itemMap.ItemName = funcInfo->FunctionName();
		auto findName =
			this->m_Container->g_FunctionNameSet->find(&itemMap);


		if ((findName != this->m_Container->g_FunctionNameSet->end()) | (findClass != this->m_Container->g_ClassNameSet->end()))
		{
			// Causes and update to this map and can be called from multiple threads.
			this->m_Container->g_MetadataHelpers->GetFunctionInformation(funcId, funcInfo);
			{ // Critsec block for thread depth start
				critsec_helper csh(&this->m_Container->g_FunctionSetCriticalSection);
				this->m_Container->g_FunctionSet->insert(std::pair<FunctionID, FunctionInfo*>(funcId, funcInfo));
			}
			*pbHookFunction = TRUE;
		}
		else {
			*pbHookFunction = FALSE;
	}

}
#endif // ALLMETHODS
	if (clientData == 0x0)
	{
		return (UINT_PTR)funcId;
	}
	return (UINT_PTR)this;
}