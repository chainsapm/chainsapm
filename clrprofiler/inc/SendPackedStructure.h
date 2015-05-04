#pragma once
#include "ICommand.h"
namespace Commands {
	class SendPackedStructure :
		public ICommand
	{
	public:
		SendPackedStructure(UINT_PTR RawStructure);
		~SendPackedStructure();
		virtual std::shared_ptr<std::vector<char>> Encode();
		virtual std::shared_ptr<ICommand> Decode(std::shared_ptr<std::vector<char>> &data);
		virtual std::wstring Name();
		virtual std::wstring Description();

	private:
		UINT_PTR m_data;
		std::shared_ptr<std::vector<char>> m_internalvector;
		bool hasEncoded;
	};

}