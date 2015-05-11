// profilermain.cpp : Implementation of Cprofilermain
#pragma once
#include "stdafx.h"
#include "ContainerClass.h"
#include "profilermain.h"
#include "srw_helper.h"
#include "critsec_helper.h"
#include "webengine4helper.h"
#include <future>
#include "AgentInfo.h"



// Maximum buffer size for input from file
#define BUFSIZE 2048

// String for mscorlib methods or not.
#define MSCORLIBCOMMAND L"Inserting into mscorlib: %c\r\n"

// Command strings for communicating out-of-process.
#define CMD_REJITFUNC L"pf"
#define CMD_REVERTFUNC L"rf"
#define CMD_QUIT L"qa"

// Response strings for communicating out-of-process.
#define RSP_REJITSUCCESS L"ps"
#define RSP_REVERTSUCCESS L"rs"
#define RSP_REJITFAILURE L"pf"
#define RSP_REVERTFAILURE L"rf"
#define RSP_QUITSUCCESS L"qs"

// Make sure the probe type matches the computer's architecture
#ifdef _WIN64
LPCWSTR k_wszEnteredFunctionProbeName = L"MgdEnteredFunction64";
LPCWSTR k_wszExitedFunctionProbeName = L"MgdExitedFunction64";
#else // Win32
LPCWSTR k_wszEnteredFunctionProbeName = L"MgdEnteredFunction32";
LPCWSTR k_wszExitedFunctionProbeName = L"MgdExitedFunction32";
#endif

// When pumping managed helpers into mscorlib, stick them into this pre-existing mscorlib type
LPCWSTR k_wszHelpersContainerType = L"System.CannotUnloadAppDomainException";

IDToInfoMap<ModuleID, ModuleInfo> m_moduleIDToInfoMap;

BOOL g_bShouldExit = FALSE;
BOOL g_bSafeToExit = FALSE;
UINT g_nLastRefid = 0;
BOOL g_fLogFilePathsInitiailized = FALSE;

// I read this file written by GUI to tell me what to do
WCHAR g_wszCmdFilePath[MAX_PATH] = { L'\0' };

// I write to this file to respond to the GUI
WCHAR g_wszResponseFilePath[MAX_PATH] = { L'\0' };

// I write additional diagnostic info to this file
WCHAR g_wszLogFilePath[MAX_PATH] = { L'\0' };

// I write the human-readable profiling results to this (HTML) file
WCHAR g_wszResultFilePath[MAX_PATH] = { L'\0' };


#define HEX(HR) L"0x" << std::hex << std::uppercase << HR << std::dec
#define RESULT_APPEND(EXPR) ;
#define RESPONSE_LITERAL(EXPR) ;
#define RESPONSE_APPEND(EXPR) RESPONSE_LITERAL(g_nLastRefid << L">" << EXPR << L"\n")
#define RESPONSE_IS(REFID, EXPR, MODULE, CLASS, FUNC) RESPONSE_LITERAL(REFID << L">" << EXPR << L"\t" << MODULE << L"\t" << CLASS << L"\t" << FUNC << L"\n")
#define RESPONSE_ERROR(EXPR) ;
#define LOG_APPEND(EXPR) ;
#define LOG_IFFAILEDRET(HR, EXPR) ;

// [extern] ilrewriter function for rewriting a module's IL
extern HRESULT RewriteIL(
	ICorProfilerInfo * pICorProfilerInfo,
	ICorProfilerFunctionControl * pICorProfilerFunctionControl,
	ModuleID moduleID,
	mdMethodDef methodDef,
	int nVersion,
	mdToken mdEnterProbeRef,
	mdToken mdExitProbeRef);

// [extern] ilrewriter function for setting helper IL
extern HRESULT SetILForManagedHelper(
	ICorProfilerInfo * pICorProfilerInfo,
	ModuleID moduleID,
	mdMethodDef mdHelperToAdd,
	mdMethodDef mdIntPtrExplicitCast,
	mdMethodDef mdPInvokeToCall);

// Struct used to hold the arguments for creating the file watcher thread. 
// Used since threadstart uses a LPVOID parameter for the called function.
struct threadargs
{
	ICorProfilerCallback * m_pCallback;
	LPCWSTR m_wszpath;
	IDToInfoMap<ModuleID, ModuleInfo> * m_iMap;
};

//************************************************************************************************//

//******************                    Forward Declarations                    ******************//            

//************************************************************************************************//

// [private] Checks to see if the given file has any changes, and if so executes the new commands.
DWORD WINAPI MonitorFile(LPVOID pFileAndModuleMap);

// [private] Checks to see if the given file exists.
bool FileExists(const PCWSTR wszFilepath);

// [private] Reads and executes a command from the file.
void ReadFile(FILE * fFile, LPVOID args);

// [private] Gets the MethodDef from the module, class and function names.
BOOL GetTokensFromNames(IDToInfoMap<ModuleID, ModuleInfo> * mMap, LPCWSTR wszModule, LPCWSTR wszClass, LPCWSTR wszFunction, ModuleID * moduleIDs, mdMethodDef * methodDefs, int cElementsMax, int * pcMethodsFound);

// [private] Returns TRUE iff wszContainer ends with wszProspectiveEnding (case-insensitive).
BOOL ContainsAtEnd(LPCWSTR wszContainer, LPCWSTR wszProspectiveEnding);




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

struct EnterfunctionWorkCallbackParamters
{
	Cprofilermain* profiler;
	FunctionID funcId;
	ThreadID threadId;
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

