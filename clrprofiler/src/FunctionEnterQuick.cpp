#pragma once
#include "stdafx.h"
#include "FunctionEnterQuick.h"

namespace Commands
{

	FunctionEnterQuick::FunctionEnterQuick(FunctionID data, ThreadID threadid) : function((__int64)data), thread((__int64)threadid), code(2), hasEncoded(false)
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
			__int32 size = sizeof(__int32) + sizeof(short) + sizeof(__int64) + sizeof(__int64) + sizeof(short);
			auto vector = std::vector<char>(size);

			int runningCount = 0;

			for (size_t i = 0; i < 4; i++, runningCount++)
			{
				vector[runningCount] = ((char*)(&size))[i];
			}

			for (size_t i = 0; i < 2; i++, runningCount++)
			{
				vector[runningCount] = ((char*)(&code))[i];
			}

			for (size_t i = 0; i < sizeof(__int64); i++, runningCount++)
			{
				vector[runningCount] = ((char*)(&function))[i];
			}
			for (size_t i = 0; i < sizeof(__int64); i++, runningCount++)
			{
				vector[runningCount] = ((char*)(&thread))[i];
			}
			for (size_t i = 0; i < 2; i++)
			{
				vector[runningCount] = 0x00;
			}
			hasEncoded = true;
			m_internalvector = std::make_shared<std::vector<char>>(vector);
		}

		return m_internalvector;
	}

	std::shared_ptr<ICommand> FunctionEnterQuick::Decode(std::shared_ptr<std::vector<char>> &data)
	{
		return std::make_shared<ICommand>(ICommand());
	}
}