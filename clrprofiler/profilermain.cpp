// profilermain.cpp : Implementation of Cprofilermain
#pragma once
#include "stdafx.h"
#include "MetadataHelpers.h"
#include "commonstructures.h"
#include "ICorProlfileInfoCallbacks.hpp"
#include "profilermain.h"



Cprofilermain::Cprofilermain()
{
	g_FunctionSet = new std::map<FunctionID, FunctionInfo>();
	g_ThreadStackMap = new std::map<ThreadID, std::queue<ThreadStackItem>>();
	g_FunctionNameSet = new std::unordered_set<std::wstring>();
	this->AddCommonFunctions();
}


struct no_separator : std::numpunct<char> {
protected:
	virtual std::string do_grouping() const
	{
		return "\000";
	} // groups of 0 (disable)
};


Cprofilermain::~Cprofilermain()
{
	WriteLogFile();

	delete g_FunctionNameSet;
	delete g_FunctionSet;
	delete g_ThreadStackMap;
	delete g_MetadataHelpers;
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
	g_ProfilerCallback->Shutdown();
}


STDMETHODIMP Cprofilermain::Initialize(IUnknown *pICorProfilerInfoUnk)
{

	// set up our global access pointer
	g_ProfilerCallback = this;
	// get the ICorProfilerInfo interface
	HRESULT hr = pICorProfilerInfoUnk->QueryInterface(IID_ICorProfilerInfo, (LPVOID*)&this->m_pICorProfilerInfo);
	if (FAILED(hr))
		return E_FAIL;

	// determine if this object implements ICorProfilerInfo2
	hr = pICorProfilerInfoUnk->QueryInterface(IID_ICorProfilerInfo2, (LPVOID*)&this->m_pICorProfilerInfo2);
	if (FAILED(hr))
	{
		// we still want to work if this call fails, might be an older .NET version
		this->m_pICorProfilerInfo2.p = NULL;
	}

	// determine if this object implements ICorProfilerInfo3
	hr = pICorProfilerInfoUnk->QueryInterface(IID_ICorProfilerInfo3, (LPVOID*)&this->m_pICorProfilerInfo3);
	if (FAILED(hr))
	{
		// we still want to work if this call fails, might be an older .NET version
		this->m_pICorProfilerInfo3.p = NULL;
	}

	// determine if this object implements ICorProfilerInfo4
	hr = pICorProfilerInfoUnk->QueryInterface(IID_ICorProfilerInfo4, (LPVOID*)&this->m_pICorProfilerInfo4);
	if (FAILED(hr))
	{
		// we still want to work if this call fails, might be an older .NET version
		this->m_pICorProfilerInfo4.p = NULL;
	}

	if (this->m_pICorProfilerInfo4 != NULL)
	{
		g_MetadataHelpers = new MetadataHelpers(this->m_pICorProfilerInfo4);
	}
	else if (this->m_pICorProfilerInfo4 == NULL && this->m_pICorProfilerInfo3 != NULL)
	{
		g_MetadataHelpers = new MetadataHelpers(this->m_pICorProfilerInfo3);
	}
	else if (this->m_pICorProfilerInfo3 == NULL && this->m_pICorProfilerInfo2 != NULL)
	{
		g_MetadataHelpers = new MetadataHelpers(this->m_pICorProfilerInfo2);
	}
	else if (this->m_pICorProfilerInfo2 == NULL && this->m_pICorProfilerInfo != NULL)
	{
		g_MetadataHelpers = new MetadataHelpers(this->m_pICorProfilerInfo);
	}

	hr = pICorProfilerInfoUnk->QueryInterface(IID_IMethodMalloc, (LPVOID*)&this->m_pIMethodMalloc);
	if (FAILED(hr))
	{
		// we still want to work if this call fails, might be an older .NET version
		this->m_pICorProfilerInfo4.p = NULL;
	}


	INT clientData = 0xDEADBEEF;
	if (m_pICorProfilerInfo2 != NULL)
	{
		m_pICorProfilerInfo2->SetFunctionIDMapper((FunctionIDMapper*)&staticMethods::Mapper1);
#ifdef X64
		m_pICorProfilerInfo2->SetEnterLeaveFunctionHooks2((FunctionEnter2*)&FunctionEnter2_Wrapper_x64, (FunctionLeave2*)&FunctionLeave2_Wrapper_x64, (FunctionTailcall2*)&FunctionTail2_Wrapper_x64);
#else
		m_pICorProfilerInfo2->SetEnterLeaveFunctionHooks2();
#endif
	}
	if (m_pICorProfilerInfo3 != NULL)
	{
		INT clientData = 0xDEADBEEF;
		m_pICorProfilerInfo3->SetFunctionIDMapper2((FunctionIDMapper2*)&staticMethods::Mapper2, &clientData);
#ifdef X64
		m_pICorProfilerInfo3->SetEnterLeaveFunctionHooks3((FunctionEnter3*)&FunctionEnter2_Wrapper_x64, (FunctionLeave3*)&FunctionLeave2_Wrapper_x64, (FunctionTailcall3*)&FunctionTail2_Wrapper_x64);
#else
		m_pICorProfilerInfo3->SetEnterLeaveFunctionHooks3();
#endif
	}
	if (m_pICorProfilerInfo4 != NULL)
	{
		INT clientData = 0xDEADBEEF;
		m_pICorProfilerInfo4->SetFunctionIDMapper2((FunctionIDMapper2*)&staticMethods::Mapper2, &clientData);
#ifdef X64
		m_pICorProfilerInfo4->SetEnterLeaveFunctionHooks3((FunctionEnter3*)&FunctionEnter2_Wrapper_x64, (FunctionLeave3*)&FunctionLeave2_Wrapper_x64, (FunctionTailcall3*)&FunctionTail2_Wrapper_x64);
#else
		m_pICorProfilerInfo4->SetEnterLeaveFunctionHooks3();
#endif
	}





	SetMask();

	return S_OK;
}

