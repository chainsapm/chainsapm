#include "stdafx.h"
#include "FunctionLeaveQuick.h"


namespace Commands
{
	FunctionLeaveQuick::FunctionLeaveQuick(FunctionID data) : function(data), hasEncoded(false)
	{
	}

	FunctionLeaveQuick::~FunctionLeaveQuick()
	{
	}

	std::wstring FunctionLeaveQuick::Name()
	{
		return L"Function Enter (Quick)";
	}

	std::wstring FunctionLeaveQuick::Description()
	{
		return L"Sends over an enter method, from the ELT calls.";
	}

	std::shared_ptr<std::vector<char>> FunctionLeaveQuick::Encode()
	{


		if (!hasEncoded)
		{
#pragma warning(suppress : 4267) // I'm only sending max 4k of data in one command however, the length() prop is long long. This is valid.
			__int32 size = 4 + 1 + sizeof(FunctionID) + 2;


			m_internalvector = std::vector<char>(size);

			char *intchar = (char*)&size;
			char *func = (char*)&function;


			for (size_t i = 0; i < 4; i++)
			{
				m_internalvector[i] = intchar[i];
			}

			m_internalvector[4] = 0x02;

			for (size_t i = 0; i < sizeof(FunctionID); i++)
			{
				m_internalvector[i + 5] = func[i];
			}
			for (size_t i = 0; i < 2; i++)
			{
				m_internalvector[i + size - 2] = 0x00;
			}
			hasEncoded = true;
		}

		return std::make_shared<std::vector<char>>(m_internalvector);
	}

	std::shared_ptr<ICommand> FunctionLeaveQuick::Decode(std::shared_ptr<std::vector<char>> &data)
	{
		return std::make_shared<ICommand>(ICommand());
	}

}