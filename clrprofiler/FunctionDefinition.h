#pragma once
#include "ICommand.h"
#pragma once
#include "FunctionInfo.h"
namespace Commands
{
	class FunctionDefinition :
		public ICommand
	{
	public:
		FunctionDefinition(FunctionInfo funcinfo);
		FunctionDefinition::FunctionDefinition(FunctionDefinition&& funcinfo);
		FunctionDefinition::FunctionDefinition(FunctionDefinition& funcinfo) = delete;
		~FunctionDefinition();
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