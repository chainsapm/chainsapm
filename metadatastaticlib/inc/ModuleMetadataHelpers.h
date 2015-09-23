#pragma once
#include "stdafx.h"
#define MAX_LENGTH 2048

class ModuleMetadataHelpers
{
public:
	ModuleMetadataHelpers(ATL::CComPtr<ICorProfilerInfo> profilerInfo, ModuleID moduleID);
	~ModuleMetadataHelpers();
	// Return the module name
	std::wstring GetModuleName();
	// Find a type defined or referenced inside of this module
	HRESULT FindTypeDefOrRef(std::wstring ModuleName, std::wstring TypeName, mdToken &TypeRefOrDef);
	// Find a method or member for a type inside of the current module
	HRESULT FindMemberDefOrRef(std::wstring ModuleOrAssembly, std::wstring TypeName, std::wstring MemberName, mdToken & TypeRefOrDef);
	
	// Find a token to be remapped to a proper Definition or Reference
	mdToken GetMappedToken(mdToken);
	
	//PCCOR_SIGNATURE ParseElementType(const CComPtr<IMetaDataImport>& pMDImport, PCCOR_SIGNATURE signature, std::wstring* buffer);

	// Get a forward copy of all of the current assembly references in this module
	void PopulateAssemblyRefs();
	// Get a forward copy of all of the current module references in this module
	void PopulateModuleRefs();
	
	// Given the proper parameters this will either Find or Define a Type or Member
	HRESULT DefineTokenReference(std::wstring ModuleOrAssembly, std::wstring TypeName, std::wstring MemberName, mdToken tokenIn);
	// Define new type inside current module
	HRESULT AddTypeDef(std::wstring TypeName, mdToken & TypeRefOrDef);
	// Reference type outside current module
	HRESULT AddTypeRef(std::wstring ModuleName, std::wstring TypeName, mdToken & TypeRefOrDef);
	// Reference module outside current module
	HRESULT AddModuleRef(std::wstring ModuleName, mdModuleRef & TypeRefOrDef);
	// Reference assembly outside current module
	HRESULT AddAssemblyRef(std::wstring AssemblyName, mdAssemblyRef & TypeRefOrDef);
	// Reference method (member) outside current module
	HRESULT AddMethodRef(std::wstring ModuleOrAssemblyName, std::wstring TypeName, std::wstring MethodName, PCCOR_SIGNATURE MethodSignature, mdMemberRef & MethodRef);
	// Create new method inside of this module
	HRESULT AddMethodDef(std::wstring TypeName, std::wstring MethodName, PCCOR_SIGNATURE MethodSignature, mdMethodDef & MethodDef);
	// Create signature token for this method
	HRESULT AddMethodLocals(std::wstring TypeName, std::wstring MethodName, PCCOR_SIGNATURE LocalsSignature, mdSignature & SigToken);

	

private:
	ModuleID ThisModuleID;
	std::wstring ModuleName;
	std::map<std::wstring, mdAssemblyRef> AssemblyRefs;
	std::map<std::wstring, mdModuleRef> ModuleRefs;
	std::map<mdToken, mdToken> TokenMapping;

	// container for ICorProfilerInfo reference
	ATL::CComPtr<ICorProfilerInfo> pICorProfilerInfo;
	ATL::CComPtr<IMetaDataEmit2> pMetaDataEmit;
	ATL::CComPtr<IMetaDataImport2> pMetaDataImport;
	ATL::CComPtr<IMetaDataAssemblyEmit> pMetaDataAssemblyEmit;
	ATL::CComPtr<IMetaDataAssemblyImport> pMetaDataAssemblyImport;
	//boost::mutex m_ThreadIdMutex;

	HRESULT GetSecuritySafeCriticalAttributeToken(mdMethodDef & pmdSafeCritical);

	CRITICAL_SECTION m_ThreadCS;
	// Saving reference to the containers in order to provide specific function lookups, eg. Is this method an entry point

};

