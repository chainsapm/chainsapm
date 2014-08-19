using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace aggregator.NetworkPackets.StackItems
{

    enum StackItemType
    {
        Thread,
        Function,
        RuntimeSuspension,
        GCSuspension,
        Exception,
        Transition
    }

    enum StackReason
    {
        ENTER,
        EXIT,
        TAIL,
        THREAD_START,
        THREAD_END
    }
    [Serializable]
    class StackItem : IBaseItem
    {

        UInt64 ThreadID;
        StackItem Type;
        UInt64 EnterTime;
        UInt64 LeaveTime;
        UInt64 GarbageCollectionTotal;
        UInt64 SuspensionTotal;
        UInt64 ProfilingOverheadTotal;

    }

    [Serializable]
    class FunctionStackItem : StackItem
    {
        UInt64 FunctionID;
        byte ParameterCount;
        ItemStackParameter ReturnValue;
        ItemStackParameter[] ItemStackParameters;
    }

    [Serializable]
    struct ItemStackParameter
    {
        byte ParameterNumber;
        Int64 ParameterValueNumeric;
        string ParameterValueString;
    }

    [Serializable]
    class ThreadStackItem : StackItem
    {
        string ThreadName;
    }

    [Serializable]
    class RuntimeSuspensionStackItem : StackItem
    {
        COR_PRF_SUSPEND_REASON SuspensionReason;
    }

    [Serializable]
    class GarbageCollectionStackItem : StackItem
    {
        GC_REASON GCReason;
    }

    [Serializable]
    struct GC_REASON
    {
        byte MaximumGenerationCollected;
        COR_PRF_GC_REASON GCReason;
    }
}
    [Serializable]
    enum COR_PRF_SUSPEND_REASON
    {
        COR_PRF_SUSPEND_OTHER = 0,
        COR_PRF_SUSPEND_FOR_GC = 1,
        COR_PRF_SUSPEND_FOR_APPDOMAIN_SHUTDOWN = 2,
        COR_PRF_SUSPEND_FOR_CODE_PITCHING = 3,
        COR_PRF_SUSPEND_FOR_SHUTDOWN = 4,
        COR_PRF_SUSPEND_FOR_INPROC_DEBUGGER = 6,
        COR_PRF_SUSPEND_FOR_GC_PREP = 7,
        COR_PRF_SUSPEND_FOR_REJIT = 8
    }

    [Serializable]
    enum COR_PRF_GC_GENERATION
    {
        COR_PRF_GC_GEN_0 = 0,
        COR_PRF_GC_GEN_1 = 1,
        COR_PRF_GC_GEN_2 = 2,
        COR_PRF_GC_LARGE_OBJECT_HEAP = 3
    }

    [Serializable]
    enum COR_PRF_GC_REASON
    {
        COR_PRF_GC_INDUCED = 1,
        COR_PRF_GC_OTHER = 0
    }


