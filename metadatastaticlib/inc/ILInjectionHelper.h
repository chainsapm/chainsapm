#pragma once

#pragma warning( disable : 4091 )
#include <corhlpr.cpp>
#include <cor.h>
#include <corprof.h>
#pragma warning( default : 4091 )

#include "ModuleMetadataHelpers.h"
#include "SignatureHelper.h"
class ILInjectionHelper
{
public:
	ILInjectionHelper();
	ILInjectionHelper(std::shared_ptr<ModuleMetadataHelpers> MetaDataHelpers, 
		CComPtr<ICorProfilerFunctionControl> pICorProfilerFunctionControl, 
		CComPtr<ICorProfilerInfo> pICorProfilerInfo);

	// Imports the IL bytes into our linked list
	HRESULT ImportIL(LPCBYTE pIL);
	// Imports the Exception Handler section from the IL bytes
	HRESULT ImportEH(const COR_ILMETHOD_SECT_EH* pILEH, unsigned nEH);
	// Write modified bytes to our allocated buffer
	HRESULT Export();
	// Get the raw bytes of the method we are going to create or rewrite, this is used when injecting a complete method
	HRESULT ImportAllBytes(LPCBYTE pIL);

	HRESULT AddEnterCall();
	HRESULT AddExitCall();
	HRESULT AddInfoCall();
private:

	HRESULT SetILFunctionBody(unsigned size, LPBYTE pBody);
	LPBYTE AllocateILMemory(unsigned size);
	void DeallocateILMemory(LPBYTE pBody);
	

	std::shared_ptr<SignatureHelper> SignatureHelpers; // Will be created when this class is instatiated
};