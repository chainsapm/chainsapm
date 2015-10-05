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

void CommandProcessor::Process(std::shared_ptr<Commands::MethodsToInstrument> cmd)
{
	if (cmd != nullptr && cmd->MethodList.size() == cmd->MethodPropList.size() && cmd->MethodList.size() == cmd->MethodClassList.size())
	{
		for (size_t i = 0; i < cmd->MethodList.size(); i++)
		{
			auto newMapping = ItemMapping();
			newMapping.FunctionName = cmd->MethodList[i];
			newMapping.ClassName = cmd->MethodClassList[i];
			newMapping.Match = ItemMapping::MatchType::FunctionOnly;
			m_profiler->m_Container->g_FullyQualifiedMethodsToProfile->insert(newMapping);
		}
	}
	SetEvent(m_profiler->ReceievedMethodsToInstrument); // Signal that we've added the events
}
