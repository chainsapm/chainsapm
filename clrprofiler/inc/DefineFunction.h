#pragma once
#include "ICommand.h"
namespace Commands {
	class DefineFunction :
		public virtual ICommand
	{
	public:
		DefineFunction();
		DefineFunction(FunctionID funcId, ClassID classId, const std::wstring& data, __int64 timestamp);
		~DefineFunction();
		virtual std::shared_ptr<std::vector<char>> Encode();
		virtual std::shared_ptr<ICommand> Decode(std::shared_ptr<std::vector<char>> &data);
		virtual std::wstring Name();
		virtual std::wstring Description();
		virtual short Code() { return code; }

	private:
		std::shared_ptr<std::vector<char>> m_internalvector;
		bool hasEncoded;
		bool wchar;
		short code;
		std::wstring* m_wstring;
		std::string* m_string;
		__int64 m_funcId; // Always use a 64bit so the message doesn't change
		__int64 m_classId; // Always use a 64bit so the message doesn't change
		__int64 m_timestamp; // Always use a 64bit so the message doesn't change
		
	};

}
