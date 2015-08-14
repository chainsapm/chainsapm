#pragma once
#include "stdafx.h"

namespace Commands
{
	class ICommand
	{
	public:
		virtual ~ICommand() {};
		virtual std::shared_ptr<std::vector<char>> Encode() = 0;
		virtual std::shared_ptr<ICommand> Decode(std::shared_ptr<std::vector<char>> &data) = 0;
		virtual std::wstring Name() = 0;
		virtual std::wstring Description() = 0;
		virtual short Code() = 0;
	};

}
