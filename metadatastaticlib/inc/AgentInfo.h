#pragma once

namespace InformationClasses {
#pragma pack(1)
	struct AgentInfo
	{
		AgentInfo()
		{
			Code = 0X5;
		}
		int Length;
		short Code;
		enum class Capabilities
		{
			PROFILE = 0x1, // Can attach to CLR profiler
			FILE_TRANSFER = 0x2, // Can send files
			PERFORMANCE_COUNTERS = 0x4, // Can access perf counters
			AGENT_REGISTRY = 0x8, // Can modify the agent definitions
			FULL_REGISTRY = 0x10, // Can modify any registry
			IL_REWRITE = 0x20, // Can modify .NET code before it's JITted
			RESERVED0 = 0x40,
			RESERVED1 = 0x80,
			RESERVED2 = 0x100,
			RESERVED3 = 0x200,
			RESERVED4 = 0x400,
			RESERVED5 = 0x800,
			RESERVED6 = 0x1000,
			RESERVED7 = 0x2000,
			RESERVED8 = 0x4000,
			RESERVED9 = 0x8000
		} AgentCapabilities;
		__int8 AgentMajorVersion;
		__int8 AgentMinorVersion;
		__int8 AgentIncrementalVersion;
		__int32 MachineNameLen;
		__int32 MachineNameHash;
		wchar_t MachineName[16];
		__int32 AgentNameLen;
		__int32 AgentHash;
		wchar_t AgentName[0];
	};

}