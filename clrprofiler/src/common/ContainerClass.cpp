#include "stdafx.h"
#include "ContainerClass.h"
#include "../../metadatastaticlib/inc/infoclasses/commonstructures.h"
#include "stackitems.h"



bool ItemMapping::operator== (const std::wstring &strCompare)
{
	return !(this->HashString.compare(strCompare));
}

ContainerClass::ContainerClass()
{
	// TODO: Issue #2 Add more containers to properly handle a proper entry point.

	this->g_FunctionSet = new std::map<FunctionID, std::unique_ptr<InformationClasses::FunctionInfo>>();
	this->g_ClassSet = new  std::map<ClassID, std::unique_ptr<InformationClasses::TypeInfo>>();
	this->g_AssemblySet = new std::map<AssemblyID, std::unique_ptr<InformationClasses::AssemblyInfo>>();
	this->g_ModuleSet = new std::map<ModuleID, std::unique_ptr<InformationClasses::ModuleInfo>>();
	this->g_FullyQualifiedMethodsToProfile = new std::unordered_multiset<ItemMapping>();


	// CRITICAL 1 Thread synchronization for Function Mapper
	InitializeCriticalSection(&this->g_FunctionSetCriticalSection);
	InitializeCriticalSection(&this->g_ThreadingCriticalSection);
	InitializeCriticalSection(&this->g_ThreadStackDepthCriticalSection);
	InitializeCriticalSection(&this->g_ThreadStackSequenceCriticalSection);
	InitializeCriticalSection(&this->g_MetaDataCriticalSection);
	InitializeCriticalSection(&this->g_EntryPointCriticalSection);
	InitializeCriticalSection(&this->g_ThreadEntrypointCriticalSection);
}

ContainerClass::~ContainerClass()
{
	delete this->g_FunctionSet;
	DeleteCriticalSection(&this->g_FunctionSetCriticalSection);
	DeleteCriticalSection(&this->g_ThreadingCriticalSection);
	DeleteCriticalSection(&this->g_ThreadStackDepthCriticalSection);
	DeleteCriticalSection(&this->g_ThreadStackSequenceCriticalSection);
	DeleteCriticalSection(&this->g_MetaDataCriticalSection);
	DeleteCriticalSection(&this->g_EntryPointCriticalSection);
	DeleteCriticalSection(&this->g_ThreadEntrypointCriticalSection);
}