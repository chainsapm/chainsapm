#pragma once
#include "stdafx.h"
#include "FunctionEnterQuick.h"


FunctionEnterQuick::FunctionEnterQuick(FunctionID data) : function(data), hasEncoded(false)
{
}

FunctionEnterQuick::~FunctionEnterQuick()
{
}

std::wstring FunctionEnterQuick::Name()
{
	return L"Function Enter (Quick)";
}

std::wstring FunctionEnterQuick::Description()
{
	return L"Sends over an enter method, from the ELT calls.";
}

std::shared_ptr<std::vector<char>> FunctionEnterQuick::Encode()
{


	if (!hasEncoded)
	{
#pragma warning(suppress : 4267) // I'm only sending max 4k of data in one command however, the length() prop is long long. This is valid.
		__int32 size = 4 + 1 + sizeof(FunctionID);
		__int32 fullsize = 8 + size;


		m_internalvector = std::vector<char>(size);
		hasEncoded = true;
		char *intchar = (char*)&size;
		char *func = (char*)&function;

		
		for (size_t i = 0; i < 4; i++)
		{
			m_internalvector[i + 4] = intchar[i];
		}

		m_internalvector[4] = 0x02;

		for (size_t i = 0; i < sizeof(FunctionID); i++)
		{
			m_internalvector[i + 5] = func[i];
		}
		for (size_t i = 0; i < 2; i++)
		{
			m_internalvector[i + size] = 0x00;
		}
	}

	return std::make_shared<std::vector<char> >(m_internalvector);
}

std::shared_ptr<ICommand> FunctionEnterQuick::Decode(std::shared_ptr<std::vector<char>> &data)
{
	return std::make_shared<ICommand>(ICommand());
}