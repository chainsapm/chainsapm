// profilermain.h : Declaration of the Cprofilermain
#ifndef PROFILERMAIN_H
#define PROFILERMAIN_H


EXTERN_C void FunctionEnter2_Wrapper_x64(FunctionID funcId, UINT_PTR clientData, COR_PRF_FRAME_INFO func, COR_PRF_FUNCTION_ARGUMENT_RANGE *argumentInfo);

EXTERN_C void FunctionLeave2_Wrapper_x64(FunctionID funcId, UINT_PTR clientData, COR_PRF_FRAME_INFO func, COR_PRF_FUNCTION_ARGUMENT_RANGE *argumentInfo);

EXTERN_C void FunctionTail2_Wrapper_x64(FunctionID funcId, UINT_PTR clientData, COR_PRF_FRAME_INFO func);

EXTERN_C void FunctionEnter2_CPP_STUB(FunctionID funcId, UINT_PTR clientData,
	COR_PRF_FRAME_INFO func, COR_PRF_FUNCTION_ARGUMENT_INFO *argumentInfo);

EXTERN_C void FunctionLeave2_CPP_STUB(FunctionID funcId, UINT_PTR clientData,
	COR_PRF_FRAME_INFO func, COR_PRF_FUNCTION_ARGUMENT_RANGE *argumentRange);

EXTERN_C void FunctionTail2_CPP_STUB(FunctionID funcId, UINT_PTR clientData,
	COR_PRF_FRAME_INFO func);

#pragma once
#include "resource.h"       // main symbols

#include "commonstructures.h"
#include "clrprofiler_i.h"
#include <cor.h>
#include <corprof.h>
#include "MetadataHelpers.h"
#include "FunctionInfo.h"
#include "stackitems.h"
#include "CorProfilerCallbackImplementation.h"
#include "Commands.h"
#include "networkclient.h"





struct ContainerClass;
class tp_helper;


#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif



extern std::wofstream g_wLogFile;

extern WCHAR g_wszCmdFilePath[];
extern WCHAR g_wszResponseFilePath[];
extern WCHAR g_wszLogFilePath[];
extern WCHAR g_wszResultFilePath[];

class CSHolder
{
public:
	CSHolder(CRITICAL_SECTION * pcs)
	{
		m_pcs = pcs;
		EnterCriticalSection(m_pcs);
	}

	~CSHolder()
	{
		assert(m_pcs != NULL);
		LeaveCriticalSection(m_pcs);
	}

private:
	CRITICAL_SECTION * m_pcs;
};

template <class _ID, class _Info>
class IDToInfoMap
{
public:
	typedef std::map<_ID, _Info> Map;
	typedef typename Map::iterator Iterator;
	typedef typename Map::const_iterator Const_Iterator;
	typedef typename Map::size_type Size_type;

	IDToInfoMap()
	{
		InitializeCriticalSection(&m_cs);
	}

	Size_type GetCount()
	{
		CSHolder csHolder(&m_cs);
		return m_map.size();
	}

	BOOL LookupIfExists(_ID id, _Info * pInfo)
	{
		CSHolder csHolder(&m_cs);
		Const_Iterator iterator = m_map.find(id);
		if (iterator == m_map.end())
		{
			return FALSE;
		}

		*pInfo = iterator->second;
		return TRUE;
	}

	_Info Lookup(_ID id)
	{
		CSHolder csHolder(&m_cs);
		_Info info;
		if (!LookupIfExists(id, &info))
		{
		}

		return info;
	}

	void Erase(_ID id)
	{
		CSHolder csHolder(&m_cs);
		Size_type cElementsRemoved = m_map.erase(id);
		if (cElementsRemoved != 1)
		{
			g_wLogFile.open(g_wszLogFilePath, std::ios::app);
			g_wLogFile << L"\nIDToInfoMap: " << cElementsRemoved <<
				L" elements removed, 1 expected.";
			g_wLogFile.close();
		}
	}

