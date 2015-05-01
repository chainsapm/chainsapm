/************************************************************************/
/* CorProfilerCallbackImplementation.cpp                                */
/*                                                                      */
/* Implementation of all CorProfilerCallback methods. This is required  */
/* for the profiling interface to attach to each method. If these are   */
/* not implemented profiling will fail.                                 */
/************************************************************************/

#include "stdafx.h"
#include "CorProfilerCallbackImplementation.h"

CorProfilerCallbackImplementation::CorProfilerCallbackImplementation()
{
}

CorProfilerCallbackImplementation::~CorProfilerCallbackImplementation()
{
}

#pragma region Profiler Initialize and Shutdown
HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::Initialize(IUnknown *pICorProfilerInfoUnk)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::Shutdown(void)
{

	return S_OK;
}


HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::InitializeForAttach(IUnknown *pCorProfilerInfoUnk, void *pvClientData, UINT cbClientData)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::ProfilerAttachComplete(void)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::ProfilerDetachSucceeded(void)
{
	return S_OK;
}
#pragma endregion

#pragma region Garbage Collection

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::GarbageCollectionStarted(int cGenerations, BOOL generationCollected[], COR_PRF_GC_REASON reason)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::GarbageCollectionFinished(void)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::FinalizeableObjectQueued(DWORD finalizerFlags, ObjectID objectID)
{
	return S_OK;
}
#pragma endregion

#pragma region AppDomain
HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::AppDomainCreationStarted(AppDomainID appDomainId)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::AppDomainCreationFinished(AppDomainID appDomainId, HRESULT hrStatus)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::AppDomainShutdownStarted(AppDomainID appDomainId)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::AppDomainShutdownFinished(AppDomainID appDomainId, HRESULT hrStatus)
{
	return S_OK;
}
#pragma endregion

#pragma region Loading and Unloading

#pragma region Assembly Loading

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::AssemblyLoadStarted(AssemblyID assemblyId)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::AssemblyLoadFinished(AssemblyID assemblyId, HRESULT hrStatus)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::AssemblyUnloadStarted(AssemblyID assemblyId)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::AssemblyUnloadFinished(AssemblyID assemblyId, HRESULT hrStatus)
{
	return S_OK;
}
#pragma endregion

#pragma region Module Loading
HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::ModuleLoadStarted(ModuleID moduleId)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::ModuleLoadFinished(ModuleID moduleId, HRESULT hrStatus)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::ModuleUnloadStarted(ModuleID moduleId)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::ModuleUnloadFinished(ModuleID moduleId, HRESULT hrStatus)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::ModuleAttachedToAssembly(ModuleID moduleId, AssemblyID AssemblyId)
{
	return S_OK;
}
#pragma endregion

#pragma region Class Loading
HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::ClassLoadStarted(ClassID classId)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::ClassLoadFinished(ClassID classId, HRESULT hrStatus)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::ClassUnloadStarted(ClassID classId)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::ClassUnloadFinished(ClassID classId, HRESULT hrStatus)
{
	return S_OK;
}
#pragma endregion

#pragma region Function Unloading
HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::FunctionUnloadStarted(FunctionID functionId)
{
	return S_OK;
}

#pragma endregion

#pragma endregion

#pragma region JITing
HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::JITCompilationStarted(FunctionID functionId, BOOL fIsSafeToBlock)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::JITCompilationFinished(FunctionID functionId, HRESULT hrStatus, BOOL fIsSafeToBlock)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::JITCachedFunctionSearchStarted(FunctionID functionId, BOOL *pbUseCachedFunction)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::JITCachedFunctionSearchFinished(FunctionID functionId, COR_PRF_JIT_CACHE result)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::JITFunctionPitched(FunctionID functionId)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::JITInlining(FunctionID callerId, FunctionID calleeId, BOOL *pfShouldInline)
{
	return S_OK;
}
#pragma endregion

#pragma region ReJIT
HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::ReJITCompilationStarted(FunctionID functionId, ReJITID rejitId, BOOL fIsSafeToBlock)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::GetReJITParameters(ModuleID moduleId, mdMethodDef methodId, ICorProfilerFunctionControl *pFunctionControl)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::ReJITCompilationFinished(FunctionID functionId, ReJITID rejitId, HRESULT hrStatus, BOOL fIsSafeToBlock)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::ReJITError(ModuleID moduleId, mdMethodDef methodId, FunctionID functionId, HRESULT hrStatus)
{
	return S_OK;
}
#pragma endregion

#pragma region Threads
HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::ThreadCreated(ThreadID threadId)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::ThreadDestroyed(ThreadID threadId)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::ThreadAssignedToOSThread(ThreadID managedThreadId, DWORD osThreadId)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::ThreadNameChanged(ThreadID threadId, ULONG cchName, _In_reads_opt_(cchName) WCHAR name[])
{
	return S_OK;
}
#pragma endregion

