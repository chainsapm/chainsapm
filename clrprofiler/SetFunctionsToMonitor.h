#pragma once
#include "ICommand.h"
namespace Commands {
	class SetFunctionsToMonitor :
		public ICommand
	{
	public:
		SetFunctionsToMonitor();
		~SetFunctionsToMonitor();
	};

}