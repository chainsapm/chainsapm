#pragma once
#include <map>
#include <string>
#include <cor.h>
struct Method
{
	std::wstring Module;
	std::wstring Assembly;
	std::wstring Type;
	std::wstring Name;
	
	mdToken AssemblyToken;
	mdToken ModuleToken;
	mdToken MethodToken;

	PCCOR_SIGNATURE RawSignature;
	std::map<ULONG, std::wstring> Locals;
	std::map<ULONG, std::wstring> Parameters;
	PBYTE MethodBytes;
};