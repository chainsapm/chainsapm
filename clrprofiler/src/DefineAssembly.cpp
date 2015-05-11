#pragma once
#include "stdafx.h"
#include "DefineAssembly.h"


namespace Commands
{
	DefineAssembly::DefineAssembly(std::wstring& data) 
		: m_wstring(data), hasEncoded(false), wchar(true), code(0x0)
	{
	}

	DefineAssembly::DefineAssembly(std::string& data) 
		: m_string(data), hasEncoded(false), code(0x0)
	{
	}

	DefineAssembly::~DefineAssembly()
	{
	}

	std::wstring DefineAssembly::Name()
	{
		return L"Send String";
	}

	std::wstring DefineAssembly::Description()
	{
		return L"Sends over a hash and a string for the sever to store.";
	}

	std::shared_ptr<std::vector<char>> DefineAssembly::Encode()
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

	std::shared_ptr<ICommand> DefineAssembly::Decode(std::shared_ptr<std::vector<char>> &data)
	{
		return std::make_shared<DefineAssembly>(DefineAssembly(m_wstring));
	}
}