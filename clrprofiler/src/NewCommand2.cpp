/*
 *
 * This code was generated from the Command Creator.
 *
 */

#pragma once
#include "stdafx.h"
#include "NewCommand2.h"


namespace Commands
{
	NewCommand2::NewCommand2(__int64 timestamp, std::wstring str1, std::wstring str2, std::wstring str3, __int64 int64_1)
		: m_timestamp(timestamp), m_str1(str1), m_str2(str2), m_str3(str3), m_int64_1(int64_1) 	{
	}

	NewCommand2::~NewCommand2()
	{
	}

	std::wstring NewCommand2::Name()
	{
		return L"New Command2";
	}

	std::wstring NewCommand2::Description()
	{
		return L"Update Command Description";
	}

	std::shared_ptr<std::vector<char>> NewCommand2::Encode()
	{
		if (!hasEncoded)
		{
			// The following items will be resued 
			auto hash = std::hash<std::wstring>();
			size_t hashout = 0;		// Temp var for string hashes

			size_t size = sizeof(__int32)	// len
				+ sizeof(short)				// code
				+ sizeof(__int64);			// timestamp

			size_t strlen_str1 = (m_str1.length());
			size_t strbytes_str1 = strlen_str1 * sizeof(wchar_t) + sizeof(wchar_t);
			size += sizeof(__int32);
			size += sizeof(size_t);
			size += strbytes_str1;


			size_t strlen_str2 = (m_str2.length());
			size_t strbytes_str2 = strlen_str2 * sizeof(wchar_t) + sizeof(wchar_t);
			size += sizeof(__int32);
			size += sizeof(size_t);
			size += strbytes_str2;


			size_t strlen_str3 = (m_str3.length());
			size_t strbytes_str3 = strlen_str3 * sizeof(wchar_t) + sizeof(wchar_t);
			size += sizeof(__int32);
			size += sizeof(size_t);
			size += strbytes_str3;

			size += sizeof(__int64); // Size of int64_1
			size += sizeof(short);	//Terminator


			auto vector = new char[size];
			short term = 0;

			// 
			auto v2 = (char*)memcpy(vector, &size, sizeof(__int32));
			v2 += sizeof(__int32);
			memcpy(v2, &code, sizeof(short));
			v2 += sizeof(short);
			memcpy(v2, &m_timestamp, sizeof(__int64));
			v2 += sizeof(__int64);


			// Regenerate the string information
			hashout = hash(m_str1);

			memcpy(v2, &strlen_str1, sizeof(__int32));
			v2 += sizeof(__int32);
			memcpy(v2, &hashout, sizeof(size_t));
			v2 += sizeof(__int64);
			memcpy(v2, m_str1.data(), strbytes_str1);
			v2 += strbytes_str1;


			// Regenerate the string information
			hashout = hash(m_str2);

			memcpy(v2, &strlen_str2, sizeof(__int32));
			v2 += sizeof(__int32);
			memcpy(v2, &hashout, sizeof(size_t));
			v2 += sizeof(__int64);
			memcpy(v2, m_str2.data(), strbytes_str2);
			v2 += strbytes_str2;


			// Regenerate the string information
			hashout = hash(m_str3);

			memcpy(v2, &strlen_str3, sizeof(__int32));
			v2 += sizeof(__int32);
			memcpy(v2, &hashout, sizeof(size_t));
			v2 += sizeof(__int64);
			memcpy(v2, m_str3.data(), strbytes_str3);
			v2 += strbytes_str3;

			memcpy(v2, &m_int64_1, sizeof(__int64));
			v2 += sizeof(__int64);

			// Terminator
			memcpy(v2, &term, sizeof(short));
			v2 += sizeof(short);

			hasEncoded = true;
			std::vector<char> v(size);
			v.assign(vector, v2);

			m_internalvector = std::make_shared<std::vector<char>>(v);
		}
		return m_internalvector;
	}

	std::shared_ptr<ICommand> NewCommand2::Decode(std::shared_ptr<std::vector<char>> &data)
	{
		return std::make_shared<NewCommand2>(NewCommand2(m_timestamp, m_str1, m_str2, m_str3, m_int64_1));
	}
}

