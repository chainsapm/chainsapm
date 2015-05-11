#pragma once
#include "ICommand.h"
namespace Commands
{
	class GetString :
		public virtual ICommand
	{
	public:
		GetString();
		~GetString();
	};

}