STDMETHODIMP Cprofilermain::AppDomainCreationStarted(AppDomainID appDomainId)
{
	return S_OK;
}

STDMETHODIMP Cprofilermain::ThreadCreated(ThreadID threadId)
{
	g_ThreadStackMap->insert(std::pair<ThreadID, std::queue<ThreadStackItem>>(threadId, std::queue<ThreadStackItem>()));
	return S_OK;
}

STDMETHODIMP Cprofilermain::ThreadDestroyed(ThreadID threadId)
{
	return S_OK;
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
	DWORD eventMask = (DWORD)(
		COR_PRF_MONITOR_ASSEMBLY_LOADS
		| COR_PRF_MONITOR_APPDOMAIN_LOADS
		| COR_PRF_MONITOR_ENTERLEAVE
		| COR_PRF_ENABLE_FRAME_INFO
		| COR_PRF_ENABLE_FUNCTION_ARGS
		| COR_PRF_ENABLE_FUNCTION_RETVAL
		| COR_PRF_MONITOR_THREADS);
	return m_pICorProfilerInfo->SetEventMask(eventMask);
}

STDMETHODIMP Cprofilermain::GetFullMethodName(FunctionID functionID, std::string *methodName, int cMethod)
{
	IMetaDataImport* pIMetaDataImport = 0;
	HRESULT hr = S_OK;
	mdToken funcToken = 0;
	WCHAR szFunction[NAME_BUFFER_SIZE];
	WCHAR szClass[NAME_BUFFER_SIZE];
	WCHAR wszMethod[NAME_BUFFER_SIZE];

	// get the token for the function which we will use to get its name
	hr = this->m_pICorProfilerInfo2->GetTokenAndMetaDataFromFunction(functionID, IID_IMetaDataImport, (LPUNKNOWN *)&pIMetaDataImport, &funcToken);
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
				_snwprintf_s(wszMethod, cMethod, cMethod, L"%s.%s", szClass, szFunction);
				char newStr[NAME_BUFFER_SIZE];
				char defChar = ' ';
				WideCharToMultiByte(CP_ACP, 0, wszMethod, -1, newStr, NAME_BUFFER_SIZE, &defChar, NULL);
				*methodName = std::string(newStr);
			}
		}
		// release our reference to the metadata
		pIMetaDataImport->Release();
	}

	return hr;
}

