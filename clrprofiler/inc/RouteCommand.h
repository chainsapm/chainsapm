#pragma once
#include "ICommand.h"
namespace Commands
{

	class RouteCommand :
		public ICommand
	{
	public:
		RouteCommand(Commands::ICommand);
		~RouteCommand();
		std::shared_ptr<std::vector<char>> RouteCommand::Encode();

	};


}