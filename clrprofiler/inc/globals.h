//#include "stdafx.h"
//
//class Cprofilermain;
//class MetadataHelpers;
//class FunctionInfo;
//class StackItemBase;
//
//
//// Global implementation of the profiler callback. Will eventually create a structure to hold multiple
//// for the SxS operation required for concurrent CLR hosting/
//extern Cprofilermain * g_ProfilerCallback;
//// Global implementation of the 
//extern MetadataHelpers *  g_MetadataHelpers;
//
//// As a function is mapped we want to keep a reference to it's specific details so we can 
//// use it again when generating the call stack.
//extern std::map<FunctionID, FunctionInfo> * g_FunctionSet;
//// Holds pointers to StackItemBase polymorphic class. This class is an extensible map of objects
//// that describe the state of this thread.
//extern std::map<ThreadID, std::queue<StackItemBase*>> * g_ThreadStackMap;
//// Holds the current depth of the stack based on the Enter / Leave / Tail calls
//extern std::map<ThreadID, volatile unsigned int> * g_ThreadStackDepth;
//// Holds the current sequence of the stack items
//extern std::map<ThreadID, volatile unsigned int> * g_ThreadStackSequence;
//// Holds the current number of functions that we've entered and exited. This will be used to stop
//// further additions to the stack. This really won't change performance too drastically, however it
//// will allow us to save memory
//extern std::map<ThreadID, volatile unsigned int> * g_ThreadFunctionCount;
//// This map is used to link together two threads as a parent and child. 
//extern std::map<ThreadID, UINT_PTR> * g_ThreadSpawnMap;
//
//// This is a structure to hold two or more instances of the Cprofilermain class that will
//// spawn the ICorProfiler* classes. This is used in instances where a SxS scenario is active
//// As of 08/16/2014 we have not implemented this.
//extern std::map<DWORD, Cprofilermain*> * g_InstanceMap;
//
//// **************************************** TESTING CLASSES ****************************************
//// This collection is used to validate that we should be mapping this function. As of right now,
//// for testing, we have this in both the mapper function and the ELT functions. Once we're out of
//// the testing phase this collection will only be used in the mapper and we can remove the extraneous
//// call to the find() method in the hooks.
//extern std::unordered_set<std::wstring> * g_FunctionNameSet;
//// This collection is used to validate that we should be mapping this entire class. As of right now,
//// for testing, we have this in both the mapper function and the ELT functions. Once we're out of
//// the testing phase this collection will only be used in the mapper and we can remove the extraneous
//// call to the find() method in the hooks.
//extern std::unordered_set<std::wstring> * g_ClassNameSet;
//// **************************************** TESTING CLASSES ****************************************
//
//
