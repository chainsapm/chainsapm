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



void CommandProcessor::Process(std::shared_ptr<Commands::SendInjectionMetadata> cmd)
{
	m_profiler->m_InjectedMethodHelper = std::make_shared<ModuleMetadataHelpers>(cmd->InjectionMetadata);
	m_profiler->m_InjectedMethodIL = cmd->InjectionIL;
	SetEvent(m_profiler->ReceievedMetaDataForInjection); // Signal that we've added the events
}

//