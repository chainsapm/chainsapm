#pragma once
#include "ICommand.h"
namespace Commands {
	class SendPackedStructure :
		public virtual ICommand
	{
	public:
		SendPackedStructure(PVOID RawStructure);
		~SendPackedStructure();
		virtual std::shared_ptr<std::vector<char>> Encode();
		virtual std::shared_ptr<ICommand> Decode(std::shared_ptr<std::vector<char>> &data);
		virtual std::wstring Name();
		virtual std::wstring Description();
		virtual short Code() { return code; }

	private:
		PVOID m_data;
		std::shared_ptr<std::vector<char>> m_internalvector;
		bool hasEncoded;
		short code; 
	};

}