#pragma region Remoting
HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::RemotingClientInvocationStarted(void)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::RemotingClientSendingMessage(GUID *pCookie, BOOL fIsAsync)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::RemotingClientReceivingReply(GUID *pCookie, BOOL fIsAsync)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::RemotingClientInvocationFinished(void)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::RemotingServerReceivingMessage(GUID *pCookie, BOOL fIsAsync)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::RemotingServerInvocationStarted(void)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::RemotingServerInvocationReturned(void)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::RemotingServerSendingReply(GUID *pCookie, BOOL fIsAsync)
{
	return S_OK;
}

#pragma endregion

#pragma region Managed Transitions
HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::UnmanagedToManagedTransition(FunctionID functionId, COR_PRF_TRANSITION_REASON reason)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::ManagedToUnmanagedTransition(FunctionID functionId, COR_PRF_TRANSITION_REASON reason)
{
	return S_OK;
}
#pragma endregion

#pragma region Runtime Suspensions
HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::RuntimeSuspendStarted(COR_PRF_SUSPEND_REASON suspendReason)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::RuntimeSuspendFinished(void)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::RuntimeSuspendAborted(void)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::RuntimeResumeStarted(void)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::RuntimeResumeFinished(void)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::RuntimeThreadSuspended(ThreadID threadId)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::RuntimeThreadResumed(ThreadID threadId)
{
	return S_OK;
}

#pragma endregion

#pragma region Object Allocation
HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::MovedReferences(ULONG cMovedObjectIDRanges, ObjectID oldObjectIDRangeStart[], ObjectID newObjectIDRangeStart[], ULONG cObjectIDRangeLength[])
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::ObjectAllocated(ObjectID objectId, ClassID classId)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::ObjectsAllocatedByClass(ULONG cClassCount, ClassID classIds[], ULONG cObjects[])
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::ObjectReferences(ObjectID objectId, ClassID classId, ULONG cObjectRefs, ObjectID objectRefIds[])
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::RootReferences(ULONG cRootRefs, ObjectID rootRefIds[])
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::SurvivingReferences(ULONG cSurvivingObjectIDRanges, ObjectID objectIDRangeStart[], ULONG cObjectIDRangeLength[])
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::MovedReferences2(ULONG cMovedObjectIDRanges, ObjectID oldObjectIDRangeStart[], ObjectID newObjectIDRangeStart[], SIZE_T cObjectIDRangeLength[])
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::SurvivingReferences2(ULONG cSurvivingObjectIDRanges, ObjectID objectIDRangeStart[], SIZE_T cObjectIDRangeLength[])
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::ConditionalWeakTableElementReferences(ULONG cRootRefs, ObjectID keyRefIds[], ObjectID valueRefIds[], GCHandleID rootIds[])
{
	return S_OK;
}


HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::RootReferences2(ULONG cRootRefs, ObjectID rootRefIds[], COR_PRF_GC_ROOT_KIND rootKinds[], COR_PRF_GC_ROOT_FLAGS rootFlags[], UINT_PTR rootIds[])
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::HandleCreated(GCHandleID handleId, ObjectID initialObjectId)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::HandleDestroyed(GCHandleID handleId)
{
	return S_OK;
}
#pragma endregion

#pragma region Exceptions
HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::ExceptionThrown(ObjectID thrownObjectId)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::ExceptionSearchFunctionEnter(FunctionID functionId)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::ExceptionSearchFunctionLeave(void)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::ExceptionSearchFilterEnter(FunctionID functionId)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::ExceptionSearchFilterLeave(void)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::ExceptionSearchCatcherFound(FunctionID functionId)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::ExceptionOSHandlerEnter(UINT_PTR __unused)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::ExceptionOSHandlerLeave(UINT_PTR __unused)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::ExceptionUnwindFunctionEnter(FunctionID functionId)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::ExceptionUnwindFunctionLeave(void)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::ExceptionUnwindFinallyEnter(FunctionID functionId)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::ExceptionUnwindFinallyLeave(void)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::ExceptionCatcherEnter(FunctionID functionId, ObjectID objectId)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::ExceptionCatcherLeave(void)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::COMClassicVTableCreated(ClassID wrappedClassId, REFGUID implementedIID, void *pVTable, ULONG cSlots)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::COMClassicVTableDestroyed(ClassID wrappedClassId, REFGUID implementedIID, void *pVTable)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::ExceptionCLRCatcherFound(void)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::ExceptionCLRCatcherExecute(void)
{
	return S_OK;
}
#pragma endregion

