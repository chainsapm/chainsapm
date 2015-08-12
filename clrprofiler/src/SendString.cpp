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

	SendString::SendString(std::wstring &&data) : hasEncoded(false)
	{
		m_wstring.swap(data);
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
				code = 0x12;

				size_t strlen = (m_wstring.length());
				size_t strbytes = strlen*sizeof(wchar_t) + sizeof(wchar_t);
				size_t size = sizeof(__int32)	// len
					+ sizeof(short)				// code
					+ sizeof(__int32)			// stringlen
					+ sizeof(__int64)			// hash
					+ strbytes					// string
					+ sizeof(short);			// term

				auto vector = new char[size];
				short term = 0;

				auto hash = std::hash<std::wstring>();
				auto hashout = hash(m_wstring);

				auto v2 = (char*)memcpy(vector, &size, sizeof(__int32));
				v2 += sizeof(__int32);
				memcpy(v2, &code, sizeof(short));
				v2 += sizeof(short);
				memcpy(v2, &strlen, sizeof(__int32));
				v2 += sizeof(__int32);
				memcpy(v2, &hashout, sizeof(__int64));
				v2 += sizeof(__int64);
				memcpy(v2, m_wstring.data(), strbytes);
				v2 += strbytes;
				memcpy(v2, &term, sizeof(short));
				v2 += sizeof(short);

				hasEncoded = true;
				std::vector<char> v(size);
				v.assign(vector, v2);

				m_internalvector = std::make_shared<std::vector<char>>(v);
			}
			else {
				code = 0x11;
				size_t strlen = (m_string.length() + 1) * sizeof(char);
				size_t size = sizeof(__int32)	// len
					+ sizeof(short)				// code
					+ sizeof(__int32)			// stringlen
					+ sizeof(__int64)			// hash
					+ strlen					// string
					+ sizeof(short);			// term

				auto vector = new char[size];
				short term = 0;

				auto hash = std::hash<std::string>();
				auto hashout = hash(m_string);

				auto v2 = (char*)memcpy(vector, &size, sizeof(__int32));
				v2 += sizeof(__int32);
				memcpy(v2, &code, sizeof(short));
				v2 += sizeof(short);
				memcpy(v2, &strlen, sizeof(__int32));
				v2 += sizeof(__int32);
				memcpy(v2, &hashout, sizeof(__int64));
				v2 += sizeof(__int64);
				memcpy(v2, m_string.data(), strlen);
				v2 += strlen;
				memcpy(v2, &term, sizeof(short));
				v2 += sizeof(short);

				hasEncoded = true;
				std::vector<char> v(size);
				v.assign(vector, v2);
				m_internalvector = std::make_shared<std::vector<char>>(v);
			}
		}

		return m_internalvector;
	}

	std::shared_ptr<ICommand> SendString::Decode(std::shared_ptr<std::vector<char>> &data)
	{
		UNREFERENCED_PARAMETER(data);
		return std::make_shared<SendString>(SendString(m_wstring));
	}
}