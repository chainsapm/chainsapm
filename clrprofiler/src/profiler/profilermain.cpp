
// profilermain.cpp : Implementation of Cprofilermain
#pragma once
#include "stdafx.h"
#include "ContainerClass.h"
#include "profilermain.h"
#include "srw_helper.h"
#include "critsec_helper.h"
#include "../../metadatastaticlib/inc/allinfo.h"



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

#ifdef _WIN64
LPCWSTR k_wszEnteredFunctionProbeName2 = L"MgdEnteredFunction64_2";
LPCWSTR k_wszExitedFunctionProbeName2 = L"MgdExitedFunction64_2";
#else // Win32
LPCWSTR k_wszEnteredFunctionProbeName2 = L"MgdEnteredFunction32_2";
LPCWSTR k_wszExitedFunctionProbeName2 = L"MgdExitedFunction32_2";
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

// [extern] ilrewriter function for rewriting a module's IL
extern HRESULT RewriteIL2(
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

// [extern] ilrewriter function for setting helper IL
extern HRESULT SetILForManagedHelper2(
	ICorProfilerInfo * pICorProfilerInfo,
	ModuleID moduleID,
	mdMethodDef mdHelperToAdd,
	mdMethodDef mdIntPtrExplicitCast,
	mdMethodDef mdPInvokeToCall);

// [extern] ilrewriter function for setting helper IL
extern HRESULT SetILForManagedHelperAddNumbers(
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




std::map<UINT_PTR, Cprofilermain*> * Cprofilermain::g_StaticContainerClass =
new std::map<UINT_PTR, Cprofilermain*>();

CRITICAL_SECTION Cprofilermain::g_StaticContainerClassCritSec;

//---------------------------------------------------------------------------------------
// Exports that managed code from ProfilerHelper.dll will P/Invoke into
// 
// NOTE: Must keep these signatures in sync with the DllImports in ProfilerHelper.cs!
//---------------------------------------------------------------------------------------

EXTERN_C void STDAPICALLTYPE NtvEnteredFunction(
	unsigned __int64 moduleIDCur,
	mdMethodDef mdCur,
	int nVersionCur)
{
	Cprofilermain * pContainerClass = nullptr;
	pContainerClass = Cprofilermain::g_StaticContainerClass->at(0x0);
	pContainerClass->NtvEnteredFunction(moduleIDCur, mdCur, nVersionCur);
}

EXTERN_C void STDAPICALLTYPE NtvExitedFunction(
	unsigned __int64 moduleIDCur,
	mdMethodDef mdCur,
	int nVersionCur)
{
	Cprofilermain * pContainerClass = nullptr;
	pContainerClass = Cprofilermain::g_StaticContainerClass->at(0x0);
	pContainerClass->NtvExitedFunction(moduleIDCur, mdCur, nVersionCur);
}

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
EXTERN_C void FunctionEnter2_CPP_STUB(FunctionID funcId, UINT_PTR clientData,
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
EXTERN_C void FunctionLeave2_CPP_STUB(FunctionID funcId, UINT_PTR clientData,
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
EXTERN_C void FunctionTail2_CPP_STUB(FunctionID funcId, UINT_PTR clientData,
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
#pragma warning( disable : 4102 )
#include "../src/asm/etl_enter_x86.inc"
#include "../src/asm/etl_tail_x86.inc"
#include "../src/asm/etl_leave_x86.inc"
#pragma warning( default : 4102 )
#endif



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
	m_NetworkClient = new NetworkClient(this->m_ServerName, this->m_ServerPort);

	tp = new tp_helper(this, 1, 1);
	tp->CreateNetworkIoThreadPool(m_NetworkClient);

	auto def = std::make_shared<Commands::DefineMethod>(0, 0, 0, 0, 0, L"");
	auto listOfCommands = std::make_shared<Commands::MethodsToInstrument>(0, std::vector<Commands::MethodsToInstrument::MethodProperties>(), std::vector<std::wstring>(), std::vector<std::wstring>());
	auto methodEnter = std::make_shared<Commands::MethodEnter>(0, 0, 0, 0);
	auto methodExit = std::make_shared<Commands::MethodExit>(0, 0, 0, 0);

	m_NetworkClient->m_CommandList.emplace(def->Code(), def);
	m_NetworkClient->m_CommandList.emplace(listOfCommands->Code(), listOfCommands);
	m_NetworkClient->m_CommandList.emplace(methodEnter->Code(), methodEnter);
	m_NetworkClient->m_CommandList.emplace(methodExit->Code(), methodExit);

	m_NetworkClient->Start(); // Ready for normal unblocked operation

	SendAgentInformation();

	AddCommonFunctions();



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

	ResetEvent(NetworkClient::DataReceived);

	m_NetworkClient->SendCommand<Commands::AgentInformation>(std::make_shared<Commands::AgentInformation>(
		0, m_AgentName, m_ComputerName, L"C:\\", L"Command Line", L"test.exe", 3000, L"4.0"
		));
	if (WaitForSingleObject(NetworkClient::DataReceived, 5000) == 0)
	{
	}

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

	auto cmd = std::dynamic_pointer_cast<Commands::MethodsToInstrument>(m_NetworkClient->ReceiveCommand());

	if (cmd->MethodList.size() == cmd->MethodPropList.size() && cmd->MethodList.size() == cmd->MethodClassList.size())
	{
		for (size_t i = 0; i < cmd->MethodList.size(); i++)
		{
			auto newMapping = ItemMapping();
			newMapping.FunctionName = cmd->MethodList[i];
			newMapping.ClassName = cmd->MethodClassList[i];
			newMapping.Match = ItemMapping::MatchType::FunctionOnly;
			this->m_Container->g_FullyQualifiedMethodsToProfile->insert(newMapping);
		}
	}
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
		| COR_PRF_ENABLE_REJIT
		| COR_PRF_DISABLE_ALL_NGEN_IMAGES
		| COR_PRF_MONITOR_JIT_COMPILATION); // I will turn this back on when the IL Rewrite is ready
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

STDMETHODIMP Cprofilermain::Initialize(IUnknown *pICorProfilerInfoUnk)
{
	// Get things like the process name, working directory, command line, etc.
	this->SetProcessName();

	this->m_Container = new ContainerClass();

	// We're using this as a quick exit so the profiler doesn't actually load.
	if (this->DoWeProfile() == S_OK)
	{
		//_CrtSetDbgFlag(_CRTDBG_CHECK_ALWAYS_DF);
		//_CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE | _CRTDBG_MODE_DEBUG);
		//_CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);
		SetUpAgent();

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


		UINT_PTR * clientData = new UINT_PTR(0xDEADBEEF); // We should never see this in our map. This is basically a bounds checker.

		if (m_pICorProfilerInfo2 != NULL)
		{
			this->m_Container->g_MetadataHelpers = new MetadataHelpers(m_pICorProfilerInfo2);
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
			this->m_Container->g_MetadataHelpers = new MetadataHelpers(m_pICorProfilerInfo2);
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
			this->m_Container->g_MetadataHelpers = new MetadataHelpers(m_pICorProfilerInfo2);
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
	UNREFERENCED_PARAMETER(appDomainId);
	return S_OK;
}

STDMETHODIMP Cprofilermain::ThreadCreated(ThreadID threadId)
{
	UNREFERENCED_PARAMETER(threadId);
	return S_OK;
}

STDMETHODIMP Cprofilermain::ThreadDestroyed(ThreadID threadId)
{
	UNREFERENCED_PARAMETER(threadId);
	return S_OK;
}

STDMETHODIMP Cprofilermain::ThreadNameChanged(ThreadID threadId, ULONG cchName, _In_reads_opt_(cchName) WCHAR name[])
{
	UNREFERENCED_PARAMETER(threadId);
	UNREFERENCED_PARAMETER(cchName);
	UNREFERENCED_PARAMETER(name);
	return S_OK;
}

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

STDMETHODIMP Cprofilermain::ModuleLoadStarted(ModuleID moduleId)
{
	UNREFERENCED_PARAMETER(moduleId);
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

STDMETHODIMP Cprofilermain::Shutdown(void)
{
	WaitForSingleObject(m_NetworkClient->DataSent, 5000); // Wait for data to be sent or 5 seconds
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
		this->m_Container->g_MetadataHelpers->GetCurrentThread(&threadId);
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
		this->m_Container->g_MetadataHelpers->GetCurrentThread(&threadId);
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
		this->m_Container->g_MetadataHelpers->GetCurrentThread(&threadId);
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
	this->m_Container->g_MetadataHelpers->GetFunctionInformation(funcId, funcInfo);
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
	assert(!m_fInstrumentationHooksInSeparateAssembly);
	assert(moduleID == m_modidMscorlib);
	assert((methodDef == m_mdEnter) || (methodDef == m_mdExit));

	HRESULT hr = SetILForManagedHelper(
		m_pICorProfilerInfo.get(),
		moduleID,
		methodDef,
		m_mdIntPtrExplicitCast,
		(methodDef == m_mdEnter) ? m_mdEnterPInvoke : m_mdExitPInvoke);

	if (FAILED(hr))
	{
		LOG_APPEND(L"SetILForManagedHelper failed for methodDef = " << HEX(methodDef) << L"--" <<
			((methodDef == m_mdEnter) ? L"enter" : L"exit") << L", hr = " << HEX(hr));
	}
}

// [public] Creates the IL for the managed leave/enter helpers.
void Cprofilermain::SetILFunctionBodyForManagedHelper2(ModuleID moduleID, mdMethodDef methodDef)
{
	assert(!m_fInstrumentationHooksInSeparateAssembly);
	assert(moduleID == m_modidMscorlib);
	assert((methodDef == m_mdEnter2) || (methodDef == m_mdExit2));

	HRESULT hr = SetILForManagedHelper2(
		m_pICorProfilerInfo.get(),
		moduleID,
		methodDef,
		m_mdIntPtrExplicitCast,
		(methodDef == m_mdEnter2) ? m_mdEnterPInvoke2 : m_mdExitPInvoke2);

	if (FAILED(hr))
	{
		LOG_APPEND(L"SetILForManagedHelper failed for methodDef = " << HEX(methodDef) << L"--" <<
			((methodDef == m_mdEnter) ? L"enter" : L"exit") << L", hr = " << HEX(hr));
	}
}
void Cprofilermain::SetILFunctionBodyForManagedHelperAdd(ModuleID moduleID, mdMethodDef methodDef)
{
	assert(!m_fInstrumentationHooksInSeparateAssembly);
	assert(moduleID == m_modidMscorlib);
	assert((methodDef == m_mdEnter2) || (methodDef == m_mdExit2));

	HRESULT hr = SetILForManagedHelperAddNumbers(
		m_pICorProfilerInfo.get(),
		moduleID,
		methodDef,
		m_mdIntPtrExplicitCast,
		(methodDef == m_mdEnter2) ? m_mdEnterPInvoke2 : m_mdExitPInvoke2);

	if (FAILED(hr))
	{
		LOG_APPEND(L"SetILForManagedHelper failed for methodDef = " << HEX(methodDef) << L"--" <<
			((methodDef == m_mdEnter) ? L"enter" : L"exit") << L", hr = " << HEX(hr));
	}
}


// [public] 
// A lot of work needs to happen when modules load.  Here, we
//      - add the module to the list of tracked modules for ReJIT
//      - add metadata refs to this module (in case we want to rewrite methods
//          in this module)
//      - add new methodDefs to this module if it's mscorlib.dll AND we're running
//          in the mode where we add probe implementations into mscorlib.dll rather
//          than using ProfilerHelper.dll
//      - create new ReJIT requests in case we're loading another copy of a module
//          (this time into a new unshared AppDomain), for which we'd previously
//          submitted a ReJIT request for the prior copy of the module
STDMETHODIMP Cprofilermain::ModuleLoadFinished(ModuleID moduleID, HRESULT hrStatus)
{
	UNREFERENCED_PARAMETER(hrStatus);
	LPCBYTE pbBaseLoadAddr;
	WCHAR wszName[300];
	ULONG cchNameIn = _countof(wszName);
	ULONG cchNameOut;
	AssemblyID assemblyID;
	DWORD dwModuleFlags;

	HRESULT hr = m_pICorProfilerInfo3->GetModuleInfo2(
		moduleID,
		&pbBaseLoadAddr,
		cchNameIn,
		&cchNameOut,
		wszName,
		&assemblyID,
		&dwModuleFlags);

	LOG_IFFAILEDRET(hr, L"GetModuleInfo2 failed for ModuleID = " << HEX(moduleID));

	if ((dwModuleFlags & COR_PRF_MODULE_WINDOWS_RUNTIME) != 0)
	{
		// Ignore any Windows Runtime modules.  We cannot obtain writeable metadata
		// interfaces on them or instrument their IL
		return S_OK;
	}

	AppDomainID appDomainID;
	ModuleID modIDDummy;
	hr = m_pICorProfilerInfo->GetAssemblyInfo(
		assemblyID,
		0,          // cchName,
		NULL,       // pcchName,
		NULL,       // szName[] ,
		&appDomainID,
		&modIDDummy);

	LOG_IFFAILEDRET(hr, L"GetAssemblyInfo failed for assemblyID = " << HEX(assemblyID));

	WCHAR wszAppDomainName[200];
	ULONG cchAppDomainName;
	ProcessID pProcID;
	//BOOL fShared = FALSE;

	hr = m_pICorProfilerInfo->GetAppDomainInfo(
		appDomainID,
		_countof(wszAppDomainName),
		&cchAppDomainName,
		wszAppDomainName,
		&pProcID);

	LOG_IFFAILEDRET(hr, L"GetAppDomainInfo failed for appDomainID = " << HEX(appDomainID));

	LOG_APPEND(L"ModuleLoadFinished for " << wszName << L", ModuleID = " << HEX(moduleID) <<
		L", LoadAddress = " << HEX(pbBaseLoadAddr) << L", AppDomainID = " << HEX(appDomainID) <<
		L", ADName = " << wszAppDomainName);

	BOOL fPumpHelperMethodsIntoThisModule = FALSE;
	if (::ContainsAtEnd(wszName, L"mscorlib.dll"))
	{
		m_modidMscorlib = moduleID;
		if (!m_fInstrumentationHooksInSeparateAssembly)
		{
			fPumpHelperMethodsIntoThisModule = TRUE;
		}
	}

	// Grab metadata interfaces 

	COMPtrHolder<IMetaDataEmit> pEmit;
	{
		COMPtrHolder<IUnknown> pUnk;

		hr = m_pICorProfilerInfo->GetModuleMetaData(moduleID, ofWrite, IID_IMetaDataEmit, &pUnk);
		LOG_IFFAILEDRET(hr, L"IID_IMetaDataEmit: GetModuleMetaData failed for ModuleID = " <<
			HEX(moduleID) << L" (" << wszName << L")");

		hr = pUnk->QueryInterface(IID_IMetaDataEmit, (LPVOID *)&pEmit);
		LOG_IFFAILEDRET(hr, L"IID_IMetaDataEmit: QueryInterface failed for ModuleID = " <<
			HEX(moduleID) << L" (" << wszName << L")");
	}

	COMPtrHolder<IMetaDataImport> pImport;
	{
		COMPtrHolder<IUnknown> pUnk;

		hr = m_pICorProfilerInfo->GetModuleMetaData(moduleID, ofRead, IID_IMetaDataImport, &pUnk);
		LOG_IFFAILEDRET(hr, L"IID_IMetaDataImport: GetModuleMetaData failed for ModuleID = " <<
			HEX(moduleID) << L" (" << wszName << L")");

		hr = pUnk->QueryInterface(IID_IMetaDataImport, (LPVOID *)&pImport);
		LOG_IFFAILEDRET(hr, L"IID_IMetaDataImport: QueryInterface failed for ModuleID = " <<
			HEX(moduleID) << L" (" << wszName << L")");
	}

	if (fPumpHelperMethodsIntoThisModule)
	{
		AddHelperMethodDefs(pImport, pEmit);
	}

	// Store module info in our list

	LOG_APPEND(L"Adding module to list...");

	ModuleInfo moduleInfo = { 0 };
	if (wcscpy_s(moduleInfo.m_wszModulePath, _countof(moduleInfo.m_wszModulePath), wszName) != 0)
	{
		LOG_IFFAILEDRET(E_FAIL, L"Failed to store module path '" << wszName << L"'");
	}

	// Store metadata reader alongside the module in the list.
	moduleInfo.m_pImport = pImport;
	moduleInfo.m_pImport->AddRef();

	moduleInfo.m_pMethodDefToLatestVersionMap = new MethodDefToLatestVersionMap();

	if (fPumpHelperMethodsIntoThisModule)
	{
		// We're operating on mscorlib and the helper methods are being pumped directly into it.
		// So we reference (from within mscorlib) the helpers via methodDefs, not memberRefs.

		assert(m_mdEnter != mdTokenNil);
		assert(m_mdExit != mdTokenNil);
		moduleInfo.m_mdEnterProbeRef = m_mdEnter;
		moduleInfo.m_mdExitProbeRef = m_mdExit;
	}
	else
	{
		// Add the references to our helper methods.

		COMPtrHolder<IMetaDataAssemblyEmit> pAssemblyEmit;
		{
			COMPtrHolder<IUnknown> pUnk;

			hr = m_pICorProfilerInfo->GetModuleMetaData(moduleID, ofWrite, IID_IMetaDataAssemblyEmit, &pUnk);
			LOG_IFFAILEDRET(hr, L"IID_IMetaDataEmit: GetModuleMetaData failed for ModuleID = " <<
				HEX(moduleID) << L" (" << wszName << L")");

			hr = pUnk->QueryInterface(IID_IMetaDataAssemblyEmit, (LPVOID *)&pAssemblyEmit);
			LOG_IFFAILEDRET(hr, L"IID_IMetaDataEmit: QueryInterface failed for ModuleID = " <<
				HEX(moduleID) << L" (" << wszName << L")");
		}

		COMPtrHolder<IMetaDataAssemblyImport> pAssemblyImport;
		{
			COMPtrHolder<IUnknown> pUnk;

			hr = m_pICorProfilerInfo->GetModuleMetaData(moduleID, ofRead, IID_IMetaDataAssemblyImport, &pUnk);
			LOG_IFFAILEDRET(hr, L"IID_IMetaDataImport: GetModuleMetaData failed for ModuleID = " <<
				HEX(moduleID) << L" (" << wszName << L")");

			hr = pUnk->QueryInterface(IID_IMetaDataAssemblyImport, (LPVOID *)&pAssemblyImport);
			LOG_IFFAILEDRET(hr, L"IID_IMetaDataImport: QueryInterface failed for ModuleID = " <<
				HEX(moduleID) << L" (" << wszName << L")");
		}

		AddMemberRefs(pAssemblyImport, pAssemblyEmit, pEmit, &moduleInfo);
	}

	// Append to the list!
	m_moduleIDToInfoMap.Update(moduleID, moduleInfo);
	LOG_APPEND(L"Successfully added module to list.");

	// If we already rejitted functions in other modules with a matching path, then
	// pre-rejit those functions in this module as well.  This takes care of the case
	// where we rejitted functions in a module loaded in one domain, and we just now
	// loaded the same module (unshared) into another domain.  We must explicitly ask to
	// rejit those functions in this domain's copy of the module, since it's identified
	// by a different ModuleID.

	std::vector<ModuleID> rgModuleIDs;
	std::vector<mdToken> rgMethodDefs;

	// Find all modules matching the name in this script entry
	{
		ModuleIDToInfoMap::LockHolder lockHolder(&m_moduleIDToInfoMap);

		// Get the methodDef map for the Module just loaded handy
		MethodDefToLatestVersionMap * pMethodDefToLatestVersionMap =
			m_moduleIDToInfoMap.Lookup(moduleID).m_pMethodDefToLatestVersionMap;
		assert(pMethodDefToLatestVersionMap != NULL);

		ModuleIDToInfoMap::Const_Iterator iterator;
		for (iterator = m_moduleIDToInfoMap.Begin();
		iterator != m_moduleIDToInfoMap.End();
			++iterator)
		{
			// Skip the entry we just added for this module
			if (iterator->first == moduleID)
			{
				continue;
			}

			const ModuleInfo * pModInfo = &(iterator->second);
			LPCWSTR wszModulePathCur = &(pModInfo->m_wszModulePath[0]);

			// We only care if the full path of the module from our internal
			// module list == full path of module just loaded
			if (_wcsicmp(wszModulePathCur, wszName) != 0)
			{
				continue;
			}

			// The module is a match!
			MethodDefToLatestVersionMap::Const_Iterator iterMethodDef;
			for (iterMethodDef = pModInfo->m_pMethodDefToLatestVersionMap->Begin();
			iterMethodDef != pModInfo->m_pMethodDefToLatestVersionMap->End();
				iterMethodDef++)
			{
				if (iterMethodDef->second == 0)
				{
					// We have reverted this method, do not pre-rejit.
					continue;
				}

				// NOTE: We may have already added this methodDef if it was rejitted in
				// multiple modules.  That means the array will have dupes.  It would be
				// wise to eliminate dupes before forcing the CLR to iterate over the
				// same methodDef multiple times (for performance reasons), but this is
				// just a sample.  Real profilers should be better than this.
				rgModuleIDs.push_back(moduleID);
				rgMethodDefs.push_back(iterMethodDef->first);

				// Remember the latest version number for this mdMethodDef
				pMethodDefToLatestVersionMap->Update(iterMethodDef->first, iterMethodDef->second);
			}
		}
	}

	if (rgMethodDefs.size() > 0)
	{
		LOG_APPEND(L"Auto-pre-rejitting " << rgMethodDefs.size() << L"  methods for modules that have just loaded into an AppDomain different from that containing a module from a prior ReJIT request.");
		CallRequestReJIT((UINT)rgMethodDefs.size(), rgModuleIDs.data(), rgMethodDefs.data());
	}

	return S_OK;
}

// Don't forget--modules can unload!  Remove it from our records when it does.
STDMETHODIMP Cprofilermain::ModuleUnloadStarted(ModuleID moduleID)
{
	LOG_APPEND(L"ModuleUnloadStarted: ModuleID = " << HEX(moduleID) << L".");

	ModuleIDToInfoMap::LockHolder lockHolder(&m_moduleIDToInfoMap);
	ModuleInfo moduleInfo;

	if (m_moduleIDToInfoMap.LookupIfExists(moduleID, &moduleInfo))
	{
		LOG_APPEND(L"Module found in list.  Removing...");
		m_moduleIDToInfoMap.Erase(moduleID);
	}
	else
	{
		LOG_APPEND(L"Module not found in list.  Do nothing.");
	}

	return S_OK;
}

// [public] Checks if the module is mscorlib, and if the CLR is trying to JIT a probe we
// dynamically added to mscorlib.  If so, this function provides the IL for the probe.
STDMETHODIMP Cprofilermain::JITCompilationStarted(FunctionID functionID, BOOL fIsSafeToBlock)
{
	UNREFERENCED_PARAMETER(fIsSafeToBlock);
	HRESULT hr;
	mdToken methodDef;
	ClassID classID;
	ModuleID moduleID;

	hr = m_pICorProfilerInfo->GetFunctionInfo(functionID, &classID, &moduleID, &methodDef);
	LOG_IFFAILEDRET(hr, L"GetFunctionInfo failed for FunctionID = " << HEX(functionID));
	WCHAR wszTypeDefName[512];
	WCHAR wszMethodDefName[512];

	ModuleInfo moduleInfo__ = m_moduleIDToInfoMap.Lookup(moduleID);

	int nVersion = 1;
	moduleInfo__.m_pMethodDefToLatestVersionMap->LookupIfExists(methodDef, &nVersion);

	GetClassAndFunctionNamesFromMethodDef(
		moduleInfo__.m_pImport,
		moduleID,
		methodDef,
		wszTypeDefName,
		_countof(wszTypeDefName),
		wszMethodDefName,
		_countof(wszMethodDefName));


	// printf("JITCompile: %S::%S\t\t\r\nmethodDef = %#8x | m_mdEnter2 = %#8x | m_mdExit2= %#8x\r\n", wszTypeDefName, wszMethodDefName, methodDef, m_mdEnter2, m_mdExit2);

	if ((moduleID == m_modidMscorlib) &&
		((methodDef == m_mdEnter) || (methodDef == m_mdExit)))
	{

		SetILFunctionBodyForManagedHelper(moduleID, methodDef);
	}

	else {
		ModuleInfo moduleInfo = m_moduleIDToInfoMap.Lookup(moduleID);

		moduleInfo.m_pMethodDefToLatestVersionMap->LookupIfExists(methodDef, &nVersion);

		auto im = ItemMapping();
		im.FunctionName = wszMethodDefName;
		im.ClassName = wszTypeDefName;
		this->m_Container->g_FullyQualifiedMethodsToProfile->find(im);

		if (this->m_Container->g_FullyQualifiedMethodsToProfile->find(im) != this->m_Container->g_FullyQualifiedMethodsToProfile->end())
		{
			hr = RewriteIL(
				m_pICorProfilerInfo.get(),
				NULL,
				moduleID,
				methodDef,
				nVersion,
				moduleInfo.m_mdEnterProbeRef,
				moduleInfo.m_mdExitProbeRef);

			FILETIME HighPrecisionFileTime{ 0 };
			GetSystemTimeAsFileTime(&HighPrecisionFileTime);
			__int64 timestamp = (((__int64)HighPrecisionFileTime.dwHighDateTime) << 32) + HighPrecisionFileTime.dwLowDateTime;

			ModInfoFunctionMap mifm;
			mifm.m_ClassDef = methodDef;
			mifm.m_ModuleID = moduleID;
			m_ModFuncMap.emplace(mifm, functionID);

			auto defp = new Commands::DefineMethod(timestamp, moduleID, classID, methodDef, functionID, wszMethodDefName);
			tp->SendEvent<Commands::DefineMethod>(defp);

		}

	}

	//LOG_APPEND(L"ReJITScript::GetReJITParameters called, methodDef = " << HEX(methodId));




	return S_OK;
}

// [public] When a ReJIT starts, profilers don't typically need to do much in this
// method.  Here, we just do some light validation and logging.
STDMETHODIMP Cprofilermain::ReJITCompilationStarted(FunctionID functionID, ReJITID rejitId, BOOL fIsSafeToBlock)
{
	UNREFERENCED_PARAMETER(fIsSafeToBlock);
	UNREFERENCED_PARAMETER(rejitId);
	LOG_APPEND(L"ReJITScript::ReJITCompilationStarted for FunctionID '" << HEX(functionID) <<
		L"' - RejitID '" << HEX(rejitId) << L"' called");

	HRESULT hr;
	mdToken methodDef;
	ClassID classID;
	ModuleID moduleID;

	hr = m_pICorProfilerInfo->GetFunctionInfo(functionID, &classID, &moduleID, &methodDef);
	LOG_IFFAILEDRET(hr, L"GetFunctionInfo failed for FunctionID =" << HEX(functionID));

	ModuleInfo moduleInfo = m_moduleIDToInfoMap.Lookup(moduleID);
	int nVersion = moduleInfo.m_pMethodDefToLatestVersionMap->Lookup(methodDef);
	if (nVersion == 0)
	{
		LOG_APPEND(L"ReJITCompilationStarted called for FunctionID = " << HEX(functionID) <<
			L", which should have been reverted.");
		return E_FAIL;
	}

	LOG_APPEND(L"Found latest version number of " << nVersion <<
		L" for rejitting function. Associating it with rejitID. (FunctionID = " << HEX(functionID) <<
		L", RejitID = " << HEX(rejitId) << L", mdMethodDef = " << HEX(methodDef) << L").");


	return S_OK;
}

// [public] Logs any errors encountered during ReJIT.
STDMETHODIMP Cprofilermain::ReJITError(ModuleID moduleId, mdMethodDef methodId, FunctionID functionId, HRESULT hrStatus)
{
	UNREFERENCED_PARAMETER(functionId);
	UNREFERENCED_PARAMETER(methodId);
	UNREFERENCED_PARAMETER(moduleId);
	UNREFERENCED_PARAMETER(hrStatus);

	LOG_IFFAILEDRET(hrStatus, L"ReJITError called.  ModuleID = " << HEX(moduleId) <<
		L", methodDef = " << HEX(methodId) << L", FunctionID = " << HEX(functionId));

	return S_OK;
}

// [public] Here's where the real work happens when a method gets ReJITed.  This is
// responsible for getting the new (instrumented) IL to be compiled.
STDMETHODIMP Cprofilermain::GetReJITParameters(ModuleID moduleId, mdMethodDef methodId, ICorProfilerFunctionControl *pFunctionControl)
{
	LOG_APPEND(L"ReJITScript::GetReJITParameters called, methodDef = " << HEX(methodId));

	ModuleInfo moduleInfo = m_moduleIDToInfoMap.Lookup(moduleId);
	HRESULT hr;

	int nVersion;
	moduleInfo.m_pMethodDefToLatestVersionMap->LookupIfExists(methodId, &nVersion);

	hr = RewriteIL(
		m_pICorProfilerInfo.get(),
		pFunctionControl,
		moduleId,
		methodId,
		nVersion,
		moduleInfo.m_mdEnterProbeRef,
		moduleInfo.m_mdExitProbeRef);

	LOG_IFFAILEDRET(hr, L"RewriteIL failed for ModuleID = " << HEX(moduleId) <<
		L", methodDef = " << HEX(methodId));

	return S_OK;
}

// [private] Adds memberRefs to the managed helper into the module so that we can ReJIT later.
void Cprofilermain::AddMemberRefs(IMetaDataAssemblyImport * pAssemblyImport, IMetaDataAssemblyEmit * pAssemblyEmit, IMetaDataEmit * pEmit, ModuleInfo * pModuleInfo)
{
	assert(pModuleInfo != NULL);

	LOG_APPEND(L"Adding memberRefs in this module to point to the helper managed methods");

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
	hr = pEmit->DefineModuleRef(L"clrprofiler", &modrefNativeExtension);

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
		LOG_APPEND(L"GetMethodProps failed in ModuleID = " <<
			HEX(moduleID) << L" for methodDef = " << HEX(methodDef) << L", hr = " << HEX(hr));
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
		LOG_APPEND(L"GetTypeDefProps failed in ModuleID = " << HEX(moduleID) <<
			L" for typeDef = " << HEX(typeDef) << L", hr = " << HEX(hr));
	}
}

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
		this->m_Container->g_MetadataHelpers->GetCurrentThread(&threadId);
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
		this->m_Container->g_MetadataHelpers->GetCurrentThread(&threadId);
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
	UNREFERENCED_PARAMETER(wszPathCommandFile);
	// NO LOG LISTENER
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