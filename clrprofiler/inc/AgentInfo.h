#pragma once
namespace InformationClasses {
	class AgentInfo
	{
	public:
		AgentInfo();
		~AgentInfo();

	private:
		enum class _AgentCapabilities
		{
			PROFILE = 0x1,
			FILE_TRANSFER = 0x2,
			PERFORMANCE_COUNTERS = 0x4,
			AGENT_REGISTRY = 0x8,
			FULL_REGISTRY = 0x10,
			RESERVED = 0x20,
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

		std::wstring AgentName;
		std::hash<std::wstring> AgentHash;

		unsigned char AgentMajorVersion;
		unsigned char AgentMinorVersion;
		unsigned char AgentIncrementalVersion;
		
	};

}