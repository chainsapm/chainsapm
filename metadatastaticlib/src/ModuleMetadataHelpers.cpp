#include "stdafx.h"
#include "ModuleMetadataHelpers.h"


// CRITICAL 1 Research thread safety around metadata functions.

ModuleMetadataHelpers::ModuleMetadataHelpers(ATL::CComPtr<ICorProfilerInfo> profilerInfo, ModuleID moduleID) :
	m_pICorProfilerInfo(profilerInfo), m_moduleID(moduleID)
{
	InitializeCriticalSection(&m_ThreadCS);
	// Grab metadata interfaces 
	HRESULT hr = E_FAIL;
	{
		ATL::CComPtr<IUnknown> pUnk;

		hr = m_pICorProfilerInfo->GetModuleMetaData(moduleID, ofWrite, IID_IMetaDataEmit2, (IUnknown**)&pUnk);
		hr = pUnk->QueryInterface(IID_IMetaDataEmit2, (LPVOID *)&pMetaDataEmit);

		hr = m_pICorProfilerInfo->GetModuleMetaData(moduleID, ofRead, IID_IMetaDataImport2, (IUnknown**)&pUnk);
		hr = pUnk->QueryInterface(IID_IMetaDataImport2, (LPVOID *)&pMetaDataImport);

		hr = m_pICorProfilerInfo->GetModuleMetaData(moduleID, ofRead, IID_IMetaDataAssemblyImport, (IUnknown**)&pUnk);
		hr = pUnk->QueryInterface(IID_IMetaDataAssemblyImport, (LPVOID *)&pMetaDataAssemblyImport);

		hr = m_pICorProfilerInfo->GetModuleMetaData(moduleID, ofRead, IID_IMetaDataAssemblyEmit, (IUnknown**)&pUnk);
		hr = pUnk->QueryInterface(IID_IMetaDataAssemblyEmit, (LPVOID *)&pMetaDataAssemblyEmit);
	}

	AssemblyRefs = std::map<std::wstring, mdAssemblyRef>();

	
}

ModuleMetadataHelpers::~ModuleMetadataHelpers()
{
	DeleteCriticalSection(&m_ThreadCS);
	if (m_pICorProfilerInfo != NULL)
	{
		m_pICorProfilerInfo.Release();
	}
}

std::wstring ModuleMetadataHelpers::GetModuleName()
{
	LPCBYTE loadAddress = NULL;
	WCHAR moduleName[255];
	ULONG modNameLen = 0;
	AssemblyID assemblyID;
	m_pICorProfilerInfo->GetModuleInfo(m_moduleID,
		&loadAddress,
		_countof(moduleName),
		&modNameLen,
		moduleName,
		&assemblyID);
	return std::wstring(moduleName);
}

void ModuleMetadataHelpers::PopulateAssemblyRefs()
{
	HRESULT hr = E_FAIL;
	HCORENUM hEnumAssembly = NULL;
	mdAssemblyRef rgAssemblyRefs[1024]{ 0 };
	ULONG numberOfTokens;
	wchar_t assemblyRefNameBuffer[255];
	ULONG numChars = 0;
	DWORD attrFlags = 0;
	mdToken tkExtends = mdTokenNil;
	mdToken resolutionScope;
	char *publicKeyToken = NULL;
	char *hashVal = NULL;
	ULONG pktLen = 0;
	ULONG hashLen = 0;
	DWORD flags = 0;
	ASSEMBLYMETADATA amd{ 0 };

	// Enum Assembly Refs
	do
	{
		hr = pMetaDataAssemblyImport->EnumAssemblyRefs(
			&hEnumAssembly,
			rgAssemblyRefs,
			_countof(rgAssemblyRefs),
			&numberOfTokens);

		for (size_t i = 0; i < numberOfTokens; i++)
		{

			pMetaDataAssemblyImport->GetAssemblyRefProps(rgAssemblyRefs[i],
				(const void**)&publicKeyToken,
				&pktLen,
				assemblyRefNameBuffer,
				_countof(assemblyRefNameBuffer),
				&numChars,
				&amd,
				(const void**)&hashVal,
				&hashLen,
				&flags);
			AssemblyRefs.emplace(assemblyRefNameBuffer, rgAssemblyRefs[i]);
		}
	} while (hr == S_OK);

	pMetaDataAssemblyImport->CloseEnum(hEnumAssembly);
}

