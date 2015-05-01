#pragma once
#include "ICommand.h"
namespace Commands {
	class ModuleDefinition :
		public ICommand
	{
	public:
		ModuleDefinition();
		~ModuleDefinition();
	};

}