#pragma once
#include "ICommand.h"
#pragma once
#include "FunctionInfo.h"
namespace Commands
{
	class FunctionEnterLong :
		public ICommand
	{
	public:
		FunctionEnterLong(FunctionInfo funcinfo);
		FunctionEnterLong::FunctionEnterLong(FunctionEnterLong&& funcinfo);
		FunctionEnterLong::FunctionEnterLong(FunctionEnterLong& funcinfo) = delete;
		~FunctionEnterLong();
		virtual std::shared_ptr<std::vector<char>> Encode();
		virtual std::shared_ptr<ICommand> Decode(std::shared_ptr<std::vector<char>> &data);
		virtual std::wstring Name();
		virtual std::wstring Description();

	private:
		FunctionInfo m_data;
		std::vector<char> m_internalvector;
		bool hasEncoded;
	};

}