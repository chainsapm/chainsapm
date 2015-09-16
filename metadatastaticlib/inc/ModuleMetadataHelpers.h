#pragma once
#include "stdafx.h"
#define MAX_LENGTH 2048

class ModuleMetadataHelpers
{
public:
	ModuleMetadataHelpers(ATL::CComPtr<ICorProfilerInfo> profilerInfo, ModuleID moduleID);
	~ModuleMetadataHelpers();

	std::wstring GetModuleName();
	HRESULT GetTypeDefOrRef(std::wstring ModuleName, std::wstring TypeName, mdToken &TypeRefOrDef);
	//PCCOR_SIGNATURE ParseElementType(const CComPtr<IMetaDataImport>& pMDImport, PCCOR_SIGNATURE signature, std::wstring* buffer);
	void PopulateAssemblyRefs();

private:
	ModuleID m_moduleID;
	std::map<std::wstring, mdAssemblyRef> AssemblyRefs;
	// container for ICorProfilerInfo reference
	ATL::CComPtr<ICorProfilerInfo> m_pICorProfilerInfo;
	ATL::CComPtr<IMetaDataEmit2> pMetaDataEmit;
	ATL::CComPtr<IMetaDataImport2> pMetaDataImport;
	ATL::CComPtr<IMetaDataAssemblyEmit> pMetaDataAssemblyEmit;
	ATL::CComPtr<IMetaDataAssemblyImport> pMetaDataAssemblyImport;
	//boost::mutex m_ThreadIdMutex;
	CRITICAL_SECTION m_ThreadCS;
	// Saving reference to the containers in order to provide specific function lookups, eg. Is this method an entry point

};

