#include "stdafx.h"
#include "profilermain.h"

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
	// TODO: Add Logging
	WCHAR wszTypeDefName[512];
	WCHAR wszMethodDefName[512];

	ModuleInfo moduleInfo__ = m_moduleIDToInfoMap.Lookup(moduleID);

	int nVersion = 1;
	moduleInfo__.m_pMethodDefToLatestVersionMap->LookupIfExists(methodDef, &nVersion);

	/*GetClassAndFunctionNamesFromMethodDef(
	moduleInfo__.m_pImport,
	moduleID,
	methodDef,
	wszTypeDefName,
	_countof(wszTypeDefName),
	wszMethodDefName,
	_countof(wszMethodDefName));
	*/

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
		WCHAR* localString = L"WaitOne";
		if (this->m_Container->g_FullyQualifiedMethodsToProfile->find(im) != this->m_Container->g_FullyQualifiedMethodsToProfile->end())
		{
			/*hr = RewriteIL(
			m_pICorProfilerInfo,
			NULL,
			moduleID,
			methodDef,
			nVersion,
			moduleInfo.m_mdEnterProbeRef,
			moduleInfo.m_mdExitProbeRef);*/

			FILETIME HighPrecisionFileTime{ 0 };
			GetSystemTimeAsFileTime(&HighPrecisionFileTime);
			__int64 timestamp = (((__int64)HighPrecisionFileTime.dwHighDateTime) << 32) + HighPrecisionFileTime.dwLowDateTime;

			ModInfoFunctionMap mifm;
			mifm.m_ClassDef = methodDef;
			mifm.m_ModuleID = moduleID;
			m_ModFuncMap.emplace(mifm, functionID);

			auto defclass = new Commands::DefineClass(timestamp, classID, wszTypeDefName);
			tp->SendEvent<Commands::DefineClass>(defclass);

			auto defp = new Commands::DefineMethod(timestamp, moduleID, classID, methodDef, functionID, wszMethodDefName);
			tp->SendEvent<Commands::DefineMethod>(defp);



		}



	}

	// TODO: Add Logging




	return S_OK;
}