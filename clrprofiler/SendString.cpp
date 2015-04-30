#pragma once
#include "stdafx.h"
#include "SendString.h"


namespace Commands
{
	SendString::SendString(std::wstring& data) : m_wstring(data), hasEncoded(false), wchar(true)
	{
	}

	SendString::SendString(std::string& data) : m_string(data), hasEncoded(false)
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
			if (wchar)
			{

#pragma warning(suppress : 4267) // I'm only sending max 4k of data in one command however, the length() prop is long long. This is valid.
				__int32 size = 4 + 4 + 1 + 2 + (m_wstring.length() * 2);


				m_internalvector = std::vector<char>(size);
				hasEncoded = true;

				char *intchar = (char*)&size;
				wchar_t *str = m_wstring._Myptr();

				for (size_t i = 0; i < 4; i++)
				{
					m_internalvector[i] = intchar[i];
				}
				m_internalvector[4] = 0x03;
				auto hash = std::hash<std::wstring>();
				auto hashout = hash(m_wstring);
				auto hashfn = (char*)&hashout;

				for (size_t i = 0; i < 4; i++)
				{
					m_internalvector[i + 5] = hashfn[i];
				}
				for (size_t i = 0; i < m_wstring.length() * 2; i++)
				{
					m_internalvector[i + 9] = (reinterpret_cast<char*>(str))[i];
				}
				for (size_t i = 0; i < 2; i++)
				{
					m_internalvector[i + 9 + m_wstring.length() * 2] = 0x00;
				}
			}
			else {

#pragma warning(suppress : 4267) // I'm only sending max 4k of data in one command however, the length() prop is long long. This is valid.
				__int32 size = 4 + 4 + 1 + 2 + (m_string.length());


				m_internalvector = std::vector<char>(size);
				hasEncoded = true;

				char *intchar = (char*)&size;
				char *str = m_string._Myptr();

				for (size_t i = 0; i < 4; i++)
				{
					m_internalvector[i] = intchar[i];
				}
				m_internalvector[4] = 0x01;
				auto hash = std::hash<std::string>();
				auto hashout = hash(m_string);
				auto hashfn = (char*)&hashout;

				for (size_t i = 0; i < 4; i++)
				{
					m_internalvector[i + 5] = hashfn[i];
				}
				for (size_t i = 0; i < m_string.length(); i++)
				{
					m_internalvector[i + 9] = (reinterpret_cast<char*>(str))[i];
				}
				for (size_t i = 0; i < 2; i++)
				{
					m_internalvector[i + 9 + m_string.length()] = 0x00;
				}
			}
		}

		return std::make_shared<std::vector<char> >(m_internalvector);
	}

	std::shared_ptr<ICommand> SendString::Decode(std::shared_ptr<std::vector<char>> &data)
	{
		return std::make_shared<ICommand>(ICommand());
	}
}