#pragma once
#include "stdafx.h"
#include "SendString.h"


namespace Commands
{
	SendString::SendString(std::wstring& data) : m_wstring(data), hasEncoded(false), wchar(true)
	{
		code = 0x03;
	}

	SendString::SendString(std::string& data) : m_string(data), hasEncoded(false)
	{
		code = 0x04;
	}

	SendString::SendString(std::wstring &&data) : hasEncoded(false)
	{
		code = 0x03;
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

		if (!hasEncoded) {
			if (wchar) {
				code = 0x03;

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
				m_internalvector = std::make_shared<std::vector<char>>(vector, v2);
				delete vector;
			}
			else {
				code = 0x04;
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
				m_internalvector = std::make_shared<std::vector<char>>(vector, v2);
				delete vector;
			}
		}

		return m_internalvector;
	}

	std::shared_ptr<ICommand> SendString::Decode(std::shared_ptr<std::vector<char>> &data)
	{
		auto ptr = data->data();

		auto len = *(unsigned int*)ptr;
		ptr += sizeof(unsigned int);

		auto code = (char)ptr;
		ptr += sizeof(short);

		auto strlen = *(__int32*)ptr;
		ptr += sizeof(__int32);

		auto hash = *(__int64*)ptr;
		ptr += sizeof(__int64);

		auto str = (wchar_t*)ptr;
		ptr += strlen * 2;

		auto term = *(short*)ptr;
		ptr += sizeof(short);

		return std::make_shared<SendString>(str);
	}
}