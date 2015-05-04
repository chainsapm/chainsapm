#pragma once
#include "ICommand.h"

namespace Commands
{
	class FunctionEnterQuick :
		public ICommand
	{
	public:
		FunctionEnterQuick(FunctionID data, ThreadID threadid);
		~FunctionEnterQuick();
		virtual std::shared_ptr<std::vector<char>> Encode();
		virtual std::shared_ptr<ICommand> Decode(std::shared_ptr<std::vector<char>> &data);
		virtual std::wstring Name();
		virtual std::wstring Description();

	private:
		__int64 function;
		__int64 thread;
		short code;
		std::shared_ptr<std::vector<char>> m_internalvector;
		bool hasEncoded;
	};

}
