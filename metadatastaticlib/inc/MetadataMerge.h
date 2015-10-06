#pragma once
#include "ModuleMetadataHelpers.h"
class MetadataMerge {
public:
	MetadataMerge(std::shared_ptr<ModuleMetadataHelpers> InjectedMethodHelper, std::shared_ptr<ModuleMetadataHelpers> TargetModuleHelper);
	void InjectTypes();
	void InjectMembers();
};