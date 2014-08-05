// profilermain.h : Declaration of the Cprofilermain

#pragma once
#include "resource.h"       // main symbols
#include "CorProfilerCallbackImplementation.h"
#include "MetadataHelpers.h"

#include "clrprofiler_i.h"




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
	STDMETHOD(SetMask)();
	STDMETHOD(GetFullMethodName)(FunctionID functionID, std::string *methodName, int cMethod);
	STDMETHOD(GetFuncArgs)(FunctionID functionID, COR_PRF_FRAME_INFO frameinfo);

private:
	// container for ICorProfilerInfo reference
	CComQIPtr<ICorProfilerInfo> m_pICorProfilerInfo;
	// container for ICorProfilerInfo2 reference
	CComQIPtr<ICorProfilerInfo2> m_pICorProfilerInfo2;
	// container for ICorProfilerInfo3 reference
	CComQIPtr<ICorProfilerInfo3> m_pICorProfilerInfo3;
	// container for ICorProfilerInfo4 reference
	CComQIPtr<ICorProfilerInfo4> m_pICorProfilerInfo4;
};

OBJECT_ENTRY_AUTO(__uuidof(profilermain), Cprofilermain)
