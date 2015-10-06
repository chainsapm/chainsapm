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
	UNREFERENCED_PARAMETER(pICorProfilerInfoUnk);
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::Shutdown(void)
{

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::InitializeForAttach(IUnknown *pCorProfilerInfoUnk, void *pvClientData, UINT cbClientData)
{
	UNREFERENCED_PARAMETER(pCorProfilerInfoUnk);
	UNREFERENCED_PARAMETER(pvClientData);
	UNREFERENCED_PARAMETER(cbClientData);
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
	UNREFERENCED_PARAMETER(cGenerations);
	UNREFERENCED_PARAMETER(generationCollected);
	UNREFERENCED_PARAMETER(reason);
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::GarbageCollectionFinished(void)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::FinalizeableObjectQueued(DWORD finalizerFlags, ObjectID objectID)
{
	UNREFERENCED_PARAMETER(finalizerFlags);
	UNREFERENCED_PARAMETER(objectID);
	return S_OK;
}
#pragma endregion

#pragma region AppDomain
HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::AppDomainCreationStarted(AppDomainID appDomainId)
{
	UNREFERENCED_PARAMETER(appDomainId);
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::AppDomainCreationFinished(AppDomainID appDomainId, HRESULT hrStatus)
{
	UNREFERENCED_PARAMETER(appDomainId);
	UNREFERENCED_PARAMETER(hrStatus);
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::AppDomainShutdownStarted(AppDomainID appDomainId)
{
	UNREFERENCED_PARAMETER(appDomainId);
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::AppDomainShutdownFinished(AppDomainID appDomainId, HRESULT hrStatus)
{
	UNREFERENCED_PARAMETER(appDomainId);
	UNREFERENCED_PARAMETER(hrStatus);
	return S_OK;
}
#pragma endregion

#pragma region Loading and Unloading

#pragma region Assembly Loading

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::AssemblyLoadStarted(AssemblyID assemblyId)
{
	UNREFERENCED_PARAMETER(assemblyId);
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::AssemblyLoadFinished(AssemblyID assemblyId, HRESULT hrStatus)
{
	UNREFERENCED_PARAMETER(assemblyId);
	UNREFERENCED_PARAMETER(hrStatus);
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::AssemblyUnloadStarted(AssemblyID assemblyId)
{
	UNREFERENCED_PARAMETER(assemblyId);
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::AssemblyUnloadFinished(AssemblyID assemblyId, HRESULT hrStatus)
{
	UNREFERENCED_PARAMETER(assemblyId);
	UNREFERENCED_PARAMETER(hrStatus);
	return S_OK;
}
#pragma endregion

#pragma region Module Loading
HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::ModuleLoadStarted(ModuleID moduleId)
{
	UNREFERENCED_PARAMETER(moduleId);
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::ModuleLoadFinished(ModuleID moduleId, HRESULT hrStatus)
{
	UNREFERENCED_PARAMETER(moduleId);
	UNREFERENCED_PARAMETER(hrStatus);
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::ModuleUnloadStarted(ModuleID moduleId)
{
	UNREFERENCED_PARAMETER(moduleId);
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::ModuleUnloadFinished(ModuleID moduleId, HRESULT hrStatus)
{
	UNREFERENCED_PARAMETER(moduleId);
	UNREFERENCED_PARAMETER(hrStatus);
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::ModuleAttachedToAssembly(ModuleID moduleId, AssemblyID AssemblyId)
{
	UNREFERENCED_PARAMETER(moduleId);
	UNREFERENCED_PARAMETER(AssemblyId);
	return S_OK;
}
#pragma endregion

#pragma region Class Loading
HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::ClassLoadStarted(ClassID classId)
{
	UNREFERENCED_PARAMETER(classId);
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::ClassLoadFinished(ClassID classId, HRESULT hrStatus)
{
	UNREFERENCED_PARAMETER(classId);
	UNREFERENCED_PARAMETER(hrStatus);
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::ClassUnloadStarted(ClassID classId)
{
	UNREFERENCED_PARAMETER(classId);
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::ClassUnloadFinished(ClassID classId, HRESULT hrStatus)
{
	UNREFERENCED_PARAMETER(classId);
	UNREFERENCED_PARAMETER(hrStatus);
	return S_OK;
}
#pragma endregion

#pragma region Function Unloading
HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::FunctionUnloadStarted(FunctionID functionId)
{
	UNREFERENCED_PARAMETER(functionId);
	return S_OK;
}

#pragma endregion

#pragma endregion

#pragma region JITing
HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::JITCompilationStarted(FunctionID functionId, BOOL fIsSafeToBlock)
{
	UNREFERENCED_PARAMETER(functionId);
	UNREFERENCED_PARAMETER(fIsSafeToBlock);
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::JITCompilationFinished(FunctionID functionId, HRESULT hrStatus, BOOL fIsSafeToBlock)
{
	UNREFERENCED_PARAMETER(functionId);
	UNREFERENCED_PARAMETER(hrStatus);
	UNREFERENCED_PARAMETER(fIsSafeToBlock);
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::JITCachedFunctionSearchStarted(FunctionID functionId, BOOL *pbUseCachedFunction)
{
	UNREFERENCED_PARAMETER(functionId);
	UNREFERENCED_PARAMETER(pbUseCachedFunction);
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::JITCachedFunctionSearchFinished(FunctionID functionId, COR_PRF_JIT_CACHE result)
{
	UNREFERENCED_PARAMETER(functionId);
	UNREFERENCED_PARAMETER(result);
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::JITFunctionPitched(FunctionID functionId)
{
	UNREFERENCED_PARAMETER(functionId);
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::JITInlining(FunctionID callerId, FunctionID calleeId, BOOL *pfShouldInline)
{
	UNREFERENCED_PARAMETER(callerId);
	UNREFERENCED_PARAMETER(calleeId);
	UNREFERENCED_PARAMETER(pfShouldInline);
	return S_OK;
}
#pragma endregion

#pragma region ReJIT
HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::ReJITCompilationStarted(FunctionID functionId, ReJITID rejitId, BOOL fIsSafeToBlock)
{
	UNREFERENCED_PARAMETER(functionId);
	UNREFERENCED_PARAMETER(rejitId);
	UNREFERENCED_PARAMETER(fIsSafeToBlock);
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::GetReJITParameters(ModuleID moduleId, mdMethodDef methodId, ICorProfilerFunctionControl *pFunctionControl)
{
	UNREFERENCED_PARAMETER(moduleId);
	UNREFERENCED_PARAMETER(methodId);
	UNREFERENCED_PARAMETER(pFunctionControl);
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::ReJITCompilationFinished(FunctionID functionId, ReJITID rejitId, HRESULT hrStatus, BOOL fIsSafeToBlock)
{
	UNREFERENCED_PARAMETER(functionId);
	UNREFERENCED_PARAMETER(rejitId);
	UNREFERENCED_PARAMETER(hrStatus);
	UNREFERENCED_PARAMETER(fIsSafeToBlock);
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::ReJITError(ModuleID moduleId, mdMethodDef methodId, FunctionID functionId, HRESULT hrStatus)
{
	UNREFERENCED_PARAMETER(moduleId);
	UNREFERENCED_PARAMETER(methodId);
	UNREFERENCED_PARAMETER(functionId);
	UNREFERENCED_PARAMETER(hrStatus);
	return S_OK;
}
#pragma endregion

#pragma region Threads
HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::ThreadCreated(ThreadID threadId)
{
	UNREFERENCED_PARAMETER(threadId);
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::ThreadDestroyed(ThreadID threadId)
{
	UNREFERENCED_PARAMETER(threadId);
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::ThreadAssignedToOSThread(ThreadID managedThreadId, DWORD osThreadId)
{
	UNREFERENCED_PARAMETER(managedThreadId);
	UNREFERENCED_PARAMETER(osThreadId);
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::ThreadNameChanged(ThreadID threadId, ULONG cchName, _In_reads_opt_(cchName) WCHAR name[])
{
	UNREFERENCED_PARAMETER(threadId);
	UNREFERENCED_PARAMETER(cchName);
	UNREFERENCED_PARAMETER(name);
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
	UNREFERENCED_PARAMETER(pCookie);
	UNREFERENCED_PARAMETER(fIsAsync);
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::RemotingClientReceivingReply(GUID *pCookie, BOOL fIsAsync)
{
	UNREFERENCED_PARAMETER(pCookie);
	UNREFERENCED_PARAMETER(fIsAsync);
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::RemotingClientInvocationFinished(void)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::RemotingServerReceivingMessage(GUID *pCookie, BOOL fIsAsync)
{
	UNREFERENCED_PARAMETER(pCookie);
	UNREFERENCED_PARAMETER(fIsAsync);
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
	UNREFERENCED_PARAMETER(pCookie);
	UNREFERENCED_PARAMETER(fIsAsync);
	return S_OK;
}

#pragma endregion

#pragma region Managed Transitions
HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::UnmanagedToManagedTransition(FunctionID functionId, COR_PRF_TRANSITION_REASON reason)
{
	UNREFERENCED_PARAMETER(functionId);
	UNREFERENCED_PARAMETER(reason);
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::ManagedToUnmanagedTransition(FunctionID functionId, COR_PRF_TRANSITION_REASON reason)
{
	UNREFERENCED_PARAMETER(functionId);
	UNREFERENCED_PARAMETER(reason);
	return S_OK;
}
#pragma endregion

#pragma region Runtime Suspensions
HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::RuntimeSuspendStarted(COR_PRF_SUSPEND_REASON suspendReason)
{
	UNREFERENCED_PARAMETER(suspendReason);
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
	UNREFERENCED_PARAMETER(threadId);
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::RuntimeThreadResumed(ThreadID threadId)
{
	UNREFERENCED_PARAMETER(threadId);
	return S_OK;
}

#pragma endregion

#pragma region Object Allocation
HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::MovedReferences(ULONG cMovedObjectIDRanges, ObjectID oldObjectIDRangeStart[], ObjectID newObjectIDRangeStart[], ULONG cObjectIDRangeLength[])
{
	UNREFERENCED_PARAMETER(cMovedObjectIDRanges);
	UNREFERENCED_PARAMETER(oldObjectIDRangeStart);
	UNREFERENCED_PARAMETER(newObjectIDRangeStart);
	UNREFERENCED_PARAMETER(cObjectIDRangeLength);
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::ObjectAllocated(ObjectID objectId, ClassID classId)
{
	UNREFERENCED_PARAMETER(objectId);
	UNREFERENCED_PARAMETER(classId);
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::ObjectsAllocatedByClass(ULONG cClassCount, ClassID classIds[], ULONG cObjects[])
{
	UNREFERENCED_PARAMETER(cClassCount);
	UNREFERENCED_PARAMETER(classIds);
	UNREFERENCED_PARAMETER(cObjects);
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::ObjectReferences(ObjectID objectId, ClassID classId, ULONG cObjectRefs, ObjectID objectRefIds[])
{
	UNREFERENCED_PARAMETER(objectId);
	UNREFERENCED_PARAMETER(classId);
	UNREFERENCED_PARAMETER(cObjectRefs);
	UNREFERENCED_PARAMETER(objectRefIds);
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::RootReferences(ULONG cRootRefs, ObjectID rootRefIds[])
{
	UNREFERENCED_PARAMETER(cRootRefs);
	UNREFERENCED_PARAMETER(rootRefIds);
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::SurvivingReferences(ULONG cSurvivingObjectIDRanges, ObjectID objectIDRangeStart[], ULONG cObjectIDRangeLength[])
{
	UNREFERENCED_PARAMETER(cSurvivingObjectIDRanges);
	UNREFERENCED_PARAMETER(objectIDRangeStart);
	UNREFERENCED_PARAMETER(cObjectIDRangeLength);
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::MovedReferences2(ULONG cMovedObjectIDRanges, ObjectID oldObjectIDRangeStart[], ObjectID newObjectIDRangeStart[], SIZE_T cObjectIDRangeLength[])
{
	UNREFERENCED_PARAMETER(cMovedObjectIDRanges);
	UNREFERENCED_PARAMETER(oldObjectIDRangeStart);
	UNREFERENCED_PARAMETER(newObjectIDRangeStart);
	UNREFERENCED_PARAMETER(cObjectIDRangeLength);
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::SurvivingReferences2(ULONG cSurvivingObjectIDRanges, ObjectID objectIDRangeStart[], SIZE_T cObjectIDRangeLength[])
{
	UNREFERENCED_PARAMETER(cSurvivingObjectIDRanges);
	UNREFERENCED_PARAMETER(objectIDRangeStart);
	UNREFERENCED_PARAMETER(cObjectIDRangeLength);
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::ConditionalWeakTableElementReferences(ULONG cRootRefs, ObjectID keyRefIds[], ObjectID valueRefIds[], GCHandleID rootIds[])
{
	UNREFERENCED_PARAMETER(cRootRefs);
	UNREFERENCED_PARAMETER(keyRefIds);
	UNREFERENCED_PARAMETER(valueRefIds);
	UNREFERENCED_PARAMETER(rootIds);
	return S_OK;
}


HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::RootReferences2(ULONG cRootRefs, ObjectID rootRefIds[], COR_PRF_GC_ROOT_KIND rootKinds[], COR_PRF_GC_ROOT_FLAGS rootFlags[], UINT_PTR rootIds[])
{
	UNREFERENCED_PARAMETER(cRootRefs);
	UNREFERENCED_PARAMETER(rootRefIds);
	UNREFERENCED_PARAMETER(rootKinds);
	UNREFERENCED_PARAMETER(rootFlags);
	UNREFERENCED_PARAMETER(rootIds);
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::HandleCreated(GCHandleID handleId, ObjectID initialObjectId)
{
	UNREFERENCED_PARAMETER(handleId);
	UNREFERENCED_PARAMETER(initialObjectId);
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::HandleDestroyed(GCHandleID handleId)
{
	UNREFERENCED_PARAMETER(handleId);
	return S_OK;
}
#pragma endregion

#pragma region Exceptions
HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::ExceptionThrown(ObjectID thrownObjectId)
{
	UNREFERENCED_PARAMETER(thrownObjectId);

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::ExceptionSearchFunctionEnter(FunctionID functionId)
{
	UNREFERENCED_PARAMETER(functionId);
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::ExceptionSearchFunctionLeave(void)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::ExceptionSearchFilterEnter(FunctionID functionId)
{
	UNREFERENCED_PARAMETER(functionId);
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::ExceptionSearchFilterLeave(void)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::ExceptionSearchCatcherFound(FunctionID functionId)
{
	UNREFERENCED_PARAMETER(functionId);
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::ExceptionOSHandlerEnter(UINT_PTR __unused)
{
	UNREFERENCED_PARAMETER(__unused);
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::ExceptionOSHandlerLeave(UINT_PTR __unused)
{
	UNREFERENCED_PARAMETER(__unused);
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::ExceptionUnwindFunctionEnter(FunctionID functionId)
{
	UNREFERENCED_PARAMETER(functionId);
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::ExceptionUnwindFunctionLeave(void)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::ExceptionUnwindFinallyEnter(FunctionID functionId)
{
	UNREFERENCED_PARAMETER(functionId);
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::ExceptionUnwindFinallyLeave(void)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::ExceptionCatcherEnter(FunctionID functionId, ObjectID objectId)
{
	UNREFERENCED_PARAMETER(functionId);
	UNREFERENCED_PARAMETER(objectId);
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::ExceptionCatcherLeave(void)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::COMClassicVTableCreated(ClassID wrappedClassId, REFGUID implementedIID, void *pVTable, ULONG cSlots)
{
	UNREFERENCED_PARAMETER(wrappedClassId);
	UNREFERENCED_PARAMETER(implementedIID);
	UNREFERENCED_PARAMETER(pVTable);
	UNREFERENCED_PARAMETER(cSlots);
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CorProfilerCallbackImplementation::COMClassicVTableDestroyed(ClassID wrappedClassId, REFGUID implementedIID, void *pVTable)
{
	UNREFERENCED_PARAMETER(wrappedClassId);
	UNREFERENCED_PARAMETER(implementedIID);
	UNREFERENCED_PARAMETER(pVTable);
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