	// Get things like the process name, working directory, command line, etc.
	this->SetProcessName();
	if (this->DoWeProfile() == S_OK) // No reason to execute this code if the process is not what we're looking for.
	{
		this->m_Container = new ContainerClass();
		InitializeCriticalSection(&this->m_Container->g_ThreadingCriticalSection);

		m_NetworkClient = new NetworkClient(this->m_ServerName, this->m_ServerPort);

		this->AddCommonFunctions();

		size_t wcharSize(this->m_AgentName.length() - 1); // string
		size_t ainfoSize(sizeof(InformationClasses::AgentInfo) + (wcharSize * sizeof(wchar_t)) + sizeof(short)); // struct + string + plus terminator
		InformationClasses::AgentInfo * ainfo = (InformationClasses::AgentInfo*)malloc(ainfoSize);
		SecureZeroMemory(ainfo, ainfoSize);

		ainfo->AgentCapabilities = InformationClasses::AgentInfo::Capabilities::PROFILE;
		auto strhasher = std::hash<std::wstring>();
		ainfo->AgentHash = strhasher(this->m_AgentName);
		this->m_AgentName._Copy_s(ainfo->AgentName, this->m_AgentName.length() - 1, this->m_AgentName.length() - 1, 0);
		ainfo->AgentMajorVersion = 0;
		ainfo->AgentMinorVersion = 1;
		ainfo->AgentIncrementalVersion = 0;
		ainfo->AgentNameLen = this->m_AgentName.length() - 1;
		ainfo->Length = ainfoSize;
		ainfo->MachineNameLen = 16;
		this->m_ComputerName._Copy_s(ainfo->MachineName, this->m_ComputerName.length(), this->m_ComputerName.length(), 0);
		ainfo->MachineNameHash = strhasher(this->m_ComputerName);
		ainfo->Code = 5;
		m_NetworkClient->SendCommand<Commands::SendPackedStructure>(std::make_shared<Commands::SendPackedStructure>(Commands::SendPackedStructure(ainfo)));
		auto hr = m_NetworkClient->SendNow();
		tp = new tp_helper(this, 1, 1);
		tp->CreateNetworkIoThreadPool(m_NetworkClient);



		m_NetworkClient->Start(); // Ready for normal unblocked operation
	}

	DWORD tID = 0;

}

Cprofilermain::~Cprofilermain()
{


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

	auto newMapping2 = ItemMapping();
	//ItemMapping *newMapping3 = new ItemMapping();
	//ItemMapping *newMapping4 = new ItemMapping();
	//ItemMapping *newMapping5 = new ItemMapping();
	//ItemMapping *newMapping6 = new ItemMapping();
	//ItemMapping *newMapping7 = new ItemMapping();
	//ItemMapping *newMapping8 = new ItemMapping();
	//ItemMapping *newMapping9 = new ItemMapping();


	newMapping2.FunctionName = TEXT("AddNumbers");
	newMapping2.Match = ItemMapping::MatchType::FunctionOnly;
	this->m_Container->g_FullyQualifiedMethodsToProfile->insert(newMapping2);

	/*newMapping3->ItemName = TEXT("Thread");
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

	newMapping9->ItemName = TEXT("Run");
	newMapping9->compare = STRINGCOMPARE::CONTAINS;
	this->m_Container->g_FunctionNameSet->insert(newMapping9);*/


}

void Cprofilermain::SetProcessName()
{
	WCHAR imageName[MAX_PATH]{0};
	WCHAR currentPath[MAX_PATH]{0};
	WCHAR computerName[MAX_COMPUTERNAME_LENGTH + 1]{0};
	DWORD maxCNameLen = (MAX_COMPUTERNAME_LENGTH + 1);
	GetModuleFileName(NULL, imageName, MAX_PATH);
	LPWSTR cmdline = GetCommandLine();
	GetCurrentDirectory(MAX_PATH, currentPath);
	GetComputerName(computerName, &maxCNameLen);
	std::wstringstream stringStream(imageName);
	std::wstring lastItem;
	for (std::wstring item; std::getline(stringStream, item, L'\\');)
	{
		lastItem.assign(item);
	}
	this->m_ProcessName.assign(lastItem);
	this->m_ProcessDirectory.insert(0, currentPath);
	this->m_ProcessCommandLine.insert(0, cmdline);
	this->m_ComputerName.insert(0, computerName);
	this->m_ProcessId = GetCurrentProcessId();

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
		| COR_PRF_MONITOR_ENTERLEAVE
		| COR_PRF_ENABLE_FRAME_INFO
		| COR_PRF_ENABLE_FUNCTION_ARGS
		| COR_PRF_ENABLE_FUNCTION_RETVAL
		| COR_PRF_MONITOR_THREADS
		//| COR_PRF_MONITOR_GC
		| COR_PRF_MONITOR_SUSPENDS
		| COR_PRF_MONITOR_EXCEPTIONS
		| COR_PRF_MONITOR_CLR_EXCEPTIONS
		| COR_PRF_MONITOR_CLASS_LOADS
		| COR_PRF_MONITOR_MODULE_LOADS
		| COR_PRF_MONITOR_ASSEMBLY_LOADS
		| COR_PRF_MONITOR_APPDOMAIN_LOADS
		| COR_PRF_MONITOR_CODE_TRANSITIONS);
	//| COR_PRF_ENABLE_REJIT
	//| COR_PRF_DISABLE_ALL_NGEN_IMAGES
	//| COR_PRF_MONITOR_JIT_COMPILATION); // I will turn this back on when the IL Rewrite is ready
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

