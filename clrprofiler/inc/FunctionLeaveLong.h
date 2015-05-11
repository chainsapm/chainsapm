#pragma once
#include "ICommand.h"
namespace Commands
{
	class FunctionLeaveLong :
		public virtual ICommand
	{
	public:
		FunctionLeaveLong();
		~FunctionLeaveLong();
	};


}