	void Update(_ID id, _Info info)
	{
		CSHolder csHolder(&m_cs);
		m_map[id] = info;
	}

	Const_Iterator Begin()
	{
		return m_map.begin();
	}

	Const_Iterator End()
	{
		return m_map.end();
	}

	class LockHolder
	{
	public:
		LockHolder(IDToInfoMap<_ID, _Info> * pIDToInfoMap) :
			m_csHolder(&(pIDToInfoMap->m_cs))
		{
		}

	private:
		CSHolder m_csHolder;
	};

private:
	Map m_map;
	CRITICAL_SECTION m_cs;
};

typedef IDToInfoMap<mdMethodDef, int> MethodDefToLatestVersionMap;

struct ModuleInfo
{
	WCHAR                               m_wszModulePath[512];
	IMetaDataImport *                   m_pImport;
	mdToken                             m_mdEnterProbeRef;
	mdToken                             m_mdExitProbeRef;
	MethodDefToLatestVersionMap *       m_pMethodDefToLatestVersionMap;
};

typedef IDToInfoMap<ModuleID, ModuleInfo> ModuleIDToInfoMap;

template <class MetaInterface>
class COMPtrHolder
{
public:
	COMPtrHolder()
	{
		m_ptr = NULL;
	}

	~COMPtrHolder()
	{
		if (m_ptr != NULL)
		{
			m_ptr->Release();
			m_ptr = NULL;
		}
	}

	MetaInterface* operator->()
	{
		return m_ptr;
	}

	MetaInterface** operator&()
	{
		return &m_ptr;
	}

	operator MetaInterface*()
	{
		return m_ptr;
	}

private:
	MetaInterface* m_ptr;
};

// A single entry in the single-thread shadow stack
struct ShadowStackFrameInfo
{
	ModuleID m_moduleID;
	mdMethodDef m_methodDef;
	int m_nVersion;
	ULONGLONG m_ui64TickCountOnEntry;
};

using namespace ATL;


// Cprofilermain

