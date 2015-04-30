#pragma once
#include "ICommand.h"

namespace Commands
{
	class GarbageCollectionFinished :
		public ICommand
	{
	public:
		GarbageCollectionFinished();
		~GarbageCollectionFinished();
	};
}
