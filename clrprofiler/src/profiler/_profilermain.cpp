
// profilermain.cpp : Implementation of Cprofilermain
#pragma once
#include "stdafx.h"
#include "profilermain.h"
#include "srw_helper.h"
#include "critsec_helper.h"
#include "../../metadatastaticlib/inc/allinfo.h"

// Maximum buffer size for input from file
#define BUFSIZE 2048

// Make sure the probe type matches the computer's architecture
#ifdef _WIN64
LPCWSTR k_wszEnteredFunctionProbeName = L"MgdEnteredFunction64";
LPCWSTR k_wszExitedFunctionProbeName = L"MgdExitedFunction64";
#else // Win32
LPCWSTR k_wszEnteredFunctionProbeName = L"MgdEnteredFunction32";
LPCWSTR k_wszExitedFunctionProbeName = L"MgdExitedFunction32";
#endif

#ifdef _WIN64
LPCWSTR k_wszEnteredFunctionProbeName2 = L"MgdEnteredFunction64_2";
LPCWSTR k_wszExitedFunctionProbeName2 = L"MgdExitedFunction64_2";
#else // Win32
LPCWSTR k_wszEnteredFunctionProbeName2 = L"MgdEnteredFunction32_2";
LPCWSTR k_wszExitedFunctionProbeName2 = L"MgdExitedFunction32_2";
#endif

// When pumping managed helpers into mscorlib, stick them into this pre-existing mscorlib type
LPCWSTR k_wszHelpersContainerType = L"System.CannotUnloadAppDomainException";



std::map<UINT_PTR, Cprofilermain*> * Cprofilermain::g_StaticContainerClass =
new std::map<UINT_PTR, Cprofilermain*>();

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
// Constructor and Destructor
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

Cprofilermain::Cprofilermain() :
	m_fInstrumentationHooksInSeparateAssembly(FALSE),
	m_mdIntPtrExplicitCast(mdTokenNil),
	m_mdEnterPInvoke(mdTokenNil),
	m_mdExitPInvoke(mdTokenNil),
	m_mdEnter(mdTokenNil),
	m_mdExit(mdTokenNil),
	m_modidMscorlib(NULL),
	m_refCount(0),
	m_dwShadowStackTlsIndex(0)
{

	if (!TryEnterCriticalSection(&Cprofilermain::g_StaticContainerClassCritSec))
	{
		InitializeCriticalSection(&Cprofilermain::g_StaticContainerClassCritSec);
	}
	else {
		LeaveCriticalSection(&Cprofilermain::g_StaticContainerClassCritSec);
	}
}

void Cprofilermain::SetUpAgent()
{
	InitializeCriticalSection(&this->m_Container->g_ThreadingCriticalSection);
	auto cmdProc = std::make_shared<CommandProcessor>(this);
	m_NetworkClient = new NetworkClient(this->m_ServerName, this->m_ServerPort, cmdProc);

	tp = new tp_helper(m_NetworkClient, 1, 1);
	tp->CreateNetworkIoThreadPool(m_NetworkClient);

	auto def = std::make_shared<Commands::DefineMethod>(0, 0, 0, 0, 0, L"");
	auto methodEnter = std::make_shared<Commands::MethodEnter>(0, 0, 0, 0);
	auto methodExit = std::make_shared<Commands::MethodExit>(0, 0, 0, 0);
	auto defineMetadata = std::make_shared<Commands::SendInjectionMetadata>(0, std::vector<char>(), std::vector<char>());


	m_NetworkClient->m_CommandList.emplace(def->Code(), def);
	m_NetworkClient->m_CommandList.emplace(methodEnter->Code(), methodEnter);
	m_NetworkClient->m_CommandList.emplace(methodExit->Code(), methodExit);
	m_NetworkClient->m_CommandList.emplace(defineMetadata->Code(), defineMetadata);

	m_NetworkClient->Start(); // Ready for normal unblocked operation
	SendAgentInformation();

	//TODO: Add code to log for timeouts and failures
	ResetEvent(ReceievedMetaDataForInjection);
	WaitForSingleObject(ReceievedMetaDataForInjection, 20000);

}

