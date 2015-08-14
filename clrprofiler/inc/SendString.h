#pragma once
#include "ICommand.h"

namespace Commands
{
	class SendString :
		public virtual ICommand
	{
	public:
		SendString(std::wstring& data);
		SendString(std::wstring&& data);
		SendString(std::string& data);
		~SendString();
		virtual std::shared_ptr<std::vector<char>> Encode();
		virtual std::shared_ptr<ICommand> Decode(std::shared_ptr<std::vector<char>> &data);
		virtual std::wstring Name();
		virtual std::wstring Description();
		virtual short Code() { return code; }
		std::wstring m_wstring;
		std::string m_string;
	private:
		std::shared_ptr<std::vector<char>> m_internalvector;
		bool hasEncoded;
		bool wchar = false;
		short code;
	};

}