STDMETHODIMP Cprofilermain::DoWeProfile()
{
	HKEY openKey;
	HRESULT result = RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\ChainsAPM\\Agents", NULL, KEY_READ, &openKey);
	if (result != S_OK)
	{
		int lastErr = GetLastError();
	}
	wchar_t* lpStr = new wchar_t[255];
	unsigned long lpDw = 255;

	HRESULT key = RegEnumKeyEx(openKey, 0, lpStr, &lpDw, nullptr, nullptr, nullptr, nullptr);
	auto fullString = std::wstring(TEXT("SOFTWARE\\ChainsAPM\\Agents\\"));
	auto currentKey = std::wstring(L"");

	HRESULT gotValue;
	unsigned long	Enabled = 0;
	wchar_t*		BufferForStrings = new wchar_t[255];
	unsigned long	BufferSize = 255;
	unsigned long	BufferForDWORD = 4;
	unsigned long	Port = 0;
	unsigned long	PortType = 0;
	bool IsAMatch = false;
	int counter = 0;
	while (key == ERROR_SUCCESS && !IsAMatch)
	{

		currentKey.append(fullString);
		currentKey.append(lpStr);

		gotValue = RegGetValue(HKEY_LOCAL_MACHINE, currentKey.data(), L"Enabled", RRF_RT_ANY | RRF_ZEROONFAILURE, NULL, &Enabled, &BufferForDWORD);
		if (gotValue != S_OK | Enabled == 0)
		{
			currentKey.assign(nullptr);
			continue;
		}
		gotValue = RegGetValue(HKEY_LOCAL_MACHINE, currentKey.data(), L"LinkName", RRF_RT_ANY | RRF_ZEROONFAILURE, nullptr, nullptr, &BufferSize);
		gotValue = RegGetValue(HKEY_LOCAL_MACHINE, currentKey.data(), L"LinkName", RRF_RT_ANY | RRF_ZEROONFAILURE, nullptr, BufferForStrings, &BufferSize);
		if (gotValue == S_OK)
		{
			if (BufferForStrings != NULL)
			{
				IsAMatch = true;
				m_AgentName.insert(0, BufferForStrings, (BufferSize / 2));
			}
			else {
				IsAMatch = false;
			}

		}

		gotValue = RegGetValue(HKEY_LOCAL_MACHINE, currentKey.data(), L"ProcessName", RRF_RT_ANY | RRF_ZEROONFAILURE, nullptr, nullptr, &BufferSize);
		gotValue = RegGetValue(HKEY_LOCAL_MACHINE, currentKey.data(), L"ProcessName", RRF_RT_ANY | RRF_ZEROONFAILURE, nullptr, BufferForStrings, &BufferSize);
		if (gotValue == S_OK)
		{
			if (BufferForStrings != NULL && m_ProcessName.compare(BufferForStrings) != 0)
			{
				IsAMatch = false;
			}
			IsAMatch = true;
		}
		gotValue = RegGetValue(HKEY_LOCAL_MACHINE, currentKey.data(), L"Directory", RRF_RT_ANY | RRF_ZEROONFAILURE, nullptr, nullptr, &BufferSize);
		gotValue = RegGetValue(HKEY_LOCAL_MACHINE, currentKey.data(), L"Directory", RRF_RT_ANY | RRF_ZEROONFAILURE, nullptr, BufferForStrings, &BufferSize);
		if (gotValue == S_OK)
		{
			if (BufferForStrings != NULL && m_ProcessDirectory.find(BufferForStrings) != 0)
			{
				IsAMatch = false;
			}
			IsAMatch = true;
		}
		gotValue = RegGetValue(HKEY_LOCAL_MACHINE, currentKey.data(), L"CommandLine", RRF_RT_ANY | RRF_ZEROONFAILURE, nullptr, nullptr, &BufferSize);
		gotValue = RegGetValue(HKEY_LOCAL_MACHINE, currentKey.data(), L"CommandLine", RRF_RT_ANY | RRF_ZEROONFAILURE, nullptr, BufferForStrings, &BufferSize);
		if (gotValue == S_OK)
		{
			if (BufferForStrings != NULL && m_ProcessCommandLine.find(BufferForStrings) != 0)
			{
				IsAMatch = false;
			}
			IsAMatch = true;
		}
		gotValue = RegGetValue(HKEY_LOCAL_MACHINE, currentKey.data(), L"Server", RRF_RT_ANY | RRF_ZEROONFAILURE, nullptr, nullptr, &BufferSize);
		gotValue = RegGetValue(HKEY_LOCAL_MACHINE, currentKey.data(), L"Server", RRF_RT_ANY | RRF_ZEROONFAILURE, nullptr, BufferForStrings, &BufferSize);
		if (gotValue == S_OK)
		{
			if (BufferForStrings == NULL || BufferForStrings == L"")
			{
				IsAMatch = true;
				m_ServerName.insert(0, BufferForStrings, (BufferSize / 2));
			}
			else {
				IsAMatch = false;
			}
		}
		gotValue = RegGetValue(HKEY_LOCAL_MACHINE, currentKey.data(), L"Port", RRF_RT_ANY | RRF_ZEROONFAILURE, NULL, &Port, &BufferForDWORD);
		if (gotValue == S_OK)
		{
			if (Port < 0 | Port > 0xFFFF)
			{
				IsAMatch = false;
			}
			else {
				m_ServerPort = std::to_wstring(Port);
				IsAMatch = true;
			}

		}
		if (IsAMatch)
		{
			continue;
		}
		else {
			currentKey.assign(nullptr);
		}
		key = RegEnumKeyEx(openKey, ++counter, lpStr, &lpDw, NULL, NULL, NULL, NULL);

	}
	RegCloseKey(openKey);

	if (IsAMatch)
	{
		return S_OK;
	}
	return E_FAIL;
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
	return S_OK;
}

STDMETHODIMP Cprofilermain::ThreadDestroyed(ThreadID threadId)
{

	return S_OK;
}

STDMETHODIMP Cprofilermain::ThreadNameChanged(ThreadID threadId, ULONG cchName, _In_reads_opt_(cchName) WCHAR name[])
{
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

HRESULT STDMETHODCALLTYPE Cprofilermain::AssemblyLoadFinished(AssemblyID assemblyId, HRESULT hrStatus)
{
	LPCBYTE baseAddress = new byte;
	wchar_t *stringName = NULL;
	ULONG cNameSize = 0;
	ULONG pcchNameSize = 0;
	AssemblyID asemId = {};
	AppDomainID appDomainId;
	ModuleID moduleId;
	this->m_pICorProfilerInfo2->GetAssemblyInfo(assemblyId, cNameSize, &pcchNameSize, stringName, &appDomainId, &moduleId);
	if (pcchNameSize > 0)
	{
		stringName = new wchar_t[pcchNameSize];
		cNameSize = pcchNameSize;
		this->m_pICorProfilerInfo2->GetAssemblyInfo(assemblyId, cNameSize, &pcchNameSize, stringName, &appDomainId, &moduleId);
		//this->m_pICorProfilerInfo2->GetModuleInfo(moduleId, &baseAddress, cNameSize, &pcchNameSize, stringName, &asemId);
		//TODO Add Call to ICorProfilerInfo::GetModuleMetaData
		//TODO Add Call to IMetaDataAssemblyImport::GetAssemblyProps
	}
	return S_OK;
}



STDMETHODIMP Cprofilermain::ModuleLoadStarted(ModuleID moduleId)
{
	return S_OK;
}

STDMETHODIMP Cprofilermain::ClassLoadFinished(ClassID classId, HRESULT hrStatus)
{
	ModuleID classModuleId;
	mdTypeDef classTypeDef;
	this->m_pICorProfilerInfo2->GetClassIDInfo(classId, &classModuleId, &classTypeDef);
	//g_MetadataHelpers->InjectFieldToModule(classModuleId, classTypeDef, std::wstring(L"test"));*/
	return S_OK;
}

STDMETHODIMP Cprofilermain::RuntimeThreadSuspended(ThreadID threadId)
{
	return S_OK;
}

STDMETHODIMP Cprofilermain::RuntimeThreadResumed(ThreadID threadId)
{
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

STDMETHODIMP Cprofilermain::Shutdown(void)
{
	m_NetworkClient->SendCommand<Commands::SendString>(std::make_shared<Commands::SendString>(Commands::SendString(std::wstring(L"Done!"))));
	m_NetworkClient->SendNow();
	Sleep(3000);
	return S_OK;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Class level static function hooks, cleaning up globals implementation
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Cprofilermain::FunctionEnterHook2(FunctionID funcId, UINT_PTR clientData,
	COR_PRF_FRAME_INFO func, COR_PRF_FUNCTION_ARGUMENT_INFO *argumentInfo)
{

	/*
	MSDN Article that describes the ELT methods and what COR flags need to be set.
	http://msdn.microsoft.com/en-us/magazine/cc300553.aspx
	*/
	ThreadID threadId = 0;
	{
		//critsec_helper csh(&this->m_Container->g_MetaDataCriticalSection);
		this->m_Container->g_MetadataHelpers->GetCurrentThread(&threadId);
		//csh.leave_early();
	}

	FILETIME HighPrecisionFileTime{ 0 };
	GetSystemTimeAsFileTime(&HighPrecisionFileTime);
	__int64 timestamp = (((__int64)HighPrecisionFileTime.dwHighDateTime) << 32) + HighPrecisionFileTime.dwLowDateTime;

	// In order to stay async we need to copy this data before passing it off to a potentially blocking
	// operation, such as sending data over the wire.

	auto argumentsPtrRaw = new std::vector<UINT_PTR>();
	if (argumentInfo != nullptr && argumentInfo->numRanges != 0)
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



	tp->SendEvent<Commands::FunctionEnterQuick>(new Commands::FunctionEnterQuick(funcId, threadId, timestamp));
}


void Cprofilermain::FunctionLeaveHook2(FunctionID funcId, UINT_PTR clientData,
	COR_PRF_FRAME_INFO func, COR_PRF_FUNCTION_ARGUMENT_RANGE *argumentRange)
{
	ThreadID threadId = 0;
	{
		//critsec_helper csh(&this->m_Container->g_MetaDataCriticalSection);
		this->m_Container->g_MetadataHelpers->GetCurrentThread(&threadId);
		//csh.leave_early();
	}

	FILETIME HighPrecisionFileTime{ 0 };
	GetSystemTimeAsFileTime(&HighPrecisionFileTime);
	__int64 timestamp = (((__int64)HighPrecisionFileTime.dwHighDateTime) << 32) + HighPrecisionFileTime.dwLowDateTime;

	tp->SendEvent<Commands::FunctionLeaveQuick>(new Commands::FunctionLeaveQuick(funcId, threadId, timestamp));

}

void Cprofilermain::FunctionTailHook2(FunctionID funcId, UINT_PTR clientData,
	COR_PRF_FRAME_INFO func)
{
	ThreadID threadId = 0;
	{
		//critsec_helper csh(&this->m_Container->g_MetaDataCriticalSection);
		this->m_Container->g_MetadataHelpers->GetCurrentThread(&threadId);
		//csh.leave_early();
	}

	FILETIME HighPrecisionFileTime{ 0 };
	GetSystemTimeAsFileTime(&HighPrecisionFileTime);
	__int64 timestamp = (((__int64)HighPrecisionFileTime.dwHighDateTime) << 32) + HighPrecisionFileTime.dwLowDateTime;

	tp->SendEvent<Commands::FunctionTailQuick>(new Commands::FunctionTailQuick(funcId, threadId, timestamp));
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

	InformationClasses::FunctionInfo *funcInfo = new InformationClasses::FunctionInfo();
	this->m_Container->g_MetadataHelpers->GetFunctionInformation(funcId, funcInfo);
	FILETIME HighPrecisionFileTime{ 0 };
	GetSystemTimeAsFileTime(&HighPrecisionFileTime);
	__int64 timestamp = (((__int64)HighPrecisionFileTime.dwHighDateTime) << 32) + HighPrecisionFileTime.dwLowDateTime;

	tp->SendEvent<Commands::DefineFunction>(new Commands::DefineFunction(funcId, funcInfo->ClassInformation().ClassId(), funcInfo->SignatureString(), timestamp));
	*pbHookFunction = TRUE;

#else

	// funcInfo declared in this block so they are not created if the function is found

	InformationClasses::FunctionInfo *funcInfo = new InformationClasses::FunctionInfo();

	this->m_Container->g_MetadataHelpers->GetFunctionInformation(funcId, funcInfo);

	ItemMapping itemMap;
	itemMap.FunctionName = funcInfo->FunctionName();
	itemMap.ClassName = funcInfo->ClassInformation().ClassName();
	itemMap.Signature = funcInfo->SignatureString();

	// These iterator operations should not cause a lock since it's only a read
	// and the find method does not alter the structure. Plus the function mapping 
	// happens at the during the class instantiation.

	auto findItem = this->m_Container->g_FullyQualifiedMethodsToProfile->find(itemMap);

	if ((findItem != this->m_Container->g_FullyQualifiedMethodsToProfile->end()))
	{
		FILETIME HighPrecisionFileTime{ 0 };
		GetSystemTimeAsFileTime(&HighPrecisionFileTime);
		__int64 timestamp = (((__int64)HighPrecisionFileTime.dwHighDateTime) << 32) + HighPrecisionFileTime.dwLowDateTime;

		tp->SendEvent<Commands::DefineFunction>(new Commands::DefineFunction(funcId, funcInfo->ClassInformation().ClassId(), funcInfo->SignatureString(), timestamp));
		*pbHookFunction = TRUE;
	}
	else {
		*pbHookFunction = FALSE;
	}


#endif // ALLMETHODS
	if (clientData == 0x0)
	{
		return (UINT_PTR)funcId;
	}
	return (UINT_PTR)this;
}

// [private] Adds memberRefs to the managed helper into the module so that we can ReJIT later.
void Cprofilermain::AddMemberRefs(IMetaDataAssemblyImport * pAssemblyImport, IMetaDataAssemblyEmit * pAssemblyEmit, IMetaDataEmit * pEmit, ModuleInfo * pModuleInfo)
{
	assert(pModuleInfo != NULL);

	LOG_APPEND(L"Adding memberRefs in this module to point to the helper managed methods");

	IMetaDataImport * pImport = pModuleInfo->m_pImport;

	HRESULT hr;

	// Signature for method the rewritten IL will call:
	// - public static void MgdEnteredFunction64(UInt64 moduleIDCur, UInt32 mdCur, int nVersionCur)
	// - public static void MgdEnteredFunction32(UInt32 moduleIDCur, UInt32 mdCur, int nVersionCur)

#ifdef _WIN64
	COR_SIGNATURE sigFunctionProbe[] = {
		IMAGE_CEE_CS_CALLCONV_DEFAULT,      // default calling convention
		0x03,                               // number of arguments == 3
		ELEMENT_TYPE_VOID,                  // return type == void
		ELEMENT_TYPE_U8,                    // arg 1: UInt64 moduleIDCur
		ELEMENT_TYPE_U4,                    // arg 2: UInt32 mdCur
		ELEMENT_TYPE_I4,                    // arg 3: int nVersionCur
	};
#else //  ! _WIN64 (32-bit code follows)
	COR_SIGNATURE sigFunctionProbe[] = {
		IMAGE_CEE_CS_CALLCONV_DEFAULT,      // default calling convention
		0x03,                               // number of arguments == 3
		ELEMENT_TYPE_VOID,                  // return type == void
		ELEMENT_TYPE_U4,                    // arg 1: UInt32 moduleIDCur
		ELEMENT_TYPE_U4,                    // arg 2: UInt32 mdCur
		ELEMENT_TYPE_I4,                    // arg 3: int nVersionCur
	};
#endif //_WIN64

	mdAssemblyRef assemblyRef = NULL;
	mdTypeRef typeRef = mdTokenNil;

	if (m_fInstrumentationHooksInSeparateAssembly)
	{
		// Generate assemblyRef to ProfilerHelper.dll
		BYTE rgbPublicKeyToken[] = { 0xfc, 0xb7, 0x40, 0xf6, 0x34, 0x46, 0xe2, 0xf2 };
		WCHAR wszLocale[MAX_PATH];
		wcscpy_s(wszLocale, L"neutral");

		ASSEMBLYMETADATA assemblyMetaData;
		ZeroMemory(&assemblyMetaData, sizeof(assemblyMetaData));
		assemblyMetaData.usMajorVersion = 1;
		assemblyMetaData.usMinorVersion = 0;
		assemblyMetaData.usBuildNumber = 0;
		assemblyMetaData.usRevisionNumber = 0;
		assemblyMetaData.szLocale = wszLocale;
		assemblyMetaData.cbLocale = _countof(wszLocale);

		hr = pAssemblyEmit->DefineAssemblyRef(
			(void *)rgbPublicKeyToken,
			sizeof(rgbPublicKeyToken),
			L"ProfilerHelper",
			&assemblyMetaData,
			NULL,                   // hash blob
			NULL,                   // cb of hash blob
			0,                      // flags
			&assemblyRef);

		if (FAILED(hr))
		{
			LOG_APPEND(L"DefineAssemblyRef failed, hr = " << HEX(hr));
		}
	}
	else
	{
		// Probes are being added to mscorlib. Find existing mscorlib assemblyRef.

		HCORENUM hEnum = NULL;
		mdAssemblyRef rgAssemblyRefs[20];
		ULONG cAssemblyRefsReturned;
		assemblyRef = mdTokenNil;

		do
		{
			hr = pAssemblyImport->EnumAssemblyRefs(
				&hEnum,
				rgAssemblyRefs,
				_countof(rgAssemblyRefs),
				&cAssemblyRefsReturned);

			if (FAILED(hr))
			{
				LOG_APPEND(L"EnumAssemblyRefs failed, hr = " << HEX(hr));
				return;
			}

			if (cAssemblyRefsReturned == 0)
			{
				LOG_APPEND(L"Could not find an AssemblyRef to mscorlib");
				return;
			}
		} while (!FindMscorlibReference(
			pAssemblyImport,
			rgAssemblyRefs,
			cAssemblyRefsReturned,
			&assemblyRef));

		pAssemblyImport->CloseEnum(hEnum);
		hEnum = NULL;

		assert(assemblyRef != mdTokenNil);
	}

	// Generate typeRef to ILRewriteProfilerHelper.ProfilerHelper or the pre-existing mscorlib type
	// that we're adding the managed helpers to.

	LPCWSTR wszTypeToReference =
		m_fInstrumentationHooksInSeparateAssembly ?
		L"ILRewriteProfilerHelper.ProfilerHelper" :
		k_wszHelpersContainerType;

	hr = pEmit->DefineTypeRefByName(
		assemblyRef,
		wszTypeToReference,
		&typeRef);

	if (FAILED(hr))
	{
		LOG_APPEND(L"DefineTypeRefByName to " << wszTypeToReference << L" failed, hr = " << HEX(hr));
	}

	hr = pEmit->DefineMemberRef(
		typeRef,
		k_wszEnteredFunctionProbeName,
		sigFunctionProbe,
		sizeof(sigFunctionProbe),
		&(pModuleInfo->m_mdEnterProbeRef));

	if (FAILED(hr))
	{
		LOG_APPEND(L"DefineMemberRef to " << k_wszEnteredFunctionProbeName <<
			L" failed, hr = " << HEX(hr));
	}

	hr = pEmit->DefineMemberRef(
		typeRef,
		k_wszExitedFunctionProbeName,
		sigFunctionProbe,
		sizeof(sigFunctionProbe),
		&(pModuleInfo->m_mdExitProbeRef));

	if (FAILED(hr))
	{
		LOG_APPEND(L"DefineMemberRef to " << k_wszExitedFunctionProbeName <<
			L" failed, hr = " << HEX(hr));
	}
}

// [private] Gets the reference to mscorlib, in case pumping helpers into it is necessary.
BOOL Cprofilermain::FindMscorlibReference(IMetaDataAssemblyImport * pAssemblyImport, mdAssemblyRef * rgAssemblyRefs, ULONG cAssemblyRefs, mdAssemblyRef * parMscorlib)
{
	HRESULT hr;

	for (ULONG i = 0; i < cAssemblyRefs; i++)
	{
		const void * pvPublicKeyOrToken;
		ULONG cbPublicKeyOrToken;
		WCHAR wszName[512];
		ULONG cchNameReturned;
		ASSEMBLYMETADATA asmMetaData;
		ZeroMemory(&asmMetaData, sizeof(asmMetaData));
		const void * pbHashValue;
		ULONG cbHashValue;
		DWORD asmRefFlags;

		hr = pAssemblyImport->GetAssemblyRefProps(
			rgAssemblyRefs[i],
			&pvPublicKeyOrToken,
			&cbPublicKeyOrToken,
			wszName,
			_countof(wszName),
			&cchNameReturned,
			&asmMetaData,
			&pbHashValue,
			&cbHashValue,
			&asmRefFlags);

		if (FAILED(hr))
		{
			LOG_APPEND(L"GetAssemblyRefProps failed, hr = " << HEX(hr));
			return FALSE;
		}

		if (::ContainsAtEnd(wszName, L"mscorlib"))
		{
			*parMscorlib = rgAssemblyRefs[i];
			return TRUE;
		}
	}

	return FALSE;
}

// [private] Adds appropriate methodDefs to mscorlib for the managed helper probes.
void Cprofilermain::AddHelperMethodDefs(IMetaDataImport * pImport, IMetaDataEmit * pEmit)
{
	HRESULT hr;

	assert(!m_fInstrumentationHooksInSeparateAssembly);

	LOG_APPEND(L"Adding methodDefs to mscorlib metadata for managed helper probes");

	// The helpers will need to call into System.IntPtr::op_Explicit(int64), so get methodDef now
	mdTypeDef tdSystemIntPtr;
	hr = pImport->FindTypeDefByName(L"System.IntPtr", mdTypeDefNil, &tdSystemIntPtr);

	if (FAILED(hr))
	{
		LOG_APPEND(L"FindTypeDefByName(System.IntPtr) failed, hr = " << HEX(hr));
		return;
	}

	COR_SIGNATURE intPtrOpExplicitSignature[] = {
		IMAGE_CEE_CS_CALLCONV_DEFAULT,
		1,               // 1 argument
		ELEMENT_TYPE_I,  // return type is native int
		ELEMENT_TYPE_I8, // argument type is int64
	};

	hr = pImport->FindMethod(
		tdSystemIntPtr,
		L"op_Explicit",
		intPtrOpExplicitSignature,
		sizeof(intPtrOpExplicitSignature),
		&m_mdIntPtrExplicitCast);

	if (FAILED(hr))
	{
		LOG_APPEND(L"FindMethod(System.IntPtr.op_Explicit(int64)) failed, hr = " << HEX(hr));
		return;
	}

	// Put the managed helpers into this pre-existing mscorlib type
	mdTypeDef tdHelpersContainer;
	hr = pImport->FindTypeDefByName(k_wszHelpersContainerType, mdTypeDefNil, &tdHelpersContainer);

	if (FAILED(hr))
	{
		LOG_APPEND(L"FindTypeDefByName(" << k_wszHelpersContainerType <<
			L") failed, hr = " << HEX(hr));
		return;
	}

	// Get a dummy method implementation RVA (CLR doesn't like you passing 0).  Pick a
	// ctor on the same type.
	COR_SIGNATURE ctorSignature[] =
	{
		IMAGE_CEE_CS_CALLCONV_HASTHIS, //__stdcall
		0,
		ELEMENT_TYPE_VOID
	};

	mdMethodDef mdCtor = NULL;
	hr = pImport->FindMethod(
		tdHelpersContainer,
		L".ctor",
		ctorSignature,
		sizeof(ctorSignature),
		&mdCtor);

	if (FAILED(hr))
	{
		LOG_APPEND(L"FindMethod(" << k_wszHelpersContainerType <<
			L"..ctor) failed, hr = " << HEX(hr));
		return;
	}

	ULONG rvaCtor;
	hr = pImport->GetMethodProps(
		mdCtor,
		NULL,		   // Put method's class here. 
		NULL,		   // Put method's name here.  
		0,			   // Size of szMethod buffer in wide chars.   
		NULL,		   // Put actual size here 
		NULL,		   // Put flags here.  
		NULL,		   // [OUT] point to the blob value of meta data   
		NULL,		   // [OUT] actual size of signature blob  
		&rvaCtor,
		NULL);

	if (FAILED(hr))
	{
		LOG_APPEND(L"GetMethodProps(" << k_wszHelpersContainerType <<
			L"..ctor) failed, hr = " << HEX(hr));
		return;
	}

	// Generate reference to unmanaged profiler DLL (i.e., us)
	mdModuleRef modrefNativeExtension;
	hr = pEmit->DefineModuleRef(L"ILRewriteProfiler", &modrefNativeExtension);

	if (FAILED(hr))
	{
		LOG_APPEND(L"DefineModuleRef against the native profiler DLL failed, hr = " << HEX(hr));
		return;
	}

	// Generate the PInvokes into the profiler DLL
	AddPInvoke(
		pEmit,
		tdHelpersContainer,
		L"NtvEnteredFunction",
		modrefNativeExtension,
		&m_mdEnterPInvoke);

	AddPInvoke(
		pEmit,
		tdHelpersContainer,
		L"NtvExitedFunction",
		modrefNativeExtension,
		&m_mdExitPInvoke);

	// Generate the SafeCritical managed methods which call the PInvokes
	mdMethodDef mdSafeCritical;
	GetSecuritySafeCriticalAttributeToken(pImport, &mdSafeCritical);

	AddManagedHelperMethod(
		pEmit,
		tdHelpersContainer,
		k_wszEnteredFunctionProbeName,
		m_mdEnterPInvoke,
		rvaCtor,
		mdSafeCritical, &m_mdEnter);

	AddManagedHelperMethod(
		pEmit,
		tdHelpersContainer,
		k_wszExitedFunctionProbeName,
		m_mdExitPInvoke,
		rvaCtor,
		mdSafeCritical,
		&m_mdExit);
}

// [private] Creates a PInvoke method to inject into mscorlib.
HRESULT Cprofilermain::AddPInvoke(IMetaDataEmit * pEmit, mdTypeDef td, LPCWSTR wszName, mdModuleRef modrefTarget, mdMethodDef * pmdPInvoke)
{
	HRESULT hr;

	//COR_SIGNATURE representation
	//   Calling convention
	//   Number of Arguments
	//   Return type
	//   Argument type
	//   ...

	COR_SIGNATURE newMethodSignature[] = { IMAGE_CEE_CS_CALLCONV_DEFAULT,   //__stdcall
		3,                               // 3 inputs
		ELEMENT_TYPE_VOID,               // No return
		ELEMENT_TYPE_I,                  // ModuleID
		ELEMENT_TYPE_U4,                 // mdMethodDef token
		ELEMENT_TYPE_I4                  // Rejit version number
	};

	hr = pEmit->DefineMethod(
		td,
		wszName,
		~mdAbstract & (mdStatic | mdPublic | mdPinvokeImpl),
		newMethodSignature,
		sizeof(newMethodSignature),
		0,
		miPreserveSig,
		pmdPInvoke);

	LOG_IFFAILEDRET(hr, L"Failed in DefineMethod when creating P/Invoke method " << wszName);

	hr = pEmit->DefinePinvokeMap(
		*pmdPInvoke,
		pmCallConvStdcall | pmNoMangle,
		wszName,
		modrefTarget);

	LOG_IFFAILEDRET(hr, L"Failed in DefinePinvokeMap when creating P/Invoke method " << wszName);

	return hr;
}

// [private] Gets the SafeCritical token to use when injecting methods into mscorlib.
HRESULT Cprofilermain::GetSecuritySafeCriticalAttributeToken(IMetaDataImport * pImport, mdMethodDef * pmdSafeCritical)
{
	mdTypeDef tdSafeCritical;

	HRESULT hr = pImport->FindTypeDefByName(
		L"System.Security.SecuritySafeCriticalAttribute",
		mdTokenNil,
		&tdSafeCritical);

	LOG_IFFAILEDRET(hr, L"FindTypeDefByName(System.Security.SecuritySafeCriticalAttribute) failed");

	COR_SIGNATURE sigSafeCriticalCtor[] = {
		IMAGE_CEE_CS_CALLCONV_HASTHIS,
		0x00,                               // number of arguments == 0
		ELEMENT_TYPE_VOID,                  // return type == void
	};

	hr = pImport->FindMember(
		tdSafeCritical,
		L".ctor",
		sigSafeCriticalCtor,
		sizeof(sigSafeCriticalCtor),
		pmdSafeCritical);

	LOG_IFFAILEDRET(hr, L"FindMember(System.Security.SecuritySafeCriticalAttribute..ctor) failed");

	return hr;
}

// [private] Adds the managed helper methods to mscorlib.
HRESULT Cprofilermain::AddManagedHelperMethod(IMetaDataEmit * pEmit, mdTypeDef td, LPCWSTR wszName, mdMethodDef mdTargetPInvoke, ULONG rvaDummy, mdMethodDef mdSafeCritical, mdMethodDef * pmdHelperMethod)
{
	HRESULT hr;

	COR_SIGNATURE newMethodSignature[] = {
		IMAGE_CEE_CS_CALLCONV_DEFAULT,  //__stdcall
		3,
		ELEMENT_TYPE_VOID,              // returns void
#ifdef _X86_
		ELEMENT_TYPE_U4,                // ModuleID
#elif defined(_AMD64_)
		ELEMENT_TYPE_U8,                // ModuleID
#else
#error THIS SAMPLE ONLY WORKS ON X86 AND X64
#endif
		ELEMENT_TYPE_U4,                // mdMethodDef token
		ELEMENT_TYPE_I4,                // Rejit version number
	};

	hr = pEmit->DefineMethod(
		td,
		wszName,
		mdStatic | mdPublic,
		newMethodSignature,
		sizeof(newMethodSignature),
		rvaDummy,
		miIL | miNoInlining,
		pmdHelperMethod);

	LOG_IFFAILEDRET(hr, L"Failed in DefineMethod when creating managed helper method " << wszName);

	mdToken tkCustomAttribute;
	hr = pEmit->DefineCustomAttribute(
		*pmdHelperMethod,
		mdSafeCritical,
		NULL,          //Blob, contains constructor params in this case none
		0,             //Size of the blob
		&tkCustomAttribute);

	LOG_IFFAILEDRET(hr, L"Failed in DefineCustomAttribute when applying SecuritySafeCritical to " <<
		L"new managed helper method " << wszName);

	return hr;
}

// [private] Launches the listener for file changes to ILRWP_watchercommands.log.


// [private] Wrapper method for the ICorProfilerCallback::RequestReJIT method, managing its errors.
HRESULT Cprofilermain::CallRequestReJIT(UINT cFunctionsToRejit, ModuleID * rgModuleIDs, mdMethodDef * rgMethodDefs)
{
	HRESULT hr = m_pICorProfilerInfo4->RequestReJIT(cFunctionsToRejit, rgModuleIDs, rgMethodDefs);

	LOG_IFFAILEDRET(hr, L"RequestReJIT failed");

	LOG_APPEND(L"RequestReJIT successfully called with " << cFunctionsToRejit << L" methods.");
	return hr;
}

// [private] Wrapper method for the ICorProfilerCallback::RequestRevert method, managing its errors.
HRESULT Cprofilermain::CallRequestRevert(UINT cFunctionsToRejit, ModuleID * rgModuleIDs, mdMethodDef * rgMethodDefs)
{
	HRESULT results[10];
	
	HRESULT hr = m_pICorProfilerInfo4->RequestRevert(cFunctionsToRejit, rgModuleIDs, rgMethodDefs, results);

	LOG_IFFAILEDRET(hr, L"RequestRevert failed");

	LOG_APPEND(L"RequestRevert successfully called with " << cFunctionsToRejit << L" methods.");
	return hr;
}


void Cprofilermain::LaunchLogListener(LPCWSTR wszPathCommandFile)
{
	// NO LOG LISTENER
}
// [private] Checks to see if the command file has any changes, and if so runs the new commands.
DWORD WINAPI MonitorFile(LPVOID args)
{
	// Monitor file until app shutdown.
	
	// No Monitor File

	return S_OK;
}

// [private] Checks to see if the given file exists.
bool FileExists(const PCWSTR wszFilepath)
{
	return true;
}

// [private] Reads and runs a command from the command file.
void ReadFile(FILE * fFile, LPVOID args)
{
	// Get a line.
	unsigned int refid = 0;
	WCHAR wszCommand[BUFSIZE], wszModule[BUFSIZE], wszClass[BUFSIZE], wszFunc[BUFSIZE];
	int nParsed = fwscanf_s(fFile,
		L"%u>\t%s\t%s\t%s%s\n",
		&refid,
		wszCommand, BUFSIZE,
		wszModule, BUFSIZE,
		wszClass, BUFSIZE,
		wszFunc, BUFSIZE);

	// Need all elements, or at least 0>quitcommand.
	if (nParsed == 5 || (nParsed > 1 && refid == 0))
	{

		if (refid == 0)
		{
			g_bShouldExit = (wcscmp(wszCommand, CMD_QUIT) == 0);

			if (!g_bShouldExit)
			{
				RESPONSE_ERROR(L"\"0>\t" << wszCommand << L"\" is not a valid command.");
			}
		}
		else if (refid > g_nLastRefid)
		{
			if ((wcscmp(wszCommand, CMD_REJITFUNC) == 0) ||
				(wcscmp(wszCommand, CMD_REVERTFUNC) == 0))
			{
				// Get the information necessary to rejit / revert, and then do it
				BOOL fRejit = (wcscmp(wszCommand, CMD_REJITFUNC) == 0);
				const int MAX_METHODS = 20;
				int cMethodsFound = 0;
				ModuleID moduleIDs[MAX_METHODS] = { 0 };
				mdMethodDef methodDefs[MAX_METHODS] = { 0 };
				if (::GetTokensFromNames(
					((threadargs *)args)->m_iMap,
					wszModule,
					wszClass,
					wszFunc,
					moduleIDs,
					methodDefs,
					_countof(moduleIDs),
					&cMethodsFound))
				{

					// This is a current command. Execute it.
					g_nLastRefid = refid;

					for (int i = 0; i < cMethodsFound; i++)
					{
						// Update this module's version in the mapping.
						MethodDefToLatestVersionMap * pMethodDefToLatestVersionMap =
							m_moduleIDToInfoMap.Lookup(moduleIDs[i]).m_pMethodDefToLatestVersionMap;
						pMethodDefToLatestVersionMap->Update(methodDefs[i], fRejit ? g_nLastRefid : 0);
					}

					HRESULT hr;
					if (fRejit)
					{
						hr = ((Cprofilermain *)((threadargs *)args)->m_pCallback)->
							CallRequestReJIT(
							cMethodsFound,          // Number of functions being rejitted
							moduleIDs,              // Pointer to the start of the ModuleID array
							methodDefs);            // Pointer to the start of the mdMethodDef array
					}
					else
					{
						hr = ((Cprofilermain *)((threadargs *)args)->m_pCallback)->
							CallRequestRevert(
							cMethodsFound,          // Number of functions being reverted
							moduleIDs,              // Pointer to the start of the ModuleID array
							methodDefs);            // Pointer to the start of the mdMethodDef array
					}

					if (FAILED(hr))
					{
						RESPONSE_IS(g_nLastRefid, RSP_REJITFAILURE, wszModule, wszClass, wszFunc);
					}
					else
					{
						RESPONSE_IS(g_nLastRefid, RSP_REJITSUCCESS, wszModule, wszClass, wszFunc);
					}

				}
				else
				{
					RESPONSE_ERROR(L"Module, class, or function not found. Maybe module is not loaded yet?");
					LOG_APPEND(L"ERROR: Module, class, or function not found. Maybe module is not loaded yet?");
				}
			}
			else
			{
				// We don't know how to deal with prof commands that aren't rejit / revert.
				RESPONSE_ERROR(L"\"" << refid << L">\t" << wszCommand << L"\" is not a valid command.");
			}
		}
	}
}

// [private] Gets the MethodDef from the module, class and function names.
BOOL GetTokensFromNames(IDToInfoMap<ModuleID, ModuleInfo> * mMap, LPCWSTR wszModule, LPCWSTR wszClass, LPCWSTR wszFunction, ModuleID * moduleIDs, mdMethodDef * methodDefs, int cElementsMax, int * pcMethodsFound)

{
	HRESULT hr;
	HCORENUM hEnum = NULL;
	ULONG cMethodDefsReturned = 0;
	mdTypeDef typeDef;
	mdMethodDef rgMethodDefs[2];
	*pcMethodsFound = 0;

	// Find all modules matching the name in this script entry.
	ModuleIDToInfoMap::LockHolder lockHolder(mMap);

	ModuleIDToInfoMap::Const_Iterator iterator;
	for (iterator = mMap->Begin(); (iterator != mMap->End()) && (*pcMethodsFound < cElementsMax); iterator++)
	{
		LPCWSTR wszModulePathCur = &(iterator->second.m_wszModulePath[0]);

		// Only matters if we have the right module name.
		if (::ContainsAtEnd(wszModulePathCur, wszModule))
		{
			hr = iterator->second.m_pImport->FindTypeDefByName(wszClass, mdTypeDefNil, &typeDef);

			if (FAILED(hr))
			{
				LOG_APPEND(L"Failed to find class '" << wszClass << L"',  hr = " << HEX(hr));
				continue;
			}

			hr = iterator->second.m_pImport->EnumMethodsWithName(
				&hEnum,
				typeDef,
				wszFunction,
				rgMethodDefs,
				_countof(rgMethodDefs),
				&cMethodDefsReturned);

			if (FAILED(hr) || (hr == S_FALSE))
			{
				LOG_APPEND(L"Found class '" << wszClass << L"', but no member methods with name '" <<
					wszFunction << L"', hr = " << HEX(hr));
				continue;
			}

			if (cMethodDefsReturned != 1)
			{
				LOG_APPEND(L"Expected exactly 1 methodDef to match class '" << wszClass << L"', method '" <<
					wszFunction << L"', but actually found '" << cMethodDefsReturned << L"'");
				continue;
			}

			// Remember the latest version number for this mdMethodDef.
			iterator->second.m_pMethodDefToLatestVersionMap->Update(rgMethodDefs[0], g_nLastRefid);

			// Save the matching pair.
			moduleIDs[*pcMethodsFound] = iterator->first;
			methodDefs[*pcMethodsFound] = rgMethodDefs[0];

			(*pcMethodsFound)++;

			// Intentionally continue through loop to find any other matching
			// modules. This catches the case where one module is loaded (unshared)
			// into multiple AppDomains
		}
	}

	// Return whether creation was successful.
	return (*pcMethodsFound) > 0;
}

// [private] Returns TRUE iff wszContainer ends with wszProspectiveEnding (case-insensitive).
BOOL ContainsAtEnd(LPCWSTR wszContainer, LPCWSTR wszProspectiveEnding)
{
	size_t cchContainer = wcslen(wszContainer);
	size_t cchEnding = wcslen(wszProspectiveEnding);

	if (cchContainer < cchEnding)
		return FALSE;

	if (cchEnding == 0)
		return FALSE;

	if (_wcsicmp(
		wszProspectiveEnding,
		&(wszContainer[cchContainer - cchEnding])) != 0)
	{
		return FALSE;
	}

	return TRUE;
}