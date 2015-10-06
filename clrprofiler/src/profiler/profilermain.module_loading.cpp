#include "stdafx.h"
#include "profilermain.h"

STDMETHODIMP Cprofilermain::ModuleLoadStarted(ModuleID moduleId)
{
	UNREFERENCED_PARAMETER(moduleId);
	return S_OK;

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

	//TOOD: Add logging

	//STEP 1 - Create a mapping for the ModuleMetadataHelper and this module.
	ModuleMetadataHelpers * helper = new ModuleMetadataHelpers(m_pICorProfilerInfo, moduleID);

	//STEP 2 - Merge in Metadata from injected methods


	if ((dwModuleFlags & COR_PRF_MODULE_WINDOWS_RUNTIME) != 0)
	{
		// Ignore any Windows Runtime modules.  We cannot obtain writeable metadata
		// interfaces on them or instrument their IL
		return S_OK;
	}

	BOOL fPumpHelperMethodsIntoThisModule = FALSE;
	if (std::wstring(wszName).find(L"mscorlib.dll") != std::wstring::npos)
	{
		m_modidMscorlib = moduleID;
		if (!m_fInstrumentationHooksInSeparateAssembly)
		{
			fPumpHelperMethodsIntoThisModule = TRUE;
		}
	}


	// Grab metadata interfaces 

	CComPtr<IMetaDataEmit> pEmit;
	{
		CComPtr<IUnknown> pUnk;

		hr = m_pICorProfilerInfo->GetModuleMetaData(moduleID, ofWrite, IID_IMetaDataEmit, &pUnk);
		// TODO: Add Logging

		hr = pUnk->QueryInterface(IID_IMetaDataEmit, (LPVOID *)&pEmit);
		// TODO: Add Logging
	}

	//std::unique_ptr<IMetaDataImport> pImport;
	CComPtr<IMetaDataImport> pImport;
	{
		CComPtr<IUnknown> pUnk;

		hr = m_pICorProfilerInfo->GetModuleMetaData(moduleID, ofRead, IID_IMetaDataImport, &pUnk);
		// TODO: Add Logging

		hr = pUnk->QueryInterface(IID_IMetaDataImport, (LPVOID *)&pImport);
		// TODO: Add Logging
	}


	CComPtr<IMetaDataAssemblyImport> pAssemblyImport;
	{
		CComPtr<IUnknown> pUnk;

		hr = m_pICorProfilerInfo->GetModuleMetaData(moduleID, ofRead, IID_IMetaDataAssemblyImport, &pUnk);
		// TODO: Add Logging

		hr = pUnk->QueryInterface(IID_IMetaDataAssemblyImport, (LPVOID *)&pAssemblyImport);
		// TODO: Add Logging

	}



	//pImport->EnumTypeDefs()

	HCORENUM hEnumAssembly = NULL;
	HCORENUM hEnumModule = NULL;
	HCORENUM hEnumTypeRefs = NULL;
	HCORENUM hEnumTypeDefs = NULL;
	mdTypeDef rgTypeDefs[1024]{ 0 };
	mdTypeRef rgTypeRefs[1024]{ 0 };
	mdModuleRef rgModuleRefs[1024]{ 0 };
	mdAssemblyRef rgAssemblyRefs[1024]{ 0 };
	ULONG numberOfTokens;

	wchar_t typeDeffNameBuffer[255];
	wchar_t modRefNameBuffer[255];
	wchar_t assemblyRefNameBuffer[255];

	ULONG numChars = 0;
	DWORD attrFlags = 0;
	mdToken tkExtends = mdTokenNil;
	mdToken resolutionScope;

	if (std::wstring(wszName).find(L".exe") != std::wstring::npos)
	{
		CComPtr<IMetaDataAssemblyImport> pAssemblyImportUP;
		{
			CComPtr<IUnknown> pUnk;

			hr = m_pICorProfilerInfo->GetModuleMetaData(moduleID, ofRead, IID_IMetaDataAssemblyImport, &pUnk);
			// TODO: Add Logging

			hr = pUnk->QueryInterface(IID_IMetaDataAssemblyImport, (LPVOID *)&pAssemblyImportUP);
			// TODO: Add Logging

		}

		mdTypeRef httpRefPtr = mdTokenNil;
		// Enum Assembly Refs
		do
		{
			hr = pAssemblyImportUP->EnumAssemblyRefs(
				&hEnumAssembly,
				rgAssemblyRefs,
				_countof(rgAssemblyRefs),
				&numberOfTokens);

			for (size_t i = 0; i < numberOfTokens; i++)
			{
				char *publicKeyToken = NULL;
				char *hashVal = NULL;
				ULONG pktLen = 0;
				ULONG hashLen = 0;
				DWORD flags = 0;
				ASSEMBLYMETADATA amd{ 0 };
				pAssemblyImportUP->GetAssemblyRefProps(rgAssemblyRefs[i],
					(const void**)&publicKeyToken,
					&pktLen,
					assemblyRefNameBuffer,
					_countof(assemblyRefNameBuffer),
					&numChars,
					&amd,
					(const void**)&hashVal,
					&hashLen,
					&flags);

				auto s2 = std::wstring(assemblyRefNameBuffer);
			}
		} while (hr == S_OK);

		pImport->CloseEnum(hEnumAssembly);

		// Enum Module Refs
		do {
			hr = pImport->EnumModuleRefs(
				&hEnumModule,
				rgModuleRefs,
				_countof(rgModuleRefs),
				&numberOfTokens);

		} while (hr == S_OK);

		pImport->CloseEnum(hEnumModule);

		//Enum TypeRefs
		do {
			hr = pImport->EnumTypeRefs(
				&hEnumTypeRefs,
				rgTypeRefs,
				_countof(rgTypeRefs),
				&numberOfTokens);

			for (size_t i = 0; i < numberOfTokens; i++)
			{
				pImport->GetTypeRefProps(rgTypeRefs[i],
					&resolutionScope,
					typeDeffNameBuffer,
					255,
					&numChars);
				if ((resolutionScope & 0x1A000000) == 0x1A000000)
				{
					pImport->GetModuleRefProps(resolutionScope,
						modRefNameBuffer,
						255,
						&numChars);
					auto s2 = std::wstring(typeDeffNameBuffer);
				}

				if ((resolutionScope & 0x23000000) == 0x23000000)
				{
					char publicKeyToken[1024];
					char hashVal[1024];
					ULONG pktLen = 0;
					ULONG hashLen = 0;
					DWORD flags = 0;
					ASSEMBLYMETADATA amd{ 0 };
					pAssemblyImport->GetAssemblyRefProps(resolutionScope,
						(const void**)&publicKeyToken,
						&pktLen,
						modRefNameBuffer,
						255,
						&numChars,
						&amd,
						(const void**)&hashVal,
						&hashLen,
						&flags);

					auto s2 = std::wstring(typeDeffNameBuffer);
				}

			}

		} while (hr == S_OK);

		pImport->CloseEnum(hEnumTypeRefs);

		// Enum Type Defs
		do
		{
			hr = pImport->EnumTypeDefs(
				&hEnumTypeDefs,
				rgTypeDefs,
				_countof(rgTypeDefs),
				&numberOfTokens);

			for (size_t i = 0; i < numberOfTokens; i++)
			{
				pImport->GetTypeDefProps(rgTypeDefs[i],
					typeDeffNameBuffer,
					255,
					&numChars,
					&attrFlags,
					&tkExtends);
				auto s = std::wstring(typeDeffNameBuffer);
			}

		} while (hr == S_OK);

		pImport->CloseEnum(hEnumTypeDefs);
	}

	if (fPumpHelperMethodsIntoThisModule)
	{
		AddHelperMethodDefs(pImport, pEmit);
	}

	// Store module info in our list

	// TODO: Add Logging

	ModuleInfo moduleInfo = { 0 };
	if (wcscpy_s(moduleInfo.m_wszModulePath, _countof(moduleInfo.m_wszModulePath), wszName) != 0)
	{
		// TODO: Add Logging
	}

	// Store metadata reader alongside the module in the list.
	moduleInfo.m_pImport = helper;

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

		CComPtr<IMetaDataAssemblyEmit> pAssemblyEmit;
		{
			CComPtr<IUnknown> pUnk;

			hr = m_pICorProfilerInfo->GetModuleMetaData(moduleID, ofWrite, IID_IMetaDataAssemblyEmit, &pUnk);
			// TODO: Add Logging

			hr = pUnk->QueryInterface(IID_IMetaDataAssemblyEmit, (LPVOID *)&pAssemblyEmit);
			// TODO: Add Logging
		}

		CComPtr<IMetaDataAssemblyImport> pAssemblyImport;
		{
			CComPtr<IUnknown> pUnk;

			hr = m_pICorProfilerInfo->GetModuleMetaData(moduleID, ofRead, IID_IMetaDataAssemblyImport, &pUnk);
			// TODO: Add Logging

			hr = pUnk->QueryInterface(IID_IMetaDataAssemblyImport, (LPVOID *)&pAssemblyImport);
			// TODO: Add Logging
		}

		AddMemberRefs(pAssemblyImport, pAssemblyEmit, pEmit, &moduleInfo);
	}

	// Append to the list!
	m_moduleIDToInfoMap.Update(moduleID, moduleInfo);
	// TODO: Add Logging

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
		// TODO: Add Logging
		CallRequestReJIT((UINT)rgMethodDefs.size(), rgModuleIDs.data(), rgMethodDefs.data());
	}

	return S_OK;
}

// Don't forget--modules can unload!  Remove it from our records when it does.
STDMETHODIMP Cprofilermain::ModuleUnloadStarted(ModuleID moduleID)
{
	// TODO: Add Logging

	ModuleIDToInfoMap::LockHolder lockHolder(&m_moduleIDToInfoMap);
	ModuleInfo moduleInfo;

	if (m_moduleIDToInfoMap.LookupIfExists(moduleID, &moduleInfo))
	{
		// TODO: Add Logging
		m_moduleIDToInfoMap.Erase(moduleID);
	}
	else
	{
		// TODO: Add Logging
	}

	return S_OK;
}
