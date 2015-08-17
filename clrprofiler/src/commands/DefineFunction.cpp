#pragma once
#include "stdafx.h"
#include "DefineFunction.h"


namespace Commands
{
	DefineFunction::DefineFunction(FunctionID funcId, ClassID classId, const std::wstring& data, __int64 timestamp)
		: hasEncoded(false), wchar(true), code(0x1B), m_funcId((__int64)funcId), m_classId((__int64)classId), m_timestamp(timestamp)
	{
		m_wstring->assign(data);
	}

	DefineFunction::DefineFunction()
	{
	}

	DefineFunction::~DefineFunction()
	{
	}

	std::wstring DefineFunction::Name()
	{
		return L"Send Function Definition";
	}

	std::wstring DefineFunction::Description()
	{
		return L"Sends over a function id to be mapped to a string for the sever to store.";
	}

	std::shared_ptr<std::vector<char>> DefineFunction::Encode()
	{
		if (!hasEncoded)
		{

			size_t strlen = (m_wstring->length());
			size_t strbytes = strlen*sizeof(wchar_t) + sizeof(wchar_t);
			size_t size = sizeof(__int32)	// len
				+ sizeof(short)				// code
				+ sizeof(__int64)			// timestamp
				+ sizeof(__int64)			// FunctionId
				+ sizeof(__int64)			// ClassId
				+ sizeof(__int32)			// stringlen
				+ strbytes					// string
				+ sizeof(short);			// term

			auto vector = new char[size];
			short term = 0;

			auto v2 = (char*)memcpy(vector, &size, sizeof(__int32));
			v2 += sizeof(__int32);
			memcpy(v2, &code, sizeof(short));
			v2 += sizeof(short);
			memcpy(v2, &m_timestamp, sizeof(__int64));
			v2 += sizeof(__int64);
			memcpy(v2, &m_funcId, sizeof(__int64));
			v2 += sizeof(__int64);
			memcpy(v2, &m_classId, sizeof(__int64));
			v2 += sizeof(__int64);
			memcpy(v2, &strlen, sizeof(__int32));
			v2 += sizeof(__int32);
			memcpy(v2, m_wstring->data(), strbytes);
			v2 += strbytes;
			memcpy(v2, &term, sizeof(short));
			v2 += sizeof(short);

			hasEncoded = true;
			std::vector<char> v(size);
			v.assign(vector, v2);

			m_internalvector = std::make_shared<std::vector<char>>(v);
		}
		return m_internalvector;
	}

	std::shared_ptr<ICommand> DefineFunction::Decode(std::shared_ptr<std::vector<char>> &data)
	{
		return std::make_shared<DefineFunction>(m_funcId, m_classId, *m_wstring, m_timestamp);
	}
}