class ATL_NO_VTABLE Cprofilermain :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<Cprofilermain, &CLSID_profilermain>,
	public CorProfilerCallbackImplementation
{
public:

	Cprofilermain();
	~Cprofilermain();


	/*
	//MgdGetRequestBasics(
	class W3_MGD_HANDLER *,
	long *,
	long *,
	unsigned short const * *,
	unsigned long *,
	unsigned short const * *,
	unsigned long *,
	char const * *,
	struct _HTTP_COOKED_URL * *))
	*/
	//[DllImport("webengine4.dll", CharSet = CharSet.None, ExactSpelling = false)]
	//internal static extern int MgdGetRequestBasics(IntPtr pRequestContext, out int pContentType, 
	//out int pContentTotalLength, out IntPtr pPathTranslated, out int pcchPathTranslated, out IntPtr pCacheUrl, out int pcchCacheUrl, 
	//out IntPtr pHttpMethod, out IntPtr pCookedUrl);



	DECLARE_REGISTRY_RESOURCEID(IDR_PROFILERMAIN)


	BEGIN_COM_MAP(Cprofilermain)
		COM_INTERFACE_ENTRY(ICorProfilerCallback)
		COM_INTERFACE_ENTRY(ICorProfilerCallback2)
		COM_INTERFACE_ENTRY(ICorProfilerCallback3)
		COM_INTERFACE_ENTRY(ICorProfilerCallback4)
		COM_INTERFACE_ENTRY(ICorProfilerCallback5)
	END_COM_MAP()



	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

	// First method run when the CLR profiler spins up.
	STDMETHOD(Initialize)(IUnknown *pICorProfilerInfoUnk);

	STDMETHOD(AppDomainCreationStarted)(AppDomainID appDomainId);
	// Method that is run when we start a new .NET thread. This method will add our threads to the appropriate 
	STDMETHOD(ThreadCreated)(ThreadID threadId);
	STDMETHOD(ThreadDestroyed)(ThreadID threadId);
	STDMETHOD(ThreadNameChanged)(ThreadID threadId, ULONG cchName, _In_reads_opt_(cchName) WCHAR name[]);
	STDMETHOD(SetMask)();
	STDMETHOD(GetFullMethodName)(FunctionID functionID, std::wstring &methodName);
	STDMETHOD(GetFuncArgs)(FunctionID functionID, COR_PRF_FRAME_INFO frameinfo);
	STDMETHOD(ModuleLoadStarted)(ModuleID moduleId);
	STDMETHOD(ModuleLoadFinished)(ModuleID moduleId, HRESULT hrStatus);
	STDMETHOD(ClassLoadFinished)(ClassID classId, HRESULT hrStatus);
	STDMETHOD(AssemblyLoadFinished)(AssemblyID moduleId, HRESULT hrStatus);

	STDMETHOD(GarbageCollectionStarted)(int cGenerations, BOOL generationCollected[], COR_PRF_GC_REASON reason);
	STDMETHOD(GarbageCollectionFinished)(void);

	STDMETHOD(RuntimeThreadSuspended)(ThreadID threadId);
	STDMETHOD(RuntimeThreadResumed)(ThreadID threadId);

	STDMETHOD(RuntimeSuspendStarted)(COR_PRF_SUSPEND_REASON suspendReason);
	STDMETHOD(RuntimeSuspendFinished)(void);
	STDMETHOD(RuntimeSuspendAborted)(void);
	STDMETHOD(RuntimeResumeStarted)(void);
	STDMETHOD(RuntimeResumeFinished)(void);

	STDMETHOD(JITCompilationStarted)(FunctionID functionId, BOOL fIsSafeToBlock);

	UINT_PTR MapFunction(FunctionID funcId, UINT_PTR clientData, BOOL *pbHookFunction);

	STDMETHOD(DequeItems)(void);

	STDMETHOD(Shutdown)(void);

	STDMETHOD(DoWeProfile)(void);

	void WriteLogFile(int fileNum);
	void AddCommonFunctions();
	void SetProcessName();


	// Attach / Embed Events
	void AddMemberRefs(
		IMetaDataAssemblyImport * pAssemblyImport,
		IMetaDataAssemblyEmit * pAssemblyEmit,
		IMetaDataEmit * pEmit,
		ModuleInfo * pModuleInfo);

	HRESULT AddPInvoke(IMetaDataEmit * pEmit, mdTypeDef td, LPCWSTR wszName, mdModuleRef modrefTarget, mdMethodDef * pmdPInvoke);
	HRESULT GetSecuritySafeCriticalAttributeToken(IMetaDataImport * pImport, mdMethodDef * pmdSafeCritical);
	HRESULT AddManagedHelperMethod(IMetaDataEmit * pEmit, mdTypeDef td, LPCWSTR wszName, mdMethodDef mdTargetPInvoke, ULONG rvaDummy, mdMethodDef mdSafeCritical, mdMethodDef * pmdHelperMethod);
	void AddHelperMethodDefs(IMetaDataImport * pImport, IMetaDataEmit * pEmit);
	BOOL FindMscorlibReference(IMetaDataAssemblyImport * pAssemblyImport, mdAssemblyRef * rgAssemblyRefs, ULONG cAssemblyRefs, mdAssemblyRef * parMscorlib);

	// OUT-OF-THREAD REQUEST CALLS
	HRESULT CallRequestReJIT(UINT cFunctionsToRejit, ModuleID * rgModuleIDs, mdMethodDef * rgMethodIDs);
	HRESULT CallRequestRevert(UINT cFunctionsToRejit, ModuleID * rgModuleIDs, mdMethodDef * rgMethodIDs);

	// P-INVOKED FUNCTIONS
	void NtvEnteredFunction(ModuleID moduleIDCur, mdMethodDef mdCur, int nVersionCur);
	void NtvExitedFunction(ModuleID moduleIDCur, mdMethodDef mdCur, int nVersionCur);

	// Pipe operations with the GUI
	void LaunchLogListener(LPCWSTR wszPath);
	void GetClassAndFunctionNamesFromMethodDef(IMetaDataImport * pImport, ModuleID moduleID, mdMethodDef methodDef, LPWSTR wszTypeDefName, ULONG cchTypeDefName, LPWSTR wszMethodDefName, ULONG cchMethodDefName);


	// Enter call back for ICorProfilerCallback
	void FunctionEnterHook2(FunctionID funcId, UINT_PTR clientData,
		COR_PRF_FRAME_INFO func, COR_PRF_FUNCTION_ARGUMENT_INFO *argumentInfo);
	// Leave call back for ICorProfilerCallback
	void FunctionLeaveHook2(FunctionID funcId, UINT_PTR clientData,
		COR_PRF_FRAME_INFO func, COR_PRF_FUNCTION_ARGUMENT_RANGE *argumentRange);
	// Tail call back for ICorProfilerCallback
	void FunctionTailHook2(FunctionID funcId, UINT_PTR clientData,
		COR_PRF_FRAME_INFO func);

	// Enter call back for ICorProfilerCallback
	void FunctionEnterHook3(FunctionID funcId, UINT_PTR clientData,
		COR_PRF_FRAME_INFO func, COR_PRF_FUNCTION_ARGUMENT_INFO *argumentInfo);
	// Leave call back for ICorProfilerCallback
	void FunctionLeaveHook3(FunctionID funcId, UINT_PTR clientData,
		COR_PRF_FRAME_INFO func, COR_PRF_FUNCTION_ARGUMENT_RANGE *argumentRange);
	// Tail call back for ICorProfilerCallback
	void FunctionTailHook3(FunctionID funcId, UINT_PTR clientData,
		COR_PRF_FRAME_INFO func);


	// Mapper call back for ICorProfilerCallback
	static UINT_PTR __stdcall Mapper1(FunctionID funcId, BOOL *pbHookFunction);
	// Mapper call back for ICorProfilerCallback, calls Mapper1
	static UINT_PTR __stdcall Mapper2(FunctionID funcId, UINT_PTR clientData, BOOL *pbHookFunction);


	static std::map<UINT_PTR, Cprofilermain*> * g_StaticContainerClass;
	static CRITICAL_SECTION g_StaticContainerClassCritSec;

	NetworkClient *m_NetworkClient = NULL;

	/************************************************************************************
	!!!NOTE!!!!

	Starting with version 4.0 .NET now allows side-by-side loading of the CLR. In order
	to be a good profiling citizen we are going to support "profile many" from the
	following link:
	http://msdn.microsoft.com/en-us/library/ee461607(v=vs.110).aspx

	In order to do this we need to account for what possible versions of the CLR can be
	loaded and we need to make sure that we don't "cross the streams." So, in order
	to do that cleanly I will implement MULTIPLE structures.

	This will cause some application bloat but will allow for clean operation.
	************************************************************************************/

private:
	// Highest available API
	byte m_HighestProfileInfo;
	// container for ICorProfilerInfo reference
	std::shared_ptr<ICorProfilerInfo> m_pICorProfilerInfo;
	// container for ICorProfilerInfo2 reference
	std::shared_ptr<ICorProfilerInfo2> m_pICorProfilerInfo2;
	// container for ICorProfilerInfo3 reference
	std::shared_ptr<ICorProfilerInfo3> m_pICorProfilerInfo3;
	// container for ICorProfilerInfo4 reference
	std::shared_ptr<ICorProfilerInfo4> m_pICorProfilerInfo4;



	// container for IL allocation
	std::shared_ptr<IMethodMalloc> m_pIMethodMalloc;
	//std::map<FunctionID, FunctionInfo> m_FunctionInfoMap;
	COR_PRF_SUSPEND_REASON m_CurrentSuspendReason;
	COR_PRF_GC_REASON m_CurrentGCReason;
	BOOL m_IsRuntimeSuspended;
	DWORD m_ProcessId;

	std::wstring m_ProcessName;
	std::wstring m_ProcessDirectory;
	std::wstring m_ProcessCommandLine;
	std::wstring m_ComputerName;
	std::wstring m_FQDN;
	std::wstring m_ClusterName;

	std::wstring m_ServerName;
	std::wstring m_ServerPort;
	std::wstring m_AgentName;

	HMODULE m_webengineHandle;

	// This is the all encompasing container class used by this class
	ContainerClass * m_Container;

	//Thread Pool Helper
	tp_helper * tp = nullptr;


	/* If the instrumented code must call into managed helpers that we pump into mscorlib (as
	* opposed to calling into managed helpers statically compiled into ProfilerHelper.dll), then
	* the tokens are used to refer to the helpers as they will be in modified mscorlib metadata. */
	mdMethodDef m_mdIntPtrExplicitCast;
	mdMethodDef m_mdEnterPInvoke;
	mdMethodDef m_mdExitPInvoke;
	mdMethodDef m_mdEnter;
	mdMethodDef m_mdExit;

	volatile long m_refCount;
	ModuleID m_modidMscorlib;
	BOOL m_fInstrumentationHooksInSeparateAssembly;
	DWORD m_dwThresholdMs;
	DWORD m_dwShadowStackTlsIndex;


};

