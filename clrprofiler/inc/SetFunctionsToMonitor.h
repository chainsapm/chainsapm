#pragma once
#include "ICommand.h"
namespace Commands {
	class SetFunctionsToMonitor :
		public virtual ICommand
	{
	public:
		SetFunctionsToMonitor();
		~SetFunctionsToMonitor();
	};

}