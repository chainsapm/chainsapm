#pragma once
#include "ICommand.h"

namespace Commands
{
	class FunctionEnterQuick :
		public ICommand
	{
	public:
		FunctionEnterQuick(FunctionID data);
		~FunctionEnterQuick();
		virtual std::shared_ptr<std::vector<char>> Encode();
		virtual std::shared_ptr<ICommand> Decode(std::shared_ptr<std::vector<char>> &data);
		virtual std::wstring Name();
		virtual std::wstring Description();

	private:
		FunctionID function;
		std::vector<char> m_internalvector;
		bool hasEncoded;
	};

}
