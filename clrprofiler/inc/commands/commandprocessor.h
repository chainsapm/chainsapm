#pragma once
#include "Commands.h"

class Cprofilermain;
class CommandProcessor
{
public:
	CommandProcessor();
	CommandProcessor(Cprofilermain *profiler);
	void Process(std::shared_ptr<Commands::ACK> mti);
	void Process(std::shared_ptr<Commands::SendInjectionMetadata> cmd);
private:
	Cprofilermain *m_profiler;
};