#include "stdafx.h"
#include "ModuleMetadataHelpers.h"


ModuleMetadataHelpers::ModuleMetadataHelpers(ATL::CComPtr<ICorProfilerInfo> profilerInfo, ModuleID moduleID) :
	pICorProfilerInfo(profilerInfo), ThisModuleID(moduleID)
{
	InitializeCriticalSection(&m_ThreadCS);
	// Grab metadata interfaces 
	HRESULT hr = E_FAIL;
	{
		ATL::CComPtr<IUnknown> pUnk;

		hr = pICorProfilerInfo->GetModuleMetaData(moduleID, ofWrite, IID_IMetaDataEmit2, (IUnknown**)&pUnk);
		hr = pUnk->QueryInterface(IID_IMetaDataEmit2, (LPVOID *)&pMetaDataEmit);

		hr = pICorProfilerInfo->GetModuleMetaData(moduleID, ofRead, IID_IMetaDataImport2, (IUnknown**)&pUnk);
		hr = pUnk->QueryInterface(IID_IMetaDataImport2, (LPVOID *)&pMetaDataImport);

		hr = pICorProfilerInfo->GetModuleMetaData(moduleID, ofRead, IID_IMetaDataAssemblyImport, (IUnknown**)&pUnk);
		hr = pUnk->QueryInterface(IID_IMetaDataAssemblyImport, (LPVOID *)&pMetaDataAssemblyImport);

		hr = pICorProfilerInfo->GetModuleMetaData(moduleID, ofRead, IID_IMetaDataAssemblyEmit, (IUnknown**)&pUnk);
		hr = pUnk->QueryInterface(IID_IMetaDataAssemblyEmit, (LPVOID *)&pMetaDataAssemblyEmit);

	}

	SetupClass();

}

ModuleMetadataHelpers::ModuleMetadataHelpers(ATL::CComPtr<IMetaDataImport2> MetaDataImport,
	ATL::CComPtr<IMetaDataEmit2> MetaDataEmit, ATL::CComPtr<IMetaDataAssemblyImport> AssemblyMetaDataImport,
	ATL::CComPtr<IMetaDataAssemblyEmit> AssemblyMetaDataEmit)
{
	MetaDataImport.CopyTo(&pMetaDataImport);
	MetaDataEmit.CopyTo(&pMetaDataEmit);
	AssemblyMetaDataImport.CopyTo(&pMetaDataAssemblyImport);
	AssemblyMetaDataEmit.CopyTo(&pMetaDataAssemblyEmit);

	SetupClass();
}

ModuleMetadataHelpers::~ModuleMetadataHelpers()
{
	if (pICorProfilerInfo != NULL)
		pICorProfilerInfo.Release();

	if (pMetaDataImport != NULL)
		pMetaDataImport.Release();

	if (pMetaDataEmit != NULL)
		pMetaDataEmit.Release();

	if (pMetaDataAssemblyImport != NULL)
		pMetaDataAssemblyImport.Release();

	if (pMetaDataAssemblyEmit != NULL)
		pMetaDataAssemblyEmit.Release();
}

void ModuleMetadataHelpers::SetupClass()
{
	AssemblyRefs = std::map<std::wstring, mdAssemblyRef>();

	GetModuleName();
	GetAssemblyName();
	PopulateAssemblyRefs();
	PopulateModuleRefs();
}

std::wstring ModuleMetadataHelpers::GetModuleName()
{
	if (ModuleName.empty())
	{
		WCHAR moduleName[255]{ 0 };
		ULONG modNameLen = 0;
		GUID modGUID{ 0 };
		pMetaDataImport->GetScopeProps(moduleName, _countof(moduleName), &modNameLen, &modGUID);
		ModuleName.assign(moduleName);
	}

	return ModuleName;
}

