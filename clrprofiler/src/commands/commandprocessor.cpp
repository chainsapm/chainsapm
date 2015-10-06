#include "stdafx.h"
#include "commands\commandprocessor.h"
#include "profilermain.h"
#include "ItemMapping.h"

CommandProcessor::CommandProcessor()
{
}

CommandProcessor::CommandProcessor(Cprofilermain *profiler)
{
	m_profiler = profiler;

}

void CommandProcessor::Process(std::shared_ptr<Commands::ACK> mti)
{
}

void CommandProcessor::Process(std::shared_ptr<Commands::DefineInstrumentationMethods> cmd)
{
	if (cmd != nullptr)
	{
		auto newMapping = ItemMapping();
		newMapping.FunctionName = cmd->MethodName;
		newMapping.ClassName = cmd->TypeNameString;
		newMapping.Match = ItemMapping::MatchType::FunctionOnly;
		m_profiler->m_Container->g_FullyQualifiedMethodsToProfile->insert(newMapping);
		if (cmd->MethodName.find(L"ENDOFMETHODS"))
		{
			SetEvent(m_profiler->ReceievedMethodsToInstrument); // Signal that we've added the events
			SetEvent(m_profiler->ReceievedILForInjection); // Signal that we've added the events
		}
	}

}

void CommandProcessor::Process(std::shared_ptr<Commands::SendInjectionMetadata> cmd)
{
	m_profiler->m_InjectedMethodHelper = std::make_shared<ModuleMetadataHelpers>(cmd->InjectionMetadata);
	SetEvent(m_profiler->ReceievedMetaDataForInjection); // Signal that we've added the events
}

//