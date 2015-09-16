#pragma once
#include "AssemblyInfo.h"
#include "../ModuleMetadataHelpers.h"

namespace InformationClasses {
	class ModuleInfo
	{
	public:
		ModuleInfo();
		~ModuleInfo();
		std::wstring ModuleName;
		std::wstring FileName;
	};
}
