#pragma once
#include "stdafx.h"
#include "SendString.h"


SendString::SendString(std::wstring data) : m_string(data), hasEncoded(false)
{
}

SendString::~SendString()
{
}

std::wstring SendString::Name()
{
	return L"Send String";
}

std::wstring SendString::Description()
{
	return L"Sends over a hash and a string for the sever to store.";
}

std::shared_ptr<std::vector<char>> SendString::Encode()
{


	if (!hasEncoded)
	{
#pragma warning(suppress : 4267) // I'm only sending max 4k of data in one command however, the length() prop is long long. This is valid.
		__int32 size = 4 + 4 + 1 + 2 + (m_string.length() * 2);
		__int32 fullsize = 8 + size;
		

		m_internalvector = std::vector<char>(fullsize);
		hasEncoded = true;


		char *intchar = (char*)&size;
		char *intcharfull = (char*)&fullsize;
		wchar_t *str = m_string._Myptr();

		for (size_t i = 0; i < 4; i++)
		{
			m_internalvector[i] = intcharfull[i];
		}
		for (size_t i = 0; i < 4; i++)
		{
			m_internalvector[i + 4] = intchar[i];
		}
		m_internalvector[8] = 0x01;
		auto hash = std::hash<std::wstring>();
		auto hashout = hash(m_string);
		auto hashfn = (char*)&hashout;

		for (size_t i = 0; i < 4; i++)
		{
			m_internalvector[i + 9] = hashfn[i];
		}
		for (size_t i = 0; i < m_string.length() * 2; i++)
		{
			m_internalvector[i + 13] = (reinterpret_cast<char*>(str))[i];
		}
		for (size_t i = 0; i < 2; i++)
		{
			m_internalvector[i + 13 + m_string.length() * 2] = 0x00;
		}
		for (size_t i = 0; i < 4; i++)
		{
#pragma warning(suppress : 4309) // We're not truncating, the compiler just doesn't like 0xCC as a valid value
			m_internalvector[i + 15 + m_string.length() * 2] = 0xCC;
		}
	}

	return std::make_shared<std::vector<char> >(m_internalvector);
}

std::shared_ptr<ICommand> SendString::Decode(std::shared_ptr<std::vector<char>> &data)
{
	return std::make_shared<ICommand>(ICommand());
}