std::wstring ModuleMetadataHelpers::GetAssemblyName()
{
	if (AssemblyName.empty())
	{
		HRESULT hr = E_FAIL;
		wchar_t assemblyNameBuffer[255];
		ULONG numChars = 0;
		DWORD attrFlags = 0;
		char *publicKeyToken = NULL;
		char *hashVal = NULL;
		ULONG pktLen = 0;
		ULONG hashLen = 0;
		DWORD flags = 0;
		ASSEMBLYMETADATA amd{ 0 };
		mdAssembly mdAsemProp = mdAssemblyNil;
		pMetaDataAssemblyImport->GetAssemblyFromScope(&mdAsemProp);
		pMetaDataAssemblyImport->GetAssemblyProps(mdAsemProp, (const void**)&publicKeyToken, &pktLen,
			&hashLen, assemblyNameBuffer, _countof(assemblyNameBuffer), &numChars, &amd, &flags);
		AssemblyName.assign(assemblyNameBuffer);
	}

	return AssemblyName;
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
		if (numberOfTokens == 0)
			break;

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

void ModuleMetadataHelpers::PopulateModuleRefs()
{
	HRESULT hr = E_FAIL;
	HCORENUM hEnumModule = NULL;
	mdAssemblyRef rgModuleRefs[1024]{ 0 };
	ULONG numberOfTokens;
	wchar_t moduleRefNameBuffer[255];
	ULONG numChars = 0;

	// Enum Module Refs
	do
	{
		hr = pMetaDataImport->EnumModuleRefs(
			&hEnumModule,
			rgModuleRefs,
			_countof(rgModuleRefs),
			&numberOfTokens);

		if (numberOfTokens == 0)
			break;

		for (size_t i = 0; i < numberOfTokens; i++)
		{
			pMetaDataImport->GetModuleRefProps(rgModuleRefs[i],
				moduleRefNameBuffer,
				_countof(moduleRefNameBuffer),
				&numChars);
			ModuleRefs.emplace(moduleRefNameBuffer, rgModuleRefs[i]);
		}
	} while (hr == S_OK);

	pMetaDataAssemblyImport->CloseEnum(hEnumModule);
}

mdToken ModuleMetadataHelpers::GetMappedToken(mdToken token)
{
	auto checkToken = TokenMapping.find(token);
	if (checkToken != TokenMapping.end())
	{
		return checkToken->second;
	}
	return mdTokenNil;
}

mdToken ModuleMetadataHelpers::DefineSigToken(mdSignature original, PCCOR_SIGNATURE pNewSig, ULONG sigLen, mdSignature & replacement)
{
	pMetaDataEmit->GetTokenFromSig(pNewSig, sigLen, &replacement);
	TokenMapping[original] = replacement;
	return replacement;
}

HRESULT ModuleMetadataHelpers::DefineTokenReference(std::wstring ModuleOrAssemblyName, std::wstring TypeName,
	std::wstring MemberName, PCCOR_SIGNATURE MethodSignature, ULONG SigLength, mdToken mdParentToken,
	mdToken tokenIn, ULONG methodAttributes, std::wstring PInvokeModuleName)
{
	mdToken tokenOut = mdTokenNil;
	HRESULT hr = E_FAIL;
	if (MemberName.empty())
	{
		if (ModuleOrAssemblyName.empty() && TypeName.empty())
		{
			pMetaDataEmit->GetTokenFromTypeSpec(MethodSignature, SigLength, &tokenOut);
		}
		else {
			hr = FindTypeDefOrRef(ModuleOrAssemblyName, TypeName, tokenOut);
			if (hr == CLDB_E_RECORD_NOTFOUND) {
				hr = AddTypeDefOrRef(TypeName, tokenOut, ModuleOrAssemblyName);
			}
		}


	}
	else {
		if (MemberName == L"METHODSPECPLACEHOLDER")
		{
			mdParentToken = GetMappedToken(mdParentToken);
			pMetaDataEmit->DefineMethodSpec(mdParentToken, MethodSignature, SigLength, &tokenOut);
		}
		else if (ModuleOrAssemblyName.empty() && TypeName.empty())
		{
			mdParentToken = GetMappedToken(mdParentToken);

			HRESULT memberRefHR;
			HCORENUM hEnumMemberRefs = NULL;
			mdMemberRef rgMemberRefs[8192]{ 0 };
			ULONG numberOfMemberRefTokens;
			mdMemberRef memberRefToken;
			PCCOR_SIGNATURE sigBlob = NULL;
			ULONG sigSize = 0;
			ULONG numChars = 0;

			wchar_t memberRefNameBuffer[255];

			do {
				memberRefHR = pMetaDataImport->EnumMemberRefs(
					&hEnumMemberRefs,
					mdParentToken,
					rgMemberRefs,
					_countof(rgMemberRefs),
					&numberOfMemberRefTokens);

				for (size_t i = 0; i < numberOfMemberRefTokens; i++)
				{
					pMetaDataImport->GetMemberRefProps(rgMemberRefs[i],
						&memberRefToken,
						memberRefNameBuffer,
						_countof(memberRefNameBuffer),
						&numChars,
						&sigBlob,
						&sigSize);

					if (StrCmp(memberRefNameBuffer, MemberName.c_str()) == 0 && memcmp(sigBlob, MethodSignature, SigLength) == 0)
					{
						TokenMapping[tokenIn] = rgMemberRefs[i];
						pMetaDataImport->CloseEnum(hEnumMemberRefs);
						return S_OK;
					}
				}
			} while (memberRefHR == S_OK);
			pMetaDataImport->CloseEnum(hEnumMemberRefs);
			hEnumMemberRefs = NULL;
			// If after all of that we can't find a proper signature, let's create one
			pMetaDataEmit->DefineMemberRef(mdParentToken, MemberName.c_str(), MethodSignature, SigLength, &tokenOut);

		}
		else {
			hr = FindMemberDefOrRef(ModuleOrAssemblyName, TypeName, MemberName, MethodSignature, SigLength, tokenOut);
			if (hr == CLDB_E_RECORD_NOTFOUND) {
				hr = AddMemberRefOrDef(TypeName, MemberName, MethodSignature, SigLength, tokenOut, ModuleOrAssemblyName, methodAttributes, PInvokeModuleName);
			}
		}

	}
	TokenMapping[tokenIn] = tokenOut;
	return hr;
}

HRESULT ModuleMetadataHelpers::AddMemberRefOrDef(std::wstring &TypeName, std::wstring &MemberName, const PCCOR_SIGNATURE &MethodSignature, ULONG SigLength, mdToken &tokenOut, std::wstring &ModuleOrAssemblyName, ULONG methodAttributes, std::wstring PInvokeModuleName)
{
	if (ModuleOrAssemblyName == GetModuleName() | ModuleOrAssemblyName == GetAssemblyName())
		return AddMethodDef(TypeName, MemberName, MethodSignature, SigLength, tokenOut, methodAttributes, PInvokeModuleName);
	else
		return AddMethodRef(ModuleOrAssemblyName, TypeName, MemberName, MethodSignature, SigLength, tokenOut);
}

HRESULT ModuleMetadataHelpers::AddTypeDefOrRef(std::wstring &TypeName, mdToken &tokenOut, std::wstring &ModuleOrAssemblyName)
{
	if (ModuleOrAssemblyName == GetModuleName() | ModuleOrAssemblyName == GetAssemblyName())
		return AddTypeDef(TypeName, tokenOut);
	else
		return AddTypeRef(ModuleOrAssemblyName, TypeName, tokenOut);
}

HRESULT ModuleMetadataHelpers::FindTypeDefOrRef(std::wstring ModuleOrAssemblyName, std::wstring TypeName, mdToken & TypeRefOrDef)
{
	if (ModuleOrAssemblyName == GetModuleName() | ModuleOrAssemblyName == GetAssemblyName())
	{
		pMetaDataImport->FindTypeDefByName(TypeName.c_str(), NULL, &TypeRefOrDef);;

	}
	else {
		auto match = AssemblyRefs.find(ModuleOrAssemblyName);
		if (match != AssemblyRefs.end())
		{
			mdToken matchToken = match->second;
			HRESULT hr = pMetaDataImport->FindTypeRef(matchToken, TypeName.c_str(), &TypeRefOrDef);
			return hr;
		}

		match = ModuleRefs.find(ModuleOrAssemblyName);
		if (match != ModuleRefs.end())
		{
			mdToken matchToken = match->second;
			return pMetaDataImport->FindTypeRef(matchToken, TypeName.c_str(), &TypeRefOrDef);
		}
	}
	return E_FAIL;
}

HRESULT ModuleMetadataHelpers::FindMemberDefOrRef(std::wstring ModuleOrAssembly, std::wstring TypeName, std::wstring MemberName, PCCOR_SIGNATURE MethodSignature, ULONG SigLength, mdToken & TypeRefOrDef)
{
	if (ModuleOrAssembly == GetModuleName() | ModuleOrAssembly == GetAssemblyName())
	{
		pMetaDataImport->FindTypeDefByName(TypeName.c_str(), NULL, &TypeRefOrDef);
		if (TypeRefOrDef != mdTypeDefNil)
		{
			return pMetaDataImport->FindMember(TypeRefOrDef, MemberName.c_str(), MethodSignature, SigLength, &TypeRefOrDef);
		}
	}
	else {
		auto match = AssemblyRefs.find(ModuleOrAssembly);
		if (match != AssemblyRefs.end())
		{
			mdToken matchToken = match->second;
			pMetaDataImport->FindTypeRef(matchToken, TypeName.c_str(), &TypeRefOrDef);
			return pMetaDataImport->FindMemberRef(TypeRefOrDef, MemberName.c_str(), MethodSignature, SigLength, &TypeRefOrDef);
		}
		match = ModuleRefs.find(ModuleOrAssembly);
		if (match != ModuleRefs.end())
		{
			mdToken matchToken = match->second;
			pMetaDataImport->FindTypeRef(matchToken, TypeName.c_str(), &TypeRefOrDef);
			return pMetaDataImport->FindMemberRef(TypeRefOrDef, MemberName.c_str(), MethodSignature, SigLength, &TypeRefOrDef);
		}
	}
	return E_FAIL;
}

std::wstring ModuleMetadataHelpers::GetFullyQualifiedName(mdToken token, PCCOR_SIGNATURE *Signature, ULONG *SigLength)
{
	std::wstring module;
	std::wstring assembly;
	std::wstring type;
	std::wstring member;

	std::wstring fullname;

	mdTypeDef tkClass = mdTypeDefNil;
	mdToken tkResolution = mdTokenNil;
	WCHAR ModuleName[255];
	ULONG ModuleNameLen;
	WCHAR AssemblyName[255];
	ULONG AssemblyNameNameLen;
	WCHAR MethodName[255];
	ULONG MethodNameLen;
	WCHAR TypeName[255];
	ULONG TypeNameLen;
	ULONG MethodAttributes = 0;
	PCCOR_SIGNATURE signature = NULL;
	ULONG sigLen = 0;
	ULONG MethodRVA = 0;
	ULONG ImplementationFlags = 0;
	ULONG TypeDefFlags = 0;
	mdToken tkExtends = 0;

	switch (TypeFromToken(token))
	{
	case mdtMethodDef:

		assembly.assign(GetAssemblyName());
		pMetaDataImport->GetMethodProps(
			token,
			&tkClass,
			MethodName,
			_countof(MethodName),
			&MethodNameLen,
			&MethodAttributes,
			Signature,
			SigLength,
			&MethodRVA,
			&ImplementationFlags);
		member.assign(MethodName);

		if (tkClass != mdTypeDefNil)
		{
			pMetaDataImport->GetTypeDefProps(
				tkClass,
				TypeName,
				_countof(TypeName),
				&TypeNameLen,
				&TypeDefFlags,
				&tkExtends);
			type.assign(TypeName);
		}
		break;
	case mdtMemberRef:
		pMetaDataImport->GetMemberRefProps(
			token,
			&tkClass,
			MethodName,
			_countof(MethodName),
			&MethodNameLen,
			&signature,
			&sigLen);
		member.assign(MethodName);

		switch (TypeFromToken(tkClass))
		{
		case mdtTypeRef:
			pMetaDataImport->GetTypeRefProps(
				tkClass,
				&tkResolution,
				TypeName,
				_countof(TypeName),
				&TypeNameLen);
			break;
		case mdtTypeDef:
			pMetaDataImport->GetTypeDefProps(
				tkClass,
				TypeName,
				_countof(TypeName),
				&TypeNameLen,
				&TypeDefFlags,
				&tkExtends);
		case mdtTypeSpec:
			type.assign(L"TYPESPEC");
			break;
		default:
			break;
		}
		if (type.empty())
		{
			type.assign(TypeName);
		}
		if (tkResolution != mdTokenNil)
		{
			switch (TypeFromToken(tkResolution))
			{
			case mdtAssemblyRef:
				pMetaDataAssemblyImport->GetAssemblyRefProps(tkResolution,
					NULL,
					NULL,
					AssemblyName,
					_countof(AssemblyName),
					&AssemblyNameNameLen,
					NULL,
					NULL,
					NULL,
					NULL);
				assembly.assign(AssemblyName);
				break;
			case mdtModuleRef:
				pMetaDataImport->GetModuleRefProps(tkResolution,
					ModuleName,
					_countof(ModuleName),
					&ModuleNameLen);
				module.assign(ModuleName);
				break;
			default:
				break;
			}
		}

		break;
	case mdtFieldDef:
		break;
	case mdtTypeDef:
		pMetaDataImport->GetTypeDefProps(
			token,
			TypeName,
			_countof(TypeName),
			&TypeNameLen,
			&TypeDefFlags,
			&tkExtends);
		type.assign(TypeName);
	case mdtTypeRef:
		pMetaDataImport->GetTypeRefProps(
			token,
			&tkResolution,
			TypeName,
			_countof(TypeName),
			&TypeNameLen);
		type.assign(TypeName);
		if (tkResolution != mdTokenNil)
		{
			switch (TypeFromToken(tkResolution))
			{
			case mdtAssemblyRef:
				pMetaDataAssemblyImport->GetAssemblyRefProps(tkResolution,
					NULL,
					NULL,
					AssemblyName,
					_countof(AssemblyName),
					&AssemblyNameNameLen,
					NULL,
					NULL,
					NULL,
					NULL);
				assembly.assign(AssemblyName);
				break;
			case mdtModuleRef:
				pMetaDataImport->GetModuleRefProps(tkResolution,
					ModuleName,
					_countof(ModuleName),
					&ModuleNameLen);
				module.assign(ModuleName);
				break;
			default:
				break;
			}
		}
		break;
	case mdtTypeSpec:
		type.assign(L"TYPESPEC");
			break;
	case mdtMethodSpec:
		type.assign(L"METHODSPEC");
	default:
		break;
	}
	fullname.append(L"[");
	if (!module.empty())
	{
		fullname.append(module);
		fullname.append(L"!");
	}
	fullname.append(assembly);
	fullname.append(L"]");
	fullname.append(type);
	if (!member.empty())
	{
		fullname.append(L"::");
		fullname.append(member);
	}

	return fullname;
}

HRESULT ModuleMetadataHelpers::AddAssemblyRef(std::wstring AssemblyName, mdAssemblyRef & AssemblyRef) {
	return E_NOTIMPL;
	//return pMetaDataAssemblyEmit->DefineAssemblyRef(AssemblyName.c_str(), &AssemblyRef);
}

HRESULT ModuleMetadataHelpers::AddModuleRef(std::wstring ModuleOrAssemblyName, mdModuleRef & ModuleRef) {
	HRESULT hr = E_FAIL;
	hr = pMetaDataEmit->DefineModuleRef(ModuleOrAssemblyName.c_str(), &ModuleRef);
	ModuleRefs.emplace(ModuleOrAssemblyName.c_str(), ModuleRef);
	return hr;
}

HRESULT ModuleMetadataHelpers::AddMethodDef(std::wstring TypeName, std::wstring MethodName,
	PCCOR_SIGNATURE MethodSignature, ULONG SigLength, mdMethodDef & MethodDef, ULONG methodAttributes, std::wstring PInvokeModuleName) {

	HRESULT hr;

	// Find the type definition or reference to add this method to.
	mdToken tokenOut = mdTokenNil;
	FindTypeDefOrRef(GetModuleName(), TypeName, tokenOut);

	// Find the constructor of this type (hopefully there is one) and get the RVA
	// This is required so the metadata interfaces won't scream. It will look for
	// void <TYPE>();
	COR_SIGNATURE ctorSignature[] =
	{
		IMAGE_CEE_CS_CALLCONV_HASTHIS, //__stdcall
		0,
		ELEMENT_TYPE_VOID
	};

	mdMethodDef mdCtor = NULL;
	hr = pMetaDataImport->FindMethod(
		tokenOut,
		L".ctor",
		ctorSignature,
		sizeof(ctorSignature),
		&mdCtor);

	if (FAILED(hr))
		return hr;

	ULONG rvaCtor;
	hr = pMetaDataImport->GetMethodProps(
		mdCtor,
		NULL,		   // Put method's class here. 
		NULL,		   // Put method's name here.  
		0,			   // Size of szMethod buffer in wide chars.   
		NULL,		   // Put actual size here 
		NULL,		   // Put flags here.  
		NULL,		   // [OUT] point to the blob value of meta data   
		NULL,		   // [OUT] actual size of signature blob  
		&rvaCtor,
		NULL);

	if (FAILED(hr))
		return hr;


	// Here we define the method for this type
	// We are setting it static public for now so it can be called from any method.
	hr = pMetaDataEmit->DefineMethod(
		tokenOut,
		MethodName.c_str(),
		methodAttributes,
		MethodSignature,
		sizeof(MethodSignature),
		rvaCtor,
		miIL | miNoInlining,
		&MethodDef);

	// If our incoming method is a PInvoke we need to add in a reference for it.
	if (IsMdPinvokeImpl(methodAttributes))
	{
		mdModuleRef clrProfilerRef;

		auto match = ModuleRefs.find(PInvokeModuleName);
		if (match == ModuleRefs.end())
		{
			AddModuleRef(PInvokeModuleName, clrProfilerRef);
		}
		else {
			clrProfilerRef = match->second;
		}
		hr = pMetaDataEmit->DefinePinvokeMap(MethodDef, pmNoMangle | pmCharSetUnicode | pmCallConvCdecl, MethodName.c_str(), clrProfilerRef);
	}

	// Get the security safe critical so we can execute in partial trust environments
	mdMethodDef mdSafeCritical;
	GetSecuritySafeCriticalAttributeToken(mdSafeCritical);

	mdToken tkCustomAttribute;
	hr = pMetaDataEmit->DefineCustomAttribute(
		MethodDef,
		mdSafeCritical,
		NULL,          //Blob, contains constructor params in this case none
		0,             //Size of the blob
		&tkCustomAttribute);
	return hr;
}

HRESULT ModuleMetadataHelpers::AddMethodRef(std::wstring ModuleOrAssemblyName, std::wstring TypeName, std::wstring MethodName, PCCOR_SIGNATURE MethodSignature, ULONG SigLength, mdMemberRef & MethodRef) {
	mdToken matchToken = mdTokenNil;
	auto match = AssemblyRefs.find(ModuleName);
	if (match != AssemblyRefs.end())
	{
		matchToken = match->second;
	}
	return pMetaDataEmit->DefineMemberRef(matchToken, TypeName.c_str(), MethodSignature, 0, &MethodRef);
}

HRESULT ModuleMetadataHelpers::AddTypeRef(std::wstring ModuleName, std::wstring TypeName, mdTypeRef & TypeRef) {
	mdToken matchToken = mdTokenNil;
	auto match = AssemblyRefs.find(ModuleName);
	if (match != AssemblyRefs.end())
	{
		matchToken = match->second;
	}
	return pMetaDataEmit->DefineTypeRefByName(matchToken, TypeName.c_str(), &TypeRef);
}

HRESULT ModuleMetadataHelpers::AddTypeDef(std::wstring TypeName, mdTypeDef & TypeDef) {
	return pMetaDataEmit->DefineTypeDef(TypeName.c_str(), 0, mdTokenNil, NULL, &TypeDef);
}

HRESULT ModuleMetadataHelpers::GetSecuritySafeCriticalAttributeToken(mdMethodDef & pmdSafeCritical)
{
	mdToken matchToken = mdTokenNil;
	HRESULT hr = E_FAIL;

	auto match = AssemblyRefs.find(L"mscorlib");
	if (match != AssemblyRefs.end())
	{
		matchToken = match->second;
	}

	mdTypeDef tdSafeCritical;
	if (ModuleName == L"mscorlib")
	{
		hr = pMetaDataImport->FindTypeDefByName(
			L"System.Security.SecuritySafeCriticalAttribute",
			mdTokenNil,
			&tdSafeCritical);
	}
	else {
		hr = pMetaDataImport->FindTypeRef(matchToken,
			L"System.Security.SecuritySafeCriticalAttribute",
			&tdSafeCritical);
	}


	COR_SIGNATURE sigSafeCriticalCtor[] = {
		IMAGE_CEE_CS_CALLCONV_HASTHIS,
		0x00,                               // number of arguments == 0
		ELEMENT_TYPE_VOID,                  // return type == void
	};

	hr = pMetaDataImport->FindMemberRef(
		tdSafeCritical,
		L".ctor",
		sigSafeCriticalCtor,
		sizeof(sigSafeCriticalCtor),
		&pmdSafeCritical);

	return hr;
}

HRESULT ModuleMetadataHelpers::AddMethodLocals(std::wstring TypeName, std::wstring MethodName, PCCOR_SIGNATURE LocalsSignature, mdSignature & SigToken)
{
	return E_NOTIMPL;
}

Method& ModuleMetadataHelpers::AddMethodToRewriteList(mdMethodDef MethodToAdd)
{
	return Method();
}


//STDMETHODIMP Cprofilermain::GetFullMethodName(FunctionID functionID, std::wstring &methodName)
//{
//	// CRITICAL 8 Move this to the metadata helpers class.
//	IMetaDataImport* pIMetaDataImport = 0;
//	HRESULT hr = S_OK;
//	mdToken funcToken = 0;
//	WCHAR szFunction[NAME_BUFFER_SIZE];
//	WCHAR szClass[NAME_BUFFER_SIZE];
//	WCHAR wszMethod[NAME_BUFFER_SIZE];
//	ClassID clsID;
//
//
//	// get the token for the function which we will use to get its name
//	hr = this->m_pICorProfilerInfo->GetFunctionInfo(functionID,  &funcToken);
//	this->m_pICorProfilerInfo->function
//
//	if (SUCCEEDED(hr))
//	{
//		mdTypeDef classTypeDef;
//		ULONG cchFunction;
//		ULONG cchClass;
//
//		// retrieve the function properties based on the token
//		hr = pIMetaDataImport->GetMethodProps(funcToken, &classTypeDef, szFunction, NAME_BUFFER_SIZE, &cchFunction, 0, 0, 0, 0, 0);
//		if (SUCCEEDED(hr))
//		{
//			// get the function name
//			hr = pIMetaDataImport->GetTypeDefProps(classTypeDef, szClass, NAME_BUFFER_SIZE, &cchClass, 0, 0);
//			if (SUCCEEDED(hr))
//			{
//				// create the fully qualified name
//				_snwprintf_s<NAME_BUFFER_SIZE>(wszMethod, NAME_BUFFER_SIZE, L"%s.%s", szClass, szFunction);
//				methodName.assign(std::wstring(wszMethod));
//			}
//		}
//		// release our reference to the metadata
//		pIMetaDataImport->Release();
//	}
//
//	return hr;
//}
//
//STDMETHODIMP Cprofilermain::GetFuncArgs(FunctionID functionID, COR_PRF_FRAME_INFO frameinfo)
//{
//	// CRITICAL 8 Move this to the metadata helpers class.
//	HRESULT hr = S_OK;
//	ClassID classID;
//	ModuleID modId;
//	mdToken token;
//	ULONG32 typeArgsMax = 1024;
//	ULONG32 typeArgsOut = 0;
//	ClassID typeArgs[1024];
//
//
//	switch (this->m_HighestProfileInfo)
//	{
//	case 1:
//		this->m_pICorProfilerInfo->GetFunctionInfo(functionID,
//			&classID,
//			&modId,
//			&token);
//	default:
//		hr = this->m_pICorProfilerInfo2->GetFunctionInfo2(
//			functionID,
//			frameinfo,
//			&classID,
//			&modId,
//			&token,
//			typeArgsMax,
//			&typeArgsOut,
//			typeArgs);
//		break;
//	}
//	// get the token for the function which we will use to get its name
//
//	return hr;
//}
