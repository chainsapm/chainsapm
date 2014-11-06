enum STRINGCOMPARE
{
	BEGIN,
	CONTAINS,
	END
};
struct ItemMapping
{
	std::wstring ItemName;
	STRINGCOMPARE compare;
	BOOL IsEntryPoint;
	BOOL NegateCompare;
	BOOL operator == (const std::wstring &strCompare);
};

class EqualFn
{
public:
	bool operator() (ItemMapping * t1, ItemMapping * t2) const
	{

		return !(t1->ItemName.compare(t2->ItemName));
	}
};


class Hasher
{
public:
	size_t operator() (ItemMapping *key) const
	{
		return std::hash<std::wstring>()(key->ItemName);
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
#include <memory>
#include <allocators>


//std::list<int, alloc<int> > _List1;

// This public only implementation of the class hold ALL the pointers to the newed up containers when initializing 
// a new profiler.  This container simplifies the process of mapping the proper profiler to the static method
//class Cprofilermain;
class FunctionInfo;
class StackItemBase;
class MetadataHelpers;

_ALLOCATOR_DECL(CACHE_FREELIST(stdext::allocators::max_fixed_size<50000>), stdext::allocators::sync_none, ALLOC_500);



namespace std{
	template<>
	class hash < ItemMapping > ;
}


struct  ContainerClass
{

	ContainerClass();
	~ContainerClass();

	// Class that holds 
	MetadataHelpers *  g_MetadataHelpers;

	// As a function is mapped we want to keep a reference to it's specific details so we can 
	// use it again when generating the call stack.
	std::map<FunctionID, FunctionInfo*> * g_FunctionSet;

	// Holds pointers to StackItemBase polymorphic class. This class is an extensible map of objects
	// that describe the state of this thread.
	std::map<ThreadID, std::deque<std::shared_ptr<StackItemBase>>> * g_ThreadStackMap;

	// In order to properly capture units of work we need to have a container that allows for an "arbitrary" entry point
	// the best example is a web request. It is assigned to a thread on a thread pool, so the thread may have to be created
	// or it can be reused. In this case we would not have an entry point and the thread would execute "forever".
	std::map<LONGLONG, std::deque<StackItemBase*>> * g_EntryPointStackMap;

	// Entrypoint counter. The max number is 18,446,744,073,709,551,614 ... we should NEVER reach that.
	// If we were to assume that each entrypoint executed in 1ms and we could execute 96 at a time, that means we could execute 96,000 in a second.
	// Doing the math it would be about 600,000 years before we hit the counter limit. 
	volatile LONGLONG currentEntryPointCounter;

	// Keeps track of the current entrypoint on a thread.
	std::map<ThreadID, ULONGLONG> * g_ThreadEntrypointID;

	// Holds the current depth of the stack based on the Enter / Leave / Tail calls
	std::map<ThreadID, volatile unsigned int> * g_ThreadStackDepth;

	// Holds the current sequence of the stack items
	std::map<ThreadID, volatile unsigned int> * g_ThreadStackSequence;

	// Holds the current number of functions that we've entered and exited. This will be used to stop
	// further additions to the stack. This really won't change performance too drastically, however it
	// will allow us to save memory
	std::map<ThreadID, volatile unsigned int> * g_ThreadFunctionCount;

	// This map is used to link together two threads as a parent and child. 
	std::map<ThreadID, UINT_PTR> * g_ThreadSpawnMap;


	// **************************************** TESTING CLASSES ****************************************
	// This collection is used to validate that we should be mapping this function. As of right now,
	// for testing, we have this in both the mapper function and the ELT functions. Once we're out of
	// the testing phase this collection will only be used in the mapper and we can remove the extraneous
	// call to the find() method in the hooks.
	std::unordered_set < ItemMapping*, Hasher, EqualFn> *g_FunctionNameSet;
	// This collection is used to validate that we should be mapping this entire class. As of right now,
	// for testing, we have this in both the mapper function and the ELT functions. Once we're out of
	// the testing phase this collection will only be used in the mapper and we can remove the extraneous
	// call to the find() method in the hooks.
	std::unordered_set<std::wstring> * g_ClassNameSet;
	// **************************************** TESTING CLASSES ****************************************

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