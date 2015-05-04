#include "stdafx.h"
#include "ICommand.h"

namespace Commands
{

	ICommand::ICommand() {}
	ICommand::~ICommand() {}
	std::shared_ptr<std::vector<char>> ICommand::Encode() { return std::make_shared<std::vector<char>>(std::vector<char>(0)); }
	std::shared_ptr<ICommand> ICommand::Decode(std::shared_ptr<std::vector<char>> &data){ return std::make_shared<ICommand>(ICommand()); }
	std::wstring ICommand::Name(){ return std::wstring(); }
	std::wstring ICommand::Description(){ return std::wstring(); }
	unsigned short ICommand::Code(){ return 0; }
}