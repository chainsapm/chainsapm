#include "stdafx.h"
#include "profilermain.h"

// [public] When a ReJIT starts, profilers don't typically need to do much in this
// method.  Here, we just do some light validation and logging.
STDMETHODIMP Cprofilermain::ReJITCompilationStarted(FunctionID functionID, ReJITID rejitId, BOOL fIsSafeToBlock)
{
	UNREFERENCED_PARAMETER(fIsSafeToBlock);
	UNREFERENCED_PARAMETER(rejitId);
	// TODO: Add Logging

	HRESULT hr;
	mdToken methodDef;
	ClassID classID;
	ModuleID moduleID;

	hr = m_pICorProfilerInfo->GetFunctionInfo(functionID, &classID, &moduleID, &methodDef);
	// TODO: Add Logging

	ModuleInfo moduleInfo = m_moduleIDToInfoMap.Lookup(moduleID);
	int nVersion = moduleInfo.m_pMethodDefToLatestVersionMap->Lookup(methodDef);
	if (nVersion == 0)
	{
		// TODO: Add Logging
		return E_FAIL;
	}

	// TODO: Add Logging


	return S_OK;
}

// [public] Logs any errors encountered during ReJIT.
STDMETHODIMP Cprofilermain::ReJITError(ModuleID moduleId, mdMethodDef methodId, FunctionID functionId, HRESULT hrStatus)
{
	UNREFERENCED_PARAMETER(functionId);
	UNREFERENCED_PARAMETER(methodId);
	UNREFERENCED_PARAMETER(moduleId);
	UNREFERENCED_PARAMETER(hrStatus);

	// TODO: Add Logging

	return S_OK;
}

// [public] Here's where the real work happens when a method gets ReJITed.  This is
// responsible for getting the new (instrumented) IL to be compiled.
STDMETHODIMP Cprofilermain::GetReJITParameters(ModuleID moduleId, mdMethodDef methodId, ICorProfilerFunctionControl *pFunctionControl)
{
	/*LOG_APPEND(L"ReJITScript::GetReJITParameters called, methodDef = " << HEX(methodId));

	ModuleInfo moduleInfo = m_moduleIDToInfoMap.Lookup(moduleId);
	HRESULT hr;

	int nVersion;
	moduleInfo.m_pMethodDefToLatestVersionMap->LookupIfExists(methodId, &nVersion);

	hr = RewriteIL(
	m_pICorProfilerInfo,
	pFunctionControl,
	moduleId,
	methodId,
	nVersion,
	moduleInfo.m_mdEnterProbeRef,
	moduleInfo.m_mdExitProbeRef);

	LOG_IFFAILEDRET(hr, L"RewriteIL failed for ModuleID = " << HEX(moduleId) <<
	L", methodDef = " << HEX(methodId));
	*/
	return S_OK;
}