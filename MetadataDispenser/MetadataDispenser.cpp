// MetadataDispenser.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "MetadataDispenser.h"
#include <Psapi.h>

// This is an example of an exported function.
EXTERN_C METADATADISPENSER_API void __stdcall GetMetadataBytes(BSTR injectiondll, SAFEARRAY*& bytearray)
{
	CComPtr<IMetaDataDispenserEx> pMetaDispense;
	CComPtr<IMetaDataImport2> pMetaImport;
	CComPtr<IMetaDataAssemblyImport> pMetaAssemblyImport;
	CComPtr<IMetaDataEmit2> pMetaEmit;
	CComPtr<IMetaDataAssemblyEmit> pMetaAssemblyEmit;

	CoCreateInstance(
		CLSID_CorMetaDataDispenser,
		NULL,
		CLSCTX_INPROC,
		IID_IMetaDataDispenser,
		(LPVOID *)&pMetaDispense);
	//LPWSTR fileName = L"C:\\Windows\\Microsoft.NET\\Framework\\v4.0.30319\\mscorlib.dll";

	pMetaDispense->OpenScope(injectiondll, ofReadOnly, IID_IMetaDataImport2, (IUnknown**)&pMetaImport);
	pMetaDispense->OpenScope(injectiondll, ofReadOnly, IID_IMetaDataAssemblyImport, (IUnknown**)&pMetaAssemblyImport);
	pMetaDispense->OpenScope(injectiondll, ofRead, IID_IMetaDataEmit2, (IUnknown**)&pMetaEmit);
	pMetaDispense->OpenScope(injectiondll, ofRead, IID_IMetaDataAssemblyEmit, (IUnknown**)&pMetaAssemblyEmit);

	ULONG SaveSize = 0;
	pMetaEmit->GetSaveSize(cssAccurate, &SaveSize);
	void * saveData = malloc(SaveSize);
	pMetaEmit->SaveToMemory(saveData, SaveSize);
	SAFEARRAYBOUND bounds[1];
	bounds[0].cElements = SaveSize;
	bounds[0].lLbound = 0;
	bytearray = SafeArrayCreate(VT_UI1, 1, bounds);
	void * pDataArray;
	SafeArrayAccessData(bytearray, &pDataArray);
	memcpy(pDataArray, saveData, SaveSize);
}

EXTERN_C METADATADISPENSER_API void __stdcall GetILBytes(BSTR injectiondll, SAFEARRAY*& bytearray, int *RVAStart)
{
	HMODULE hmInjectiondll = LoadLibrary(injectiondll);
	MODULEINFO miInjection;
	GetModuleInformation(GetCurrentProcess(), hmInjectiondll, &miInjection, sizeof(MODULEINFO));

	auto dosHeader = (PIMAGE_DOS_HEADER)miInjection.lpBaseOfDll;
	auto ntHeader = (PIMAGE_NT_HEADERS)((UINT_PTR)miInjection.lpBaseOfDll + dosHeader->e_lfanew);
	auto imageHeader = (PIMAGE_FILE_HEADER)&ntHeader->FileHeader;
	auto peHeader = (PIMAGE_OPTIONAL_HEADER)&ntHeader->OptionalHeader;

	auto codeSection = (LPVOID)((UINT_PTR)miInjection.lpBaseOfDll + peHeader->BaseOfCode);
	
	*RVAStart = peHeader->BaseOfCode;
	
	SAFEARRAYBOUND bounds[1];
	bounds[0].cElements = peHeader->SizeOfCode;
	bounds[0].lLbound = 0;
	bytearray = SafeArrayCreate(VT_UI1, 1, bounds);
	void * pDataArray;
	SafeArrayAccessData(bytearray, &pDataArray);
	memcpy(pDataArray, codeSection, peHeader->SizeOfCode);
}

// This is the constructor of a class that has been exported.
// see MetadataDispenser.h for the class definition
CMetadataDispenser::CMetadataDispenser()
{
	
    return;
}
