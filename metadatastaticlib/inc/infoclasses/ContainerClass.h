#pragma once

struct ItemMapping
{
	enum class MatchType
	{
		FullyQualified,				// System.Console::WriteLine(string)
		FunctionAndSignature,		// WriteLine(string)
		FunctionAndClass,			// System.Console::WriteLine
		FunctionOnly,				// WriteLine
		FullyQualifiedDerivesFrom,	// System.Data.IDbCommand::.ctor(System.Data.IDbConnection)
		DerivesFrom,				// System.Data.IDbCommand::Execute
		AllInModule,				// EVERYTHING in the specified Module
		AllInClass,					// EVERYTHING in the specified Class
		AllInAssembly				// EVERYTHING in the specified Assembly
	};

	enum class StringMatchMethod
	{
		BEGIN,
		CONTAINS,
		END
	};

	std::wstring FunctionName;
	std::wstring ClassName;
	std::wstring AssemblyName;
	std::wstring ModuleName;
	std::wstring DerivesFrom;
	std::wstring Signature;

	std::wstring HashString;

	StringMatchMethod Compare;
	MatchType Match;

	bool operator == (const std::wstring &strCompare);

	bool IgnoreGetters;
	bool IgnoreSetters;
};

class EqualFn
{
public:
	bool operator() (ItemMapping * t1, ItemMapping * t2) const
	{
		switch (t1->Match)
		{
		case ItemMapping::MatchType::AllInAssembly:
			return t1->AssemblyName == t2->AssemblyName;
			break;
		case ItemMapping::MatchType::AllInClass:
			return t1->ClassName == t2->ModuleName;
			break;
		case ItemMapping::MatchType::AllInModule:
			return t1->ModuleName == t2->ClassName;
			break;
		case ItemMapping::MatchType::DerivesFrom:
			return t1->DerivesFrom == t2->DerivesFrom;
			break;
		case ItemMapping::MatchType::FullyQualified:
			return (t1->ClassName == t2->ClassName) && (t1->FunctionName == t2->FunctionName) && (t1->Signature == t2->Signature);
			break;
		case ItemMapping::MatchType::FullyQualifiedDerivesFrom:
			return (t1->DerivesFrom == t2->DerivesFrom) && (t1->ClassName == t2->ClassName) && (t1->FunctionName == t2->FunctionName) && (t1->Signature == t2->Signature);
			break;
		case ItemMapping::MatchType::FunctionAndClass:
			return (t1->ClassName == t2->ClassName) && (t1->FunctionName == t2->FunctionName);
			break;
		case ItemMapping::MatchType::FunctionAndSignature:
			return (t1->FunctionName == t2->FunctionName) && (t1->Signature == t2->Signature);
			break;
		case ItemMapping::MatchType::FunctionOnly:
			return t1->FunctionName == t2->FunctionName;
			break;
		default:
			break;
		}
	}
};

class Hasher
{
public:
	size_t operator() (ItemMapping *key) const
	{
		switch (key->Match)
		{
		case ItemMapping::MatchType::AllInAssembly:
			if (!key->HashString.length() > 0)
			{
				key->HashString = key->AssemblyName;
			}
			break;
		case ItemMapping::MatchType::AllInClass:
			if (!key->HashString.length() > 0)
			{
				key->HashString = key->ClassName;
			}
			break;
		case ItemMapping::MatchType::AllInModule:
			if (!key->HashString.length() > 0)
			{
				key->HashString = key->ModuleName;
			}
			break;
		case ItemMapping::MatchType::DerivesFrom:
			if (!key->HashString.length() > 0)
			{
				key->HashString = key->DerivesFrom;
			}
			break;
		case ItemMapping::MatchType::FullyQualified:
			if (!key->HashString.length() > 0)
			{
				key->HashString.insert(0, key->DerivesFrom);
				key->HashString.insert(key->DerivesFrom.length(), key->FunctionName);
			}
			break;
		case ItemMapping::MatchType::FullyQualifiedDerivesFrom:
			if (!key->HashString.length() > 0)
			{
				key->HashString.insert(0, key->DerivesFrom);
				key->HashString.insert(key->DerivesFrom.length(), key->FunctionName);
				key->HashString.insert(key->DerivesFrom.length() + key->FunctionName.length(), key->Signature);
			}
			break;
		case ItemMapping::MatchType::FunctionAndClass:
			if (!key->HashString.length() > 0)
			{
				key->HashString.insert(0, key->ClassName);
				key->HashString.insert(key->ClassName.length(), key->FunctionName);
			}
			break;
		case ItemMapping::MatchType::FunctionAndSignature:
			if (!key->HashString.length() > 0)
			{
				key->HashString.insert(0, key->FunctionName);
				key->HashString.insert(key->FunctionName.length(), key->Signature);
			}
			break;
		case ItemMapping::MatchType::FunctionOnly:
			if (!key->HashString.length() > 0)
			{
				key->HashString = key->FunctionName;
			}
			break;
		default:
			break;
		}
		return std::hash<std::wstring>()(key->HashString);
	}
};

class LessFunctionIDFn
{
public:
	bool operator() (FunctionID  t1, FunctionID  t2) const
	{

		return (!(t1 < t2));
	}
};

#include <list>
#include <set>
#include <memory>
#include <allocators>


//std::list<int, alloc<int> > _List1;

// This public only implementation of the class hold ALL the pointers to the newed up containers when initializing 
// a new profiler.  This container simplifies the process of mapping the proper profiler to the static method
//class Cprofilermain;
#pragma once
#include "FunctionInfo.h"

class StackItemBase;
class ModuleMetadataHelpers;