OBJECT_ENTRY_AUTO(__uuidof(profilermain), Cprofilermain)



class tp_helper
{

private:
	static Cprofilermain * m_cprof;

	PTP_POOL m_customThreadPool = nullptr;
	PTP_CALLBACK_ENVIRON m_ptpcbe = nullptr;
	PTP_CLEANUP_GROUP m_ptpcug = nullptr;

	template <class C>
	static VOID CALLBACK SendEventCallBack(PTP_CALLBACK_INSTANCE Instance, PVOID Parameter, PTP_WORK Work)
	{
		m_cprof->m_NetworkClient->SendCommand(std::make_shared<C>(*(C*)Parameter));
	}



public:

	tp_helper(Cprofilermain * profiler, int min, int max);
	~tp_helper();

	template <class C>
	void SendEvent(C *param)
	{
		auto tpw = CreateThreadpoolWork(&tp_helper::SendEventCallBack<C>, param, m_ptpcbe);
		SubmitThreadpoolWork(tpw);
	}

	void CreateNetworkIoThreadPool(NetworkClient* NetClient);
	





	
};



Cprofilermain * tp_helper::m_cprof = nullptr;

tp_helper::tp_helper(Cprofilermain * cpmain, int min, int max)
{
	tp_helper::m_cprof = cpmain;
	m_ptpcbe = new TP_CALLBACK_ENVIRON();
	InitializeThreadpoolEnvironment(m_ptpcbe);
	m_customThreadPool = CreateThreadpool(NULL);
	SetThreadpoolThreadMinimum(m_customThreadPool, min);
	SetThreadpoolThreadMaximum(m_customThreadPool, max);
	m_ptpcug = CreateThreadpoolCleanupGroup();
	SetThreadpoolCallbackPool(m_ptpcbe, m_customThreadPool);
	SetThreadpoolCallbackCleanupGroup(m_ptpcbe, m_ptpcug, NULL);
}


void tp_helper::CreateNetworkIoThreadPool(NetworkClient* NetClient)
{
	 NetClient->SetPTPIO(CreateThreadpoolIo(reinterpret_cast<HANDLE>(NetworkClient::m_SocketConnection),
			&NetworkClient::IoCompletionCallback, NetClient, nullptr));
}


tp_helper::~tp_helper()
{
}


#endif