HRESULT ModuleMetadataHelpers::GetTypeDefOrRef(std::wstring ModuleName, std::wstring TypeName, mdToken & TypeRefOrDef)
{
	if (ModuleName == GetModuleName())
	{
		return pMetaDataImport->FindTypeDefByName(TypeName.c_str(), NULL, &TypeRefOrDef);;
	}
	else {
		auto match = AssemblyRefs.find(ModuleName);
		if (match != AssemblyRefs.end())
		{
			mdToken matchToken = match->second;
			return pMetaDataImport->FindTypeRef(matchToken, TypeName.c_str(), &TypeRefOrDef);
		}
	}
	return E_FAIL;
	
	//if (GetModuleName().find(L".exe") != std::wstring::npos)
	//{
	//	

	//	pMetaDataAssemblyImport->CloseEnum(hEnumAssembly);

	//	// Enum Module Refs
	//	do {
	//		hr = pMetaDataImport->EnumModuleRefs(
	//			&hEnumModule,
	//			rgModuleRefs,
	//			_countof(rgModuleRefs),
	//			&numberOfTokens);

	//	} while (hr == S_OK);

	//	pMetaDataImport->CloseEnum(hEnumModule);

	//	//Enum TypeRefs
	//	do {
	//		hr = pMetaDataImport->EnumTypeRefs(
	//			&hEnumTypeRefs,
	//			rgTypeRefs,
	//			_countof(rgTypeRefs),
	//			&numberOfTokens);

	//		for (size_t i = 0; i < numberOfTokens; i++)
	//		{
	//			pMetaDataImport->GetTypeRefProps(rgTypeRefs[i],
	//				&resolutionScope,
	//				typeDeffNameBuffer,
	//				255,
	//				&numChars);
	//			if ((resolutionScope & 0x1A000000) == 0x1A000000)
	//			{
	//				pMetaDataImport->GetModuleRefProps(resolutionScope,
	//					modRefNameBuffer,
	//					255,
	//					&numChars);
	//				auto s2 = std::wstring(typeDeffNameBuffer);
	//			}

	//			if ((resolutionScope & 0x23000000) == 0x23000000)
	//			{
	//				char publicKeyToken[1024];
	//				char hashVal[1024];
	//				ULONG pktLen = 0;
	//				ULONG hashLen = 0;
	//				DWORD flags = 0;
	//				ASSEMBLYMETADATA amd{ 0 };
	//				pMetaDataAssemblyImport->GetAssemblyRefProps(resolutionScope,
	//					(const void**)&publicKeyToken,
	//					&pktLen,
	//					modRefNameBuffer,
	//					255,
	//					&numChars,
	//					&amd,
	//					(const void**)&hashVal,
	//					&hashLen,
	//					&flags);

	//				auto s2 = std::wstring(typeDeffNameBuffer);
	//			}

	//		}

	//	} while (hr == S_OK);

	//	pMetaDataImport->CloseEnum(hEnumTypeRefs);

	//	// Enum Type Defs
	//	do
	//	{
	//		hr = pMetaDataImport->EnumTypeDefs(
	//			&hEnumTypeDefs,
	//			rgTypeDefs,
	//			_countof(rgTypeDefs),
	//			&numberOfTokens);

	//		for (size_t i = 0; i < numberOfTokens; i++)
	//		{
	//			pMetaDataImport->GetTypeDefProps(rgTypeDefs[i],
	//				typeDeffNameBuffer,
	//				255,
	//				&numChars,
	//				&attrFlags,
	//				&tkExtends);
	//			auto s = std::wstring(typeDeffNameBuffer);
	//		}

	//	} while (hr == S_OK);

	//	pMetaDataImport->CloseEnum(hEnumTypeDefs);
	//}
	//return E_NOTIMPL;
}
///* static public */
//PCCOR_SIGNATURE ModuleMetadataHelpers::ParseElementType(const ATL::CComPtr<IMetaDataImport>&  pMDImport,
//	PCCOR_SIGNATURE signature, std::wstring *buffer)
//{
//	switch (*signature++)
//	{
//	case ELEMENT_TYPE_VOID:
//		buffer->append(TEXT("void"));
//		break;
//
//
//	case ELEMENT_TYPE_BOOLEAN:
//		buffer->append(TEXT("bool"));
//		break;
//
//
//	case ELEMENT_TYPE_CHAR:
//		buffer->append(TEXT("wchar"));
//		break;
//
//
//	case ELEMENT_TYPE_I1:
//		buffer->append(TEXT("int8"));
//		break;
//
//
//	case ELEMENT_TYPE_U1:
//		buffer->append(TEXT("unsigned int8"));
//		break;
//
//
//	case ELEMENT_TYPE_I2:
//		buffer->append(TEXT("int16"));
//		break;
//
//
//	case ELEMENT_TYPE_U2:
//		buffer->append(TEXT("unsigned int16"));
//		break;
//
//
//	case ELEMENT_TYPE_I4:
//		buffer->append(TEXT("int32"));
//		break;
//
//
//	case ELEMENT_TYPE_U4:
//		buffer->append(TEXT("unsigned int32"));
//		break;
//
//
//	case ELEMENT_TYPE_I8:
//		buffer->append(TEXT("int64"));
//		break;
//
//
//	case ELEMENT_TYPE_U8:
//		buffer->append(TEXT("unsigned int64"));
//		break;
//
//
//	case ELEMENT_TYPE_R4:
//		buffer->append(TEXT("float32"));
//		break;
//
//
//	case ELEMENT_TYPE_R8:
//		buffer->append(TEXT("float64"));
//		break;
//
//
//	case ELEMENT_TYPE_U:
//		buffer->append(TEXT("unsigned int"));
//		break;
//
//
//	case ELEMENT_TYPE_I:
//		buffer->append(TEXT("int"));
//		break;
//
//
//	case ELEMENT_TYPE_OBJECT:
//		buffer->append(TEXT("Object"));
//		break;
//
//
//	case ELEMENT_TYPE_STRING:
//		buffer->append(TEXT("String"));
//		break;
//
//
//	case ELEMENT_TYPE_TYPEDBYREF:
//		buffer->append(TEXT("refany "));
//		break;
//
//	case ELEMENT_TYPE_CLASS:
//	case ELEMENT_TYPE_VALUETYPE:
//	case ELEMENT_TYPE_CMOD_REQD:
//	case ELEMENT_TYPE_CMOD_OPT:
//
//		mdToken	token;
//		signature += CorSigUncompressToken(signature, &token);
//		if (TypeFromToken(token) != mdtTypeRef)
//		{
//			HRESULT	hr;
//			WCHAR zName[MAX_LENGTH];
//			ULONG zLen;
//
//			hr = pMDImport->GetTypeDefProps(token,
//				zName,
//				MAX_LENGTH,
//				&zLen,
//				NULL,
//				NULL);
//			buffer->append(zName, zLen - 1);
//		}
//
//
//
//		break;
//
//
//	case ELEMENT_TYPE_SZARRAY:
//		signature = this->ParseElementType(pMDImport, signature, buffer);
//		buffer->append(TEXT("[]"));
//		break;
//
//
//	case ELEMENT_TYPE_ARRAY:
//	{
//		ULONG rank;
//
//
//		signature = this->ParseElementType(pMDImport, signature, buffer);
//		rank = CorSigUncompressData(signature);
//		if (rank == 0)
//			buffer->append(TEXT("[?]"));
//
//		else
//		{
//			ULONG *lower;
//			ULONG *sizes;
//			ULONG numsizes;
//			ULONG arraysize = (sizeof(ULONG) * 2 * rank);
//
//
//			lower = (ULONG *)_alloca(arraysize);
//			memset(lower, 0, arraysize);
//			sizes = &lower[rank];
//
//			numsizes = CorSigUncompressData(signature);
//			if (numsizes <= rank)
//			{
//				ULONG numlower;
//
//
//				for (ULONG i = 0; i < numsizes; i++)
//					sizes[i] = CorSigUncompressData(signature);
//
//
//				numlower = CorSigUncompressData(signature);
//				if (numlower <= rank)
//				{
//					for (ULONG i = 0; i < numlower; i++)
//						lower[i] = CorSigUncompressData(signature);
//
//
//					buffer->append(TEXT("["));
//					for (ULONG i = 0; i < rank; i++)
//					{
//						if ((sizes[i] != 0) && (lower[i] != 0))
//						{
//							if (lower[i] == 0)
//								wsprintf((LPWSTR)buffer->c_str(), TEXT("%d"), sizes[i]);
//
//							else
//							{
//								wsprintf((LPWSTR)buffer->c_str(), TEXT("%d"), lower[i]);
//								buffer->append(TEXT("..."));
//
//								if (sizes[i] != 0)
//									wsprintf((LPWSTR)buffer->c_str(), TEXT("%d"), (lower[i] + sizes[i] + 1));
//							}
//						}
//
//						if (i < (rank - 1))
//							buffer->append(TEXT(","));
//					}
//
//					buffer->append(TEXT("]"));
//				}
//			}
//		}
//	}
//	break;
//
//	case ELEMENT_TYPE_PINNED:
//		signature = this->ParseElementType(pMDImport, signature, buffer);
//		buffer->append(TEXT("pinned"));
//		break;
//
//
//	case ELEMENT_TYPE_PTR:
//		signature = this->ParseElementType(pMDImport, signature, buffer);
//		buffer->append(TEXT("*"));
//		break;
//
//
//	case ELEMENT_TYPE_BYREF:
//		signature = this->ParseElementType(pMDImport, signature, buffer);
//		buffer->append(TEXT("&"));
//		break;
//
//
//	default:
//	case ELEMENT_TYPE_END:
//	case ELEMENT_TYPE_SENTINEL:
//		buffer->append(TEXT("<UNKNOWN>"));
//		break;
//
//	} // switch	
//
//
//	return signature;
//
//} // BASEHELPER::ParseElementType
//