void Cprofilermain::SendAgentInformation()
{
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

	ResetEvent(&NetworkClient::DataReceived);

	m_NetworkClient->SendCommand(std::make_shared<Commands::AgentInformation>(
		0, m_AgentName, m_ComputerName, L"C:\\", L"Command Line", L"test.exe", 3000, L"4.0"
		));

	if (WaitForSingleObject(&NetworkClient::DataReceived, 5000) == 0)
	{
	}

}

Cprofilermain::~Cprofilermain()
{
	if (m_pICorProfilerInfo != NULL)
	{
		m_pICorProfilerInfo.Release();
	}
	if (m_pICorProfilerInfo2 != NULL)
	{
		m_pICorProfilerInfo2.Release();
	}
	if (m_pICorProfilerInfo3 != NULL)
	{
		m_pICorProfilerInfo3.Release();
	}
	if (m_pICorProfilerInfo4 != NULL)
	{
		m_pICorProfilerInfo4.Release();
	}
	// CRITICAL 1 Research this critical section in the profiler main destructor.
	DeleteCriticalSection(&this->m_Container->g_ThreadingCriticalSection);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// These are helper methods and have nothing to do with the core profiling.
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Cprofilermain::AddCommonFunctions()
{




}

void Cprofilermain::SetProcessName()
{
	WCHAR imageName[MAX_PATH]{ 0 };
	WCHAR currentPath[MAX_PATH]{ 0 };
	WCHAR computerName[MAX_COMPUTERNAME_LENGTH + 1]{ 0 };
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
		COR_PRF_MONITOR_THREADS
		//| COR_PRF_MONITOR_REMOTING_COOKIE 
		//| COR_PRF_MONITOR_REMOTING_ASYNC
		| COR_PRF_MONITOR_SUSPENDS
		| COR_PRF_MONITOR_EXCEPTIONS
		| COR_PRF_MONITOR_CLR_EXCEPTIONS
		//| COR_PRF_MONITOR_CLASS_LOADS
		| COR_PRF_MONITOR_MODULE_LOADS
		//| COR_PRF_MONITOR_ASSEMBLY_LOADS
		| COR_PRF_MONITOR_APPDOMAIN_LOADS
		//| COR_PRF_MONITOR_CODE_TRANSITIONS
		| COR_PRF_DISABLE_INLINING
		| COR_PRF_DISABLE_OPTIMIZATIONS
		| COR_PRF_ENABLE_REJIT
		| COR_PRF_DISABLE_ALL_NGEN_IMAGES
		| COR_PRF_MONITOR_JIT_COMPILATION);


	return m_pICorProfilerInfo->SetEventMask(eventMask);

}