STDMETHODIMP Cprofilermain::GetFuncArgs(FunctionID functionID, COR_PRF_FRAME_INFO frameinfo)
{
	HRESULT hr = S_OK;
	mdToken funcToken = 0;
	ClassID classID;
	ModuleID modId;
	mdToken token;
	ULONG32 typeArgsMax = 1024;
	ULONG32 typeArgsOut = 0;
	ClassID typeArgs[1024];

	// get the token for the function which we will use to get its name
	hr = this->m_pICorProfilerInfo2->GetFunctionInfo2(
		functionID,
		frameinfo,
		&classID,
		&modId,
		&token,
		typeArgsMax,
		&typeArgsOut,
		typeArgs);

	return hr;
}

void Cprofilermain::WriteLogFile()
{
	std::wofstream outFile("C:\\stackTrace.txt");
	if (outFile)
	{

		for (std::map<ThreadID, std::queue<ThreadStackItem>>::const_iterator it = g_ThreadStackMap->begin();
			it != g_ThreadStackMap->end(); it++)
		{
			int depth = 0;
			UINT_PTR highPart = 0xFFFFFFFF00000000 & it->first;
			UINT_PTR lowPart = 0x00000000FFFFFFFF & it->first;

			std::locale loc("");

			// imbue loc and add your own facet:
			outFile.imbue(std::locale(loc, new no_separator()));
			std::cout.imbue(std::locale(loc, new no_separator()));
			std::cout << "Thread: " << boost::format("0x%08x`%08x") % highPart % lowPart << std::endl;

			outFile << std::wstring(40, '=') << std::endl;
			outFile << "Thread: " << boost::wformat(TEXT("0x%08x`%08x")) % highPart % lowPart << std::endl;
			outFile << std::wstring(40, '=') << std::endl;
			outFile << std::endl;

			std::queue<ThreadStackItem> st = it->second;
			while (!st.empty())
			{
				ThreadStackItem tsi = st.front();
				std::map<FunctionID, FunctionInfo>::const_iterator itFunc
					= g_FunctionSet->find(tsi.ItemFunctionID());

				if (itFunc != g_FunctionSet->end())
				{
					/*switch (tsi.ItemStackReason())
					{
					case ThreadStackReason::ENTER:
						depth++;
						break;
					default:
						break;
					}*/

					if (depth < 0)
					{
						depth = 0;
					}
					FunctionInfo fi = itFunc->second;

					std::wstring spaces(depth, ' ');
					//outFile << spaces << fi.SignatureString() << tsi.ItemStackReason() << std::endl;
					outFile << spaces << fi.SignatureString()  << std::endl;
					outFile << boost::wformat(TEXT("Total time: %d\tProfiling Time: %d")) % tsi.ItemRunTime().total_microseconds() % tsi.ProfilingOverhead().total_microseconds() << std::endl;
					/*switch (tsi.ItemStackReason())
					{
					case ThreadStackReason::EXIT:
					depth--;
					break;
					default:
					break;
					}*/
				}
				st.pop();
			}
		}
	}
}

void Cprofilermain::AddCommonFunctions()
{
	g_FunctionNameSet->insert(TEXT("AddNumbers"));
	//g_FunctionNameSet->insert(TEXT("Main"));
	//g_FunctionNameSet->insert(TEXT("ThreadStart"));
	//g_FunctionNameSet->insert(TEXT("Start"));
	//g_FunctionNameSet->insert(TEXT(".ctor"));
	//g_FunctionNameSet->insert(TEXT(".cctor"));
}
