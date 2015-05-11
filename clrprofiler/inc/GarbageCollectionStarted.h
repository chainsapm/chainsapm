#pragma once
#include "ICommand.h"

namespace Commands
{
	class GarbageCollectionStarted :
		public virtual ICommand
	{
	public:
		GarbageCollectionStarted();
		~GarbageCollectionStarted();
	};

}