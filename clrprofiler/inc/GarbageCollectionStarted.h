#pragma once
#include "ICommand.h"

namespace Commands
{
	class GarbageCollectionStarted :
		public ICommand
	{
	public:
		GarbageCollectionStarted();
		~GarbageCollectionStarted();
	};

}