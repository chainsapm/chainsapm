#include "stdafx.h"
#include "SendPackedStructure.h"

namespace Commands {
	SendPackedStructure::SendPackedStructure(PVOID RawStructure) : m_data(RawStructure), hasEncoded(false)
	{
	}

	SendPackedStructure::~SendPackedStructure() 
	{
	}

	std::wstring SendPackedStructure::Name()
	{
		return L"Function Enter (Quick)";
	}

	std::wstring SendPackedStructure::Description()
	{
		return L"Sends over an enter method, from the ELT calls.";
	}

	std::shared_ptr<std::vector<char>> SendPackedStructure::Encode()
	{


		if (!hasEncoded)
		{
#pragma warning(suppress : 4267) // I'm only sending max 4k of data in one command however, the length() prop is __int64. This is valid.
			
			hasEncoded = true;
			char* thisptr = (char*)m_data;
			std::vector<char> vecc;
			auto size = *(int*)thisptr; // grab the first 4 bytes
			vecc.assign(thisptr, thisptr+size);
			m_internalvector = std::make_shared <std::vector<char>>(vecc);
		}

		return m_internalvector;
	}

	std::shared_ptr<ICommand> SendPackedStructure::Decode(std::shared_ptr<std::vector<char>> &data)
	{
		return std::make_shared<SendPackedStructure>(SendPackedStructure(m_data));
	}
}