STDMETHODIMP Cprofilermain::DoWeProfile()
{
	HKEY openKey;
	HRESULT result = RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\ChainsAPM\\Agents", NULL, KEY_READ, &openKey);
	if (result != S_OK)
	{
		//int lastErr = GetLastError();
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
	//unsigned long	PortType = 0;
	bool IsAMatch = false;
	int counter = 0;
	while (key == ERROR_SUCCESS && !IsAMatch)
	{

		currentKey.append(fullString);
		currentKey.append(lpStr);

		gotValue = RegGetValue(HKEY_LOCAL_MACHINE, currentKey.data(), L"Enabled", RRF_RT_ANY | RRF_ZEROONFAILURE, NULL, &Enabled, &BufferForDWORD);
		if ((gotValue != S_OK) | (Enabled == 0))
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
			if (BufferForStrings != NULL)
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
			if ((Port < 0) | (Port > 0xFFFF))
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


STDMETHODIMP Cprofilermain::AssemblyLoadFinished(AssemblyID assemblyId, HRESULT hrStatus)
{
	UNREFERENCED_PARAMETER(hrStatus);
	//LPCBYTE baseAddress = new byte;
	wchar_t *stringName = NULL;
	ULONG cNameSize = 0;
	ULONG pcchNameSize = 0;
	//AssemblyID asemId = {};
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



STDMETHODIMP Cprofilermain::ClassLoadFinished(ClassID classId, HRESULT hrStatus)
{
	UNREFERENCED_PARAMETER(hrStatus);
	ModuleID classModuleId;
	mdTypeDef classTypeDef;
	this->m_pICorProfilerInfo2->GetClassIDInfo(classId, &classModuleId, &classTypeDef);
	//g_MetadataHelpers->InjectFieldToModule(classModuleId, classTypeDef, std::wstring(L"test"));*/
	return S_OK;
}

STDMETHODIMP Cprofilermain::RuntimeThreadSuspended(ThreadID threadId)
{
	UNREFERENCED_PARAMETER(threadId);
	return S_OK;
}

STDMETHODIMP Cprofilermain::RuntimeThreadResumed(ThreadID threadId)
{
	UNREFERENCED_PARAMETER(threadId);
	return S_OK;
}

STDMETHODIMP Cprofilermain::GarbageCollectionStarted(int cGenerations, BOOL generationCollected[], COR_PRF_GC_REASON reason)
{
	UNREFERENCED_PARAMETER(cGenerations);
	UNREFERENCED_PARAMETER(generationCollected);
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




///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Class level static function hooks, cleaning up globals implementation
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Cprofilermain::FunctionEnterHook2(FunctionID funcId, UINT_PTR clientData,
	COR_PRF_FRAME_INFO func, COR_PRF_FUNCTION_ARGUMENT_INFO *argumentInfo)
{
	UNREFERENCED_PARAMETER(func);
	UNREFERENCED_PARAMETER(clientData);
	UNREFERENCED_PARAMETER(argumentInfo);


	/*
	MSDN Article that describes the ELT methods and what COR flags need to be set.
	http://msdn.microsoft.com/en-us/magazine/cc300553.aspx
	*/


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


	ThreadID threadId = 0;
	{
		//critsec_helper csh(&this->m_Container->g_MetaDataCriticalSection);
		this->m_pICorProfilerInfo->GetCurrentThreadID(&threadId);
		//csh.leave_early();
	}
	// Send no commands for now.
	// tp->SendEvent<Commands::MethodEnter>(new Commands::MethodEnter(funcId, threadId, timestamp));
}


void Cprofilermain::FunctionLeaveHook2(FunctionID funcId, UINT_PTR clientData,
	COR_PRF_FRAME_INFO func, COR_PRF_FUNCTION_ARGUMENT_RANGE *argumentRange)
{
	UNREFERENCED_PARAMETER(clientData);
	UNREFERENCED_PARAMETER(func);
	UNREFERENCED_PARAMETER(argumentRange);
	ThreadID threadId = 0;
	{
		//critsec_helper csh(&this->m_Container->g_MetaDataCriticalSection);
		this->m_pICorProfilerInfo->GetCurrentThreadID(&threadId);
		//csh.leave_early();
	}

	FILETIME HighPrecisionFileTime{ 0 };
	GetSystemTimeAsFileTime(&HighPrecisionFileTime);
	__int64 timestamp = (((__int64)HighPrecisionFileTime.dwHighDateTime) << 32) + HighPrecisionFileTime.dwLowDateTime;

	// Send no commands for now
	//tp->SendEvent<Commands::FunctionLeaveQuick>(new Commands::FunctionLeaveQuick(funcId, threadId, timestamp));

}

void Cprofilermain::FunctionTailHook2(FunctionID funcId, UINT_PTR clientData,
	COR_PRF_FRAME_INFO func)
{
	UNREFERENCED_PARAMETER(clientData);
	UNREFERENCED_PARAMETER(func);
	ThreadID threadId = 0;
	{
		//critsec_helper csh(&this->m_Container->g_MetaDataCriticalSection);
		this->m_pICorProfilerInfo->GetCurrentThreadID(&threadId);
		//csh.leave_early();
	}

	FILETIME HighPrecisionFileTime{ 0 };
	GetSystemTimeAsFileTime(&HighPrecisionFileTime);
	__int64 timestamp = (((__int64)HighPrecisionFileTime.dwHighDateTime) << 32) + HighPrecisionFileTime.dwLowDateTime;

	// Send no commands for now
	//tp->SendEvent<Commands::FunctionTailQuick>(new Commands::FunctionTailQuick(funcId, threadId, timestamp));
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
	//this->m_Container->g_MetadataHelpers->GetFunctionInformation(funcId, funcInfo);
	FILETIME HighPrecisionFileTime{ 0 };
	GetSystemTimeAsFileTime(&HighPrecisionFileTime);
	__int64 timestamp = (((__int64)HighPrecisionFileTime.dwHighDateTime) << 32) + HighPrecisionFileTime.dwLowDateTime;

	/*auto df = new Commands::DefineFunction(funcId, funcInfo->ClassInformation()->ClassId(), funcInfo->SignatureString(), timestamp);
	tp->SendEvent<Commands::DefineFunction>(df);*/
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


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Stolen Methods from IL Rewrite Profiler
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// [public] Creates the IL for the managed leave/enter helpers.
void Cprofilermain::SetILFunctionBodyForManagedHelper(ModuleID moduleID, mdMethodDef methodDef)
{
	///*assert(!m_fInstrumentationHooksInSeparateAssembly);
	//assert(moduleID == m_modidMscorlib);
	//assert((methodDef == m_mdEnter) || (methodDef == m_mdExit));*/

	//HRESULT hr = E_FAIL;
	//if ((moduleID == m_modidMscorlib) &&
	//	((methodDef == m_mdEnter) || (methodDef == m_mdExit)))
	//{
	//	hr = SetILForManagedHelper(
	//		m_pICorProfilerInfo,
	//		moduleID,
	//		methodDef,
	//		m_mdIntPtrExplicitCast,
	//		(methodDef == m_mdEnter) ? m_mdEnterPInvoke : m_mdExitPInvoke);
	//}
	//if (FAILED(hr))
	//{
	//	LOG_APPEND(L"SetILForManagedHelper failed for methodDef = " << HEX(methodDef) << L"--" <<
	//		((methodDef == m_mdEnter) ? L"enter" : L"exit") << L", hr = " << HEX(hr));
	//}
}

// [public] Creates the IL for the managed leave/enter helpers.
void Cprofilermain::SetILFunctionBodyForManagedHelper2(ModuleID moduleID, mdMethodDef methodDef)
{
	/*assert(!m_fInstrumentationHooksInSeparateAssembly);
	assert(moduleID == m_modidMscorlib);
	assert((methodDef == m_mdEnter2) || (methodDef == m_mdExit2));

	HRESULT hr = SetILForManagedHelper2(
		m_pICorProfilerInfo,
		moduleID,
		methodDef,
		m_mdIntPtrExplicitCast,
		(methodDef == m_mdEnter2) ? m_mdEnterPInvoke2 : m_mdExitPInvoke2);

	if (FAILED(hr))
	{
		LOG_APPEND(L"SetILForManagedHelper failed for methodDef = " << HEX(methodDef) << L"--" <<
			((methodDef == m_mdEnter) ? L"enter" : L"exit") << L", hr = " << HEX(hr));
	}*/
}

void Cprofilermain::SetILFunctionBodyForManagedHelperAdd(ModuleID moduleID, mdMethodDef methodDef)
{
	/*assert(!m_fInstrumentationHooksInSeparateAssembly);
	assert(moduleID == m_modidMscorlib);
	assert((methodDef == m_mdEnter2) || (methodDef == m_mdExit2));

	HRESULT hr = SetILForManagedHelperAddNumbers(
		m_pICorProfilerInfo,
		moduleID,
		methodDef,
		m_mdIntPtrExplicitCast,
		(methodDef == m_mdEnter2) ? m_mdEnterPInvoke2 : m_mdExitPInvoke2);

	if (FAILED(hr))
	{
		LOG_APPEND(L"SetILForManagedHelper failed for methodDef = " << HEX(methodDef) << L"--" <<
			((methodDef == m_mdEnter) ? L"enter" : L"exit") << L", hr = " << HEX(hr));
	}*/
}

// [private] Adds memberRefs to the managed helper into the module so that we can ReJIT later.
void Cprofilermain::AddMemberRefs(IMetaDataAssemblyImport * pAssemblyImport, IMetaDataAssemblyEmit * pAssemblyEmit, IMetaDataEmit * pEmit, ModuleInfo * pModuleInfo)
{
	assert(pModuleInfo != NULL);

	// TODO: Add Logging

	//IMetaDataImport * pImport = pModuleInfo->m_pImport;

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
		ELEMENT_TYPE_I4,                    // arg 3: int Param1
	};
#else //  ! _WIN64 (32-bit code follows)
	COR_SIGNATURE sigFunctionProbe[] = {
		IMAGE_CEE_CS_CALLCONV_DEFAULT,      // default calling convention
		0x03,                               // number of arguments == 3
		ELEMENT_TYPE_VOID,                  // return type == void
		ELEMENT_TYPE_U4,                    // arg 1: UInt32 moduleIDCur
		ELEMENT_TYPE_U4,                    // arg 2: UInt32 mdCur
		ELEMENT_TYPE_I4,                    // arg 3: int Param1
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
			// TODO: Add Logging
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
				// TODO: Add Logging
				return;
			}

			if (cAssemblyRefsReturned == 0)
			{
				// TODO: Add Logging
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
		// TODO: Add Logging
	}

	hr = pEmit->DefineMemberRef(
		typeRef,
		k_wszEnteredFunctionProbeName,
		sigFunctionProbe,
		sizeof(sigFunctionProbe),
		&(pModuleInfo->m_mdEnterProbeRef));

	if (FAILED(hr))
	{
		// TODO: Add Logging
	}

	hr = pEmit->DefineMemberRef(
		typeRef,
		k_wszExitedFunctionProbeName,
		sigFunctionProbe,
		sizeof(sigFunctionProbe),
		&(pModuleInfo->m_mdExitProbeRef));


	if (FAILED(hr))
	{
		// TODO: Add Logging
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
			// TODO: Add Logging
			return FALSE;
		}

		if (std::wstring(wszName).find(L"mscorlib") != std::wstring::npos)
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

	// TODO: Add Logging

	// The helpers will need to call into System.IntPtr::op_Explicit(int64), so get methodDef now
	mdTypeDef tdSystemIntPtr;
	hr = pImport->FindTypeDefByName(L"System.IntPtr", mdTypeDefNil, &tdSystemIntPtr);

	if (FAILED(hr))
	{
		// TODO: Add Logging
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
		// TODO: Add Logging
		return;
	}

	// Put the managed helpers into this pre-existing mscorlib type
	mdTypeDef tdHelpersContainer;
	hr = pImport->FindTypeDefByName(k_wszHelpersContainerType, mdTypeDefNil, &tdHelpersContainer);

	if (FAILED(hr))
	{
		// TODO: Add Logging
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
		// TODO: Add Logging
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
		// TODO: Add Logging
		return;
	}

	// Generate reference to unmanaged profiler DLL (i.e., us)
	mdModuleRef modrefNativeExtension;
	hr = pEmit->DefineModuleRef(L"clrprofiler", &modrefNativeExtension);

	if (FAILED(hr))
	{
		// TODO: Add Logging
		return;
	}

	// Generate the PInvokes into the profiler DLL

	AddPInvoke(
		pEmit,
		tdHelpersContainer,
		L"NtvEnteredFunction",
		modrefNativeExtension,
		&m_mdEnterPInvoke);

	assert(m_mdEnterPInvoke != mdTokenNil);

	AddPInvoke(
		pEmit,
		tdHelpersContainer,
		L"NtvExitedFunction",
		modrefNativeExtension,
		&m_mdExitPInvoke);

	assert(m_mdExitPInvoke != mdTokenNil);
	// Generate the SafeCritical managed methods which call the PInvokes
	mdMethodDef mdSafeCritical;
	GetSecuritySafeCriticalAttributeToken(pImport, &mdSafeCritical);


	AddManagedHelperMethod(
		pEmit,
		tdHelpersContainer,
		k_wszEnteredFunctionProbeName,
		m_mdEnterPInvoke,
		rvaCtor,
		mdSafeCritical,
		&m_mdEnter);

	assert(m_mdEnter != mdTokenNil);

	AddManagedHelperMethod(
		pEmit,
		tdHelpersContainer,
		k_wszExitedFunctionProbeName,
		m_mdExitPInvoke,
		rvaCtor,
		mdSafeCritical,
		&m_mdExit);

	assert(m_mdExit != mdTokenNil);
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
		ELEMENT_TYPE_U8,                 // ModuleID
		ELEMENT_TYPE_U4,                 // mdMethodDef token
		ELEMENT_TYPE_I4                 // Rejit version number
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

	// TODO: Add Logging

	hr = pEmit->DefinePinvokeMap(
		*pmdPInvoke,
		pmCallConvStdcall | pmNoMangle,
		wszName,
		modrefTarget);


	// TODO: Add Logging

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

	// TODO: Add Logging

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

	// TODO: Add Logging

	return hr;
}

// [private] Adds the managed helper methods to mscorlib.
HRESULT Cprofilermain::AddManagedHelperMethod(IMetaDataEmit * pEmit, mdTypeDef td, LPCWSTR wszName, mdMethodDef mdTargetPInvoke, ULONG rvaDummy, mdMethodDef mdSafeCritical, mdMethodDef * pmdHelperMethod)
{
	UNREFERENCED_PARAMETER(mdTargetPInvoke);
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

	// TODO: Add Logging

	mdToken tkCustomAttribute;
	hr = pEmit->DefineCustomAttribute(
		*pmdHelperMethod,
		mdSafeCritical,
		NULL,          //Blob, contains constructor params in this case none
		0,             //Size of the blob
		&tkCustomAttribute);

	// TODO: Add Logging

	return hr;
}

// [private] Gets the text names from a method def.
void Cprofilermain::GetClassAndFunctionNamesFromMethodDef(IMetaDataImport * pImport, ModuleID moduleID, mdMethodDef methodDef, LPWSTR wszTypeDefName, ULONG cchTypeDefName, LPWSTR wszMethodDefName, ULONG cchMethodDefName)
{
	UNREFERENCED_PARAMETER(moduleID);
	HRESULT hr;
	mdTypeDef typeDef;
	ULONG cchMethodDefActual;
	DWORD dwMethodAttr;
	ULONG cchTypeDefActual;
	DWORD dwTypeDefFlags;
	mdTypeDef typeDefBase;

	hr = pImport->GetMethodProps(
		methodDef,
		&typeDef,
		wszMethodDefName,
		cchMethodDefName,
		&cchMethodDefActual,
		&dwMethodAttr,
		NULL,       // [OUT] point to the blob value of meta data
		NULL,       // [OUT] actual size of signature blob
		NULL,       // [OUT] codeRVA
		NULL);      // [OUT] Impl. Flags

	if (FAILED(hr))
	{
		// TODO: Add Logging
	}

	hr = pImport->GetTypeDefProps(
		typeDef,
		wszTypeDefName,
		cchTypeDefName,
		&cchTypeDefActual,
		&dwTypeDefFlags,
		&typeDefBase);

	if (FAILED(hr))
	{
		// TODO: Add Logging
	}
}

// [private] Wrapper method for the ICorProfilerCallback::RequestReJIT method, managing its errors.
HRESULT Cprofilermain::CallRequestReJIT(UINT cFunctionsToRejit, ModuleID * rgModuleIDs, mdMethodDef * rgMethodDefs)
{
	HRESULT hr = m_pICorProfilerInfo4->RequestReJIT(cFunctionsToRejit, rgModuleIDs, rgMethodDefs);

	// TODO: Add Logging

	// TODO: Add Logging
	return hr;
}

// [private] Wrapper method for the ICorProfilerCallback::RequestRevert method, managing its errors.
HRESULT Cprofilermain::CallRequestRevert(UINT cFunctionsToRejit, ModuleID * rgModuleIDs, mdMethodDef * rgMethodDefs)
{
	HRESULT results[10];

	HRESULT hr = m_pICorProfilerInfo4->RequestRevert(cFunctionsToRejit, rgModuleIDs, rgMethodDefs, results);

	// TODO: Add Logging

	// TODO: Add Logging
	return hr;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Non-static callbacks into specific version of profiler
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// [private] Launches the listener for file changes to ILRWP_watchercommands.log.
// [public] Instrumented code eventually calls into here (when function is entered)
// to do the work of maintaining the shadow stack and function timings.
void Cprofilermain::NtvEnteredFunction(unsigned __int64 moduleIDCur, mdMethodDef mdCur, int nVersionCur)
{
	UNREFERENCED_PARAMETER(nVersionCur);
	ModuleInfo moduleInfo = m_moduleIDToInfoMap.Lookup(moduleIDCur);

	ThreadID threadId = 0;
	{
		critsec_helper csh(&this->m_Container->g_MetaDataCriticalSection);
		m_pICorProfilerInfo->GetCurrentThreadID(&threadId);
		csh.leave_early();
	}

	FILETIME HighPrecisionFileTime{ 0 };
	GetSystemTimeAsFileTime(&HighPrecisionFileTime);
	__int64 timestamp = (((__int64)HighPrecisionFileTime.dwHighDateTime) << 32) + HighPrecisionFileTime.dwLowDateTime;



	ModInfoFunctionMap mifm;
	mifm.m_ClassDef = mdCur;
	mifm.m_ModuleID = moduleIDCur;
	auto id = m_ModFuncMap.find(mifm);
	tp->SendEvent<Commands::MethodEnter>(new Commands::MethodEnter(timestamp, threadId, moduleIDCur, mdCur));
}

// [public] Instrumented code eventually calls into here (when function is exited)
// to do the work of maintaining the shadow stack and function timings.
void Cprofilermain::NtvExitedFunction(ModuleID moduleIDCur, mdMethodDef mdCur, int nVersionCur)
{
	UNREFERENCED_PARAMETER(nVersionCur);
	ModuleInfo moduleInfo = m_moduleIDToInfoMap.Lookup(moduleIDCur);
	ThreadID threadId = 0;
	{
		critsec_helper csh(&this->m_Container->g_MetaDataCriticalSection);
		m_pICorProfilerInfo->GetCurrentThreadID(&threadId);
		csh.leave_early();
	}

	FILETIME HighPrecisionFileTime{ 0 };
	GetSystemTimeAsFileTime(&HighPrecisionFileTime);
	__int64 timestamp = (((__int64)HighPrecisionFileTime.dwHighDateTime) << 32) + HighPrecisionFileTime.dwLowDateTime;

	//FunctionID funcId;

	ModInfoFunctionMap mifm;
	mifm.m_ClassDef = mdCur;
	mifm.m_ModuleID = moduleIDCur;
	auto id = m_ModFuncMap.find(mifm);

	tp->SendEvent<Commands::MethodExit>(new Commands::MethodExit(timestamp, threadId, moduleIDCur, mdCur));



}
