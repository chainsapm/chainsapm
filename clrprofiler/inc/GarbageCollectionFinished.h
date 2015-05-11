#pragma once
#include "ICommand.h"

namespace Commands
{
	class GarbageCollectionFinished :
		public virtual ICommand
	{
	public:
		GarbageCollectionFinished();
		~GarbageCollectionFinished();
	};
}
