#pragma once
#include "ICommand.h"
namespace Commands
{
	class GetString :
		public ICommand
	{
	public:
		GetString();
		~GetString();
	};

}