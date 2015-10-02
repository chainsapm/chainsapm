#pragma once
#pragma once
// Cor Profiling Libraries
#include <cor.h>
#include <corprof.h>
#include <memory>
// ATL LIbraries
#include <atlbase.h>
#include <atlcom.h>
#include <atlctl.h>
#include "Method.h"

#define MAX_LENGTH 2048

class IMetadataHelper
{
public:
	// Return the module name
	virtual std::wstring GetModuleName() = 0;
	// Return the assembly name
	virtual std::wstring GetAssemblyName() = 0;
	// Find a type defined or referenced inside of this module
	virtual HRESULT FindTypeDefOrRef(std::wstring ModuleName, std::wstring TypeName, mdToken &TypeRefOrDef) = 0;
	// Find a method or member for a type inside of the current module
	virtual HRESULT FindMemberDefOrRef(std::wstring ModuleOrAssembly, std::wstring TypeName, std::wstring MemberName, PCCOR_SIGNATURE MethodSignature, ULONG SigLength, mdToken & TypeRefOrDef) = 0;
	// Return the item name based on token and optionally signature
	virtual std::wstring GetFullyQualifiedName(mdToken token, PCCOR_SIGNATURE * Signature, ULONG * SigLength) = 0;
	// Find a token to be remapped to a proper Definition or Reference
	virtual mdToken GetMappedToken(mdToken) = 0;
	// Define a signature
	virtual mdToken DefineSigToken(mdSignature original, PCCOR_SIGNATURE pNewSig, ULONG sigLen, mdSignature & replacement) = 0;
	// Get a forward copy of all of the current assembly references in this module
	virtual void PopulateAssemblyRefs() = 0;
	// Get a forward copy of all of the current module references in this module
	virtual void PopulateModuleRefs() = 0;
	// Given the proper parameters this will either Find or Define a Type or Member
	virtual HRESULT DefineTokenReference(std::wstring ModuleOrAssembly, std::wstring TypeName, std::wstring MemberName, PCCOR_SIGNATURE MethodSignature, ULONG SigLength,
		mdToken mdParentToken, mdToken tokenIn, ULONG methodAttributes, std::wstring PInvokeModuleName) = 0;
	// Define a member for this metadata helper
	virtual HRESULT AddMemberRefOrDef(std::wstring &TypeName, std::wstring &MemberName, const PCCOR_SIGNATURE &MethodSignature, ULONG SigLength, mdToken &tokenOut, std::wstring &ModuleOrAssemblyName, ULONG methodAttributes, std::wstring PInvokeModuleName) = 0;
	// Define a type for this metadata helper
	virtual HRESULT AddTypeDefOrRef(std::wstring &TypeName, mdToken &tokenOut, std::wstring &ModuleOrAssembly) = 0;
	// Define new type inside current module
	virtual HRESULT AddTypeDef(std::wstring TypeName, mdToken & TypeRefOrDef) = 0;
	// Reference type outside current module
	virtual HRESULT AddTypeRef(std::wstring ModuleName, std::wstring TypeName, mdToken & TypeRefOrDef) = 0;
	// Reference module outside current module
	virtual HRESULT AddModuleRef(std::wstring ModuleName, mdModuleRef & TypeRefOrDef) = 0;
	// Reference assembly outside current module
	virtual HRESULT AddAssemblyRef(std::wstring AssemblyName, mdAssemblyRef & TypeRefOrDef) = 0;
	// Reference method (member) outside current module
	virtual HRESULT AddMethodRef(std::wstring ModuleOrAssemblyName, std::wstring TypeName, std::wstring MethodName, PCCOR_SIGNATURE MethodSignature, ULONG SigLength, mdMemberRef & MethodRef) = 0;
	// Create new method inside of this module
	virtual HRESULT AddMethodDef(std::wstring TypeName, std::wstring MethodName, PCCOR_SIGNATURE MethodSignature, ULONG SigLength, mdMethodDef & MethodDef, ULONG methodAttributes, std::wstring PInvokeModuleName) = 0;
	// Create signature token for this method
	virtual HRESULT AddMethodLocals(std::wstring TypeName, std::wstring MethodName, PCCOR_SIGNATURE LocalsSignature, mdSignature & SigToken) = 0;
	
	virtual Method& AddMethodToRewriteList(mdMethodDef MethodToAdd) = 0;

};
