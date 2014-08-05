/************************************************************************/
/* CorProfilerCallbackImplementation.h                                */
/*                                                                      */
/* Implementation of all CorProfilerCallback methods. This is required  */
/* for the profiling interface to attach to each method. If these are   */
/* not implemented profiling will fail.                                 */
/************************************************************************/

#include <cor.h>
#include <corprof.h>

#pragma once
class CorProfilerCallbackImplementation
	: public ICorProfilerCallback5

{
public:
	CorProfilerCallbackImplementation();


	virtual HRESULT STDMETHODCALLTYPE ConditionalWeakTableElementReferences(ULONG cRootRefs, ObjectID keyRefIds[], ObjectID valueRefIds[], GCHandleID rootIds[]);

	virtual HRESULT STDMETHODCALLTYPE ReJITCompilationStarted(FunctionID functionId, ReJITID rejitId, BOOL fIsSafeToBlock);

	virtual HRESULT STDMETHODCALLTYPE GetReJITParameters(ModuleID moduleId, mdMethodDef methodId, ICorProfilerFunctionControl *pFunctionControl);

	virtual HRESULT STDMETHODCALLTYPE ReJITCompilationFinished(FunctionID functionId, ReJITID rejitId, HRESULT hrStatus, BOOL fIsSafeToBlock);

	virtual HRESULT STDMETHODCALLTYPE ReJITError(ModuleID moduleId, mdMethodDef methodId, FunctionID functionId, HRESULT hrStatus);

	virtual HRESULT STDMETHODCALLTYPE MovedReferences2(ULONG cMovedObjectIDRanges, ObjectID oldObjectIDRangeStart[], ObjectID newObjectIDRangeStart[], SIZE_T cObjectIDRangeLength[]);

	virtual HRESULT STDMETHODCALLTYPE SurvivingReferences2(ULONG cSurvivingObjectIDRanges, ObjectID objectIDRangeStart[], SIZE_T cObjectIDRangeLength[]);

	virtual HRESULT STDMETHODCALLTYPE InitializeForAttach(IUnknown *pCorProfilerInfoUnk, void *pvClientData, UINT cbClientData);

	virtual HRESULT STDMETHODCALLTYPE ProfilerAttachComplete(void);

	virtual HRESULT STDMETHODCALLTYPE ProfilerDetachSucceeded(void);

	virtual HRESULT STDMETHODCALLTYPE ThreadNameChanged(ThreadID threadId, ULONG cchName, _In_reads_opt_(cchName) WCHAR name[]);

	virtual HRESULT STDMETHODCALLTYPE GarbageCollectionStarted(int cGenerations, BOOL generationCollected[], COR_PRF_GC_REASON reason);

	virtual HRESULT STDMETHODCALLTYPE SurvivingReferences(ULONG cSurvivingObjectIDRanges, ObjectID objectIDRangeStart[], ULONG cObjectIDRangeLength[]);

	virtual HRESULT STDMETHODCALLTYPE GarbageCollectionFinished(void);

	virtual HRESULT STDMETHODCALLTYPE FinalizeableObjectQueued(DWORD finalizerFlags, ObjectID objectID);

	virtual HRESULT STDMETHODCALLTYPE RootReferences2(ULONG cRootRefs, ObjectID rootRefIds[], COR_PRF_GC_ROOT_KIND rootKinds[], COR_PRF_GC_ROOT_FLAGS rootFlags[], UINT_PTR rootIds[]);

	virtual HRESULT STDMETHODCALLTYPE HandleCreated(GCHandleID handleId, ObjectID initialObjectId);

	virtual HRESULT STDMETHODCALLTYPE HandleDestroyed(GCHandleID handleId);

	virtual HRESULT STDMETHODCALLTYPE Initialize(IUnknown *pICorProfilerInfoUnk);

	virtual HRESULT STDMETHODCALLTYPE Shutdown(void);

	virtual HRESULT STDMETHODCALLTYPE AppDomainCreationStarted(AppDomainID appDomainId);

	virtual HRESULT STDMETHODCALLTYPE AppDomainCreationFinished(AppDomainID appDomainId, HRESULT hrStatus);

	virtual HRESULT STDMETHODCALLTYPE AppDomainShutdownStarted(AppDomainID appDomainId);

	virtual HRESULT STDMETHODCALLTYPE AppDomainShutdownFinished(AppDomainID appDomainId, HRESULT hrStatus);

	virtual HRESULT STDMETHODCALLTYPE AssemblyLoadStarted(AssemblyID assemblyId);

	virtual HRESULT STDMETHODCALLTYPE AssemblyLoadFinished(AssemblyID assemblyId, HRESULT hrStatus);

	virtual HRESULT STDMETHODCALLTYPE AssemblyUnloadStarted(AssemblyID assemblyId);

	virtual HRESULT STDMETHODCALLTYPE AssemblyUnloadFinished(AssemblyID assemblyId, HRESULT hrStatus);

	virtual HRESULT STDMETHODCALLTYPE ModuleLoadStarted(ModuleID moduleId);

	virtual HRESULT STDMETHODCALLTYPE ModuleLoadFinished(ModuleID moduleId, HRESULT hrStatus);

	virtual HRESULT STDMETHODCALLTYPE ModuleUnloadStarted(ModuleID moduleId);

	virtual HRESULT STDMETHODCALLTYPE ModuleUnloadFinished(ModuleID moduleId, HRESULT hrStatus);

	virtual HRESULT STDMETHODCALLTYPE ModuleAttachedToAssembly(ModuleID moduleId, AssemblyID AssemblyId);

	virtual HRESULT STDMETHODCALLTYPE ClassLoadStarted(ClassID classId);

	virtual HRESULT STDMETHODCALLTYPE ClassLoadFinished(ClassID classId, HRESULT hrStatus);

	virtual HRESULT STDMETHODCALLTYPE ClassUnloadStarted(ClassID classId);

	virtual HRESULT STDMETHODCALLTYPE ClassUnloadFinished(ClassID classId, HRESULT hrStatus);

	virtual HRESULT STDMETHODCALLTYPE FunctionUnloadStarted(FunctionID functionId);

	virtual HRESULT STDMETHODCALLTYPE JITCompilationStarted(FunctionID functionId, BOOL fIsSafeToBlock);

	virtual HRESULT STDMETHODCALLTYPE JITCompilationFinished(FunctionID functionId, HRESULT hrStatus, BOOL fIsSafeToBlock);

	virtual HRESULT STDMETHODCALLTYPE JITCachedFunctionSearchStarted(FunctionID functionId, BOOL *pbUseCachedFunction);

	virtual HRESULT STDMETHODCALLTYPE JITCachedFunctionSearchFinished(FunctionID functionId, COR_PRF_JIT_CACHE result);

	virtual HRESULT STDMETHODCALLTYPE JITFunctionPitched(FunctionID functionId);

	virtual HRESULT STDMETHODCALLTYPE JITInlining(FunctionID callerId, FunctionID calleeId, BOOL *pfShouldInline);

	virtual HRESULT STDMETHODCALLTYPE ThreadCreated(ThreadID threadId);

	virtual HRESULT STDMETHODCALLTYPE ThreadDestroyed(ThreadID threadId);

	virtual HRESULT STDMETHODCALLTYPE ThreadAssignedToOSThread(ThreadID managedThreadId, DWORD osThreadId);

	virtual HRESULT STDMETHODCALLTYPE RemotingClientInvocationStarted(void);

	virtual HRESULT STDMETHODCALLTYPE RemotingClientSendingMessage(GUID *pCookie, BOOL fIsAsync);

	virtual HRESULT STDMETHODCALLTYPE RemotingClientReceivingReply(GUID *pCookie, BOOL fIsAsync);

	virtual HRESULT STDMETHODCALLTYPE RemotingClientInvocationFinished(void);

	virtual HRESULT STDMETHODCALLTYPE RemotingServerReceivingMessage(GUID *pCookie, BOOL fIsAsync);

	virtual HRESULT STDMETHODCALLTYPE RemotingServerInvocationStarted(void);

	virtual HRESULT STDMETHODCALLTYPE RemotingServerInvocationReturned(void);

	virtual HRESULT STDMETHODCALLTYPE RemotingServerSendingReply(GUID *pCookie, BOOL fIsAsync);

	virtual HRESULT STDMETHODCALLTYPE UnmanagedToManagedTransition(FunctionID functionId, COR_PRF_TRANSITION_REASON reason);

	virtual HRESULT STDMETHODCALLTYPE ManagedToUnmanagedTransition(FunctionID functionId, COR_PRF_TRANSITION_REASON reason);

	virtual HRESULT STDMETHODCALLTYPE RuntimeSuspendStarted(COR_PRF_SUSPEND_REASON suspendReason);

	virtual HRESULT STDMETHODCALLTYPE RuntimeSuspendFinished(void);

	virtual HRESULT STDMETHODCALLTYPE RuntimeSuspendAborted(void);

	virtual HRESULT STDMETHODCALLTYPE RuntimeResumeStarted(void);

	virtual HRESULT STDMETHODCALLTYPE RuntimeResumeFinished(void);

	virtual HRESULT STDMETHODCALLTYPE RuntimeThreadSuspended(ThreadID threadId);

	virtual HRESULT STDMETHODCALLTYPE RuntimeThreadResumed(ThreadID threadId);

	virtual HRESULT STDMETHODCALLTYPE MovedReferences(ULONG cMovedObjectIDRanges, ObjectID oldObjectIDRangeStart[], ObjectID newObjectIDRangeStart[], ULONG cObjectIDRangeLength[]);

	virtual HRESULT STDMETHODCALLTYPE ObjectAllocated(ObjectID objectId, ClassID classId);

	virtual HRESULT STDMETHODCALLTYPE ObjectsAllocatedByClass(ULONG cClassCount, ClassID classIds[], ULONG cObjects[]);

	virtual HRESULT STDMETHODCALLTYPE ObjectReferences(ObjectID objectId, ClassID classId, ULONG cObjectRefs, ObjectID objectRefIds[]);

	virtual HRESULT STDMETHODCALLTYPE RootReferences(ULONG cRootRefs, ObjectID rootRefIds[]);

	virtual HRESULT STDMETHODCALLTYPE ExceptionThrown(ObjectID thrownObjectId);

	virtual HRESULT STDMETHODCALLTYPE ExceptionSearchFunctionEnter(FunctionID functionId);

	virtual HRESULT STDMETHODCALLTYPE ExceptionSearchFunctionLeave(void);

	virtual HRESULT STDMETHODCALLTYPE ExceptionSearchFilterEnter(FunctionID functionId);

	virtual HRESULT STDMETHODCALLTYPE ExceptionSearchFilterLeave(void);

	virtual HRESULT STDMETHODCALLTYPE ExceptionSearchCatcherFound(FunctionID functionId);

	virtual HRESULT STDMETHODCALLTYPE ExceptionOSHandlerEnter(UINT_PTR __unused);

	virtual HRESULT STDMETHODCALLTYPE ExceptionOSHandlerLeave(UINT_PTR __unused);

	virtual HRESULT STDMETHODCALLTYPE ExceptionUnwindFunctionEnter(FunctionID functionId);

	virtual HRESULT STDMETHODCALLTYPE ExceptionUnwindFunctionLeave(void);

	virtual HRESULT STDMETHODCALLTYPE ExceptionUnwindFinallyEnter(FunctionID functionId);

	virtual HRESULT STDMETHODCALLTYPE ExceptionUnwindFinallyLeave(void);

	virtual HRESULT STDMETHODCALLTYPE ExceptionCatcherEnter(FunctionID functionId, ObjectID objectId);

	virtual HRESULT STDMETHODCALLTYPE ExceptionCatcherLeave(void);

	virtual HRESULT STDMETHODCALLTYPE COMClassicVTableCreated(ClassID wrappedClassId, REFGUID implementedIID, void *pVTable, ULONG cSlots);

	virtual HRESULT STDMETHODCALLTYPE COMClassicVTableDestroyed(ClassID wrappedClassId, REFGUID implementedIID, void *pVTable);

	virtual HRESULT STDMETHODCALLTYPE ExceptionCLRCatcherFound(void);

	virtual HRESULT STDMETHODCALLTYPE ExceptionCLRCatcherExecute(void);

protected:
	virtual ~CorProfilerCallbackImplementation();

};

