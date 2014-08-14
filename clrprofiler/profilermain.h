// profilermain.h : Declaration of the Cprofilermain
#ifndef PROFILERMAIN_H
#define PROFILERMAIN_H

#pragma once
#include "resource.h"       // main symbols
#include <cor.h>
#include "CorProfilerCallbackImplementation.h"
#include "MetadataHelpers.h"
#include "commonstructures.h"
#include "ThreadStackItem.h"
#include "FunctionInfo.h"
#include "TimerItem.h"
#include "clrprofiler_i.h"

#include <cor.h>
#include <corprof.h>

#define MAINCSENTER EnterCriticalSection(&g_ThreadingCriticalSection)
#define MAINCSLEAVE LeaveCriticalSection(&g_ThreadingCriticalSection)
	


#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

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

	
	STDMETHOD(Initialize)(IUnknown *pICorProfilerInfoUnk);
	STDMETHOD(AppDomainCreationStarted)(AppDomainID appDomainId);
	STDMETHOD(ThreadCreated)(ThreadID threadId);
	STDMETHOD(ThreadDestroyed)(ThreadID threadId);
	STDMETHOD(ThreadNameChanged)(ThreadID threadId, ULONG cchName, _In_reads_opt_(cchName) WCHAR name[]);
	STDMETHOD(SetMask)();
	STDMETHOD(GetFullMethodName)(FunctionID functionID, std::string *methodName, int cMethod);
	STDMETHOD(GetFuncArgs)(FunctionID functionID, COR_PRF_FRAME_INFO frameinfo);
	STDMETHOD(ModuleLoadStarted)(ModuleID moduleId);
	STDMETHOD(ModuleLoadFinished)(ModuleID moduleId, HRESULT hrStatus);
	STDMETHOD(ClassLoadFinished)(ClassID classId, HRESULT hrStatus);

	STDMETHOD(GarbageCollectionStarted)(int cGenerations, BOOL generationCollected[], COR_PRF_GC_REASON reason);
	STDMETHOD(GarbageCollectionFinished)(void);

	STDMETHOD(RuntimeThreadSuspended)(ThreadID threadId);
	STDMETHOD(RuntimeThreadResumed)(ThreadID threadId);

	STDMETHOD(RuntimeSuspendStarted)(COR_PRF_SUSPEND_REASON suspendReason);
	STDMETHOD(RuntimeSuspendFinished)(void);
	STDMETHOD(RuntimeSuspendAborted)(void);
	STDMETHOD(RuntimeResumeStarted)(void);
	STDMETHOD(RuntimeResumeFinished)(void);

	void WriteLogFile();
	void AddCommonFunctions();

private:
	// container for ICorProfilerInfo reference
	CComQIPtr<ICorProfilerInfo> m_pICorProfilerInfo;
	// container for ICorProfilerInfo2 reference
	CComQIPtr<ICorProfilerInfo2> m_pICorProfilerInfo2;
	// container for ICorProfilerInfo3 reference
	CComQIPtr<ICorProfilerInfo3> m_pICorProfilerInfo3;
	// container for ICorProfilerInfo4 reference
	CComQIPtr<ICorProfilerInfo4> m_pICorProfilerInfo4;
	// container for IL allocation
	CComQIPtr<IMethodMalloc> m_pIMethodMalloc;
	//std::map<FunctionID, FunctionInfo> m_FunctionInfoMap;
	COR_PRF_SUSPEND_REASON m_CurrentSuspendReason;
	COR_PRF_GC_REASON m_CurrentGCReason;
	BOOL m_IsRuntimeSuspended;
	std::wstring m_ProcessName;
	DWORD m_ProcessId;
	
};

OBJECT_ENTRY_AUTO(__uuidof(profilermain), Cprofilermain)

#endif