#pragma once
#include "ICommand.h"

namespace Commands
{
	class DefineClass :
		public virtual ICommand
	{
	public:
		DefineClass(std::wstring& data);
		DefineClass(std::string& data);
		~DefineClass();
		virtual std::shared_ptr<std::vector<char>> Encode();
		virtual std::shared_ptr<ICommand> Decode(std::shared_ptr<std::vector<char>> &data);
		virtual std::wstring Name();
		virtual std::wstring Description();
		virtual short Code() { return code; }

	private:
		std::wstring m_wstring;
		std::string m_string;
		std::shared_ptr<std::vector<char>> m_internalvector;
		bool hasEncoded;
		bool wchar = false;
		short code;
	};

}
