// MetadataDispenser.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "MetadataDispenser.h"

// This is an example of an exported function.
EXTERN_C METADATADISPENSER_API void GetMetadataBytes(BSTR injectiondll, SAFEARRAY*& bytearray)
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

// This is the constructor of a class that has been exported.
// see MetadataDispenser.h for the class definition
CMetadataDispenser::CMetadataDispenser()
{
	
    return;
}