_ALLOCATOR_DECL(CACHE_FREELIST(stdext::allocators::max_fixed_size<50000>), stdext::allocators::sync_none, ALLOC_500);



namespace std{
	template<>
	class hash < ItemMapping >
	{
	public:
		size_t operator() (const ItemMapping& key)
		{
			return std::hash<std::wstring>()(key.FunctionName);
		}
	};
}

namespace std{
	template<>
	class equal_to < ItemMapping >
	{
	public:
		bool operator() (const ItemMapping& t1, const ItemMapping& t2) const
		{
			switch (t1.Match)
			{
			case ItemMapping::MatchType::AllInAssembly:
				return t1.AssemblyName == t2.AssemblyName;
				break;
			case ItemMapping::MatchType::AllInClass:
				return t1.ClassName == t2.ModuleName;
				break;
			case ItemMapping::MatchType::AllInModule:
				return t1.ModuleName == t2.ClassName;
				break;
			case ItemMapping::MatchType::DerivesFrom:
				return t1.DerivesFrom == t2.DerivesFrom;
				break;
			case ItemMapping::MatchType::FullyQualified:
				return (t1.ClassName == t2.ClassName) && (t1.FunctionName == t2.FunctionName) && (t1.Signature == t2.Signature);
				break;
			case ItemMapping::MatchType::FullyQualifiedDerivesFrom:
				return (t1.DerivesFrom == t2.DerivesFrom) && (t1.ClassName == t2.ClassName) && (t1.FunctionName == t2.FunctionName) && (t1.Signature == t2.Signature);
				break;
			case ItemMapping::MatchType::FunctionAndClass:
				return (t1.ClassName == t2.ClassName) && (t1.FunctionName == t2.FunctionName);
				break;
			case ItemMapping::MatchType::FunctionAndSignature:
				return (t1.FunctionName == t2.FunctionName) && (t1.Signature == t2.Signature);
				break;
			case ItemMapping::MatchType::FunctionOnly:
				return t1.FunctionName == t2.FunctionName;
				break;
			default:
				break;
			}
		}
	};
}

namespace std{
	template<>
	class less < ItemMapping >
	{
	public:
		bool operator() (const ItemMapping& t1, const ItemMapping&  t2) const
		{
			switch (t2.Match)
			{
			case ItemMapping::MatchType::AllInAssembly:
				return t1.AssemblyName == t2.AssemblyName;
				break;
			case ItemMapping::MatchType::AllInClass:
				return t1.ClassName == t2.ModuleName;
				break;
			case ItemMapping::MatchType::AllInModule:
				return t1.ModuleName == t2.ClassName;
				break;
			case ItemMapping::MatchType::DerivesFrom:
				return t1.DerivesFrom == t2.DerivesFrom;
				break;
			case ItemMapping::MatchType::FullyQualified:
				return (t1.ClassName == t2.ClassName) && (t1.FunctionName == t2.FunctionName) && (t1.Signature == t2.Signature);
				break;
			case ItemMapping::MatchType::FullyQualifiedDerivesFrom:
				return (t1.DerivesFrom == t2.DerivesFrom) && (t1.ClassName == t2.ClassName) && (t1.FunctionName == t2.FunctionName) && (t1.Signature == t2.Signature);
				break;
			case ItemMapping::MatchType::FunctionAndClass:
				return (t1.ClassName == t2.ClassName) && (t1.FunctionName == t2.FunctionName);
				break;
			case ItemMapping::MatchType::FunctionAndSignature:
				return (t1.FunctionName == t2.FunctionName) && (t1.Signature == t2.Signature);
				break;
			case ItemMapping::MatchType::FunctionOnly:
				return t1.FunctionName == t2.FunctionName;
				break;
			default:
				break;
			}
		}
	};
}


struct  ContainerClass
{

	ContainerClass();
	~ContainerClass();

	// Class that holds 
	ModuleMetadataHelpers *  g_MetadataHelpers;

	// As a function is mapped we want to keep a reference to it's specific details so we can 
	// use it again when generating the call stack.
	std::map<FunctionID, std::unique_ptr<InformationClasses::FunctionInfo>> * g_FunctionSet;
	std::map<ClassID, std::unique_ptr<InformationClasses::TypeInfo>> * g_ClassSet;
	std::map<AssemblyID, std::unique_ptr<InformationClasses::AssemblyInfo>> * g_AssemblySet;
	std::map<ModuleID, std::unique_ptr<InformationClasses::ModuleInfo>> * g_ModuleSet;
	std::unordered_multiset<ItemMapping> * g_FullyQualifiedMethodsToProfile;



	///////////////////////////////////////////////////////////////////////////////////////
	// _Critical Sections 
	///////////////////////////////////////////////////////////////////////////////////////

	// For locking inserts to the main thread map. This is used quite a bit when threads become active 
	CRITICAL_SECTION g_ThreadingCriticalSection;				// g_ThreadStackMap
	// For locking inserts to the thread sequence
	CRITICAL_SECTION g_ThreadStackSequenceCriticalSection;		// g_ThreadStackSequence
	// For locking inserts to the thread stack depth
	CRITICAL_SECTION g_ThreadStackDepthCriticalSection;			// g_ThreadStackDepth
	// For locking inserts to the function information classes
	CRITICAL_SECTION g_FunctionSetCriticalSection;				// g_FunctionSet
	// For locking around the metdata helper
	CRITICAL_SECTION g_MetaDataCriticalSection;					// g_MetadataHelpers
	// For locking around the threadEntryPoint helper
	CRITICAL_SECTION g_EntryPointCriticalSection;				// g_EntryPointStackMap
	// For locking around the entrypoint counter
	CRITICAL_SECTION g_ThreadEntrypointCriticalSection;			// g_ThreadEntrypointID

};