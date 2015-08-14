#pragma once
#include "ICommand.h"
namespace Commands
{
	class GetFunctionsToInstrument : public ICommand
	{
	public:
		GetFunctionsToInstrument();
		~GetFunctionsToInstrument();
		// Inherited via ICommand
		virtual std::shared_ptr<std::vector<char>> Encode() override;
		virtual std::shared_ptr<ICommand> Decode(std::shared_ptr<std::vector<char>>& data) override;
		virtual std::wstring Name() override;
		virtual std::wstring Description() override;
		virtual short Code() override;
	};
}