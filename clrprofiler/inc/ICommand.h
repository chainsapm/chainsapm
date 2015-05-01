#pragma once
#include "stdafx.h"

namespace Commands
{
	class ICommand
	{
	public:
		ICommand();
		virtual ~ICommand();
		virtual std::shared_ptr<std::vector<char>> Encode();
		virtual std::shared_ptr<ICommand> Decode(std::shared_ptr<std::vector<char>> &data);
		virtual std::wstring Name();
		virtual std::wstring Description();
		virtual unsigned short Code();
	};

}
