#include "stdafx.h"
#include "GetFunctionsToInstrument.h"
#include "..\..\inc\GetFunctionsToInstrument.h"

namespace Commands
{
	GetFunctionsToInstrument::GetFunctionsToInstrument()
	{
	}

	GetFunctionsToInstrument::~GetFunctionsToInstrument()
	{
	}
	std::shared_ptr<std::vector<char>> GetFunctionsToInstrument::Encode()
	{
		return std::shared_ptr<std::vector<char>>();
	}
	std::shared_ptr<ICommand> GetFunctionsToInstrument::Decode(std::shared_ptr<std::vector<char>>& data)
	{
		return std::shared_ptr<ICommand>();
	}
	std::wstring GetFunctionsToInstrument::Name()
	{
		return std::wstring();
	}
	std::wstring GetFunctionsToInstrument::Description()
	{
		return std::wstring();
	}
	short GetFunctionsToInstrument::Code()
	{
		return 0;
	}
}