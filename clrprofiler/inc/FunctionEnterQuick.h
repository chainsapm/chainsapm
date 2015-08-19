#pragma once
#include "ICommand.h"

namespace Commands
{
	
	class FunctionEnterQuick :
		public ICommand
	{
	public:
		FunctionEnterQuick(FunctionID data, ThreadID threadid, __int64 timestamp);
		~FunctionEnterQuick();
		virtual std::shared_ptr<std::vector<char>> Encode();
		virtual std::shared_ptr<ICommand> Decode(std::shared_ptr<std::vector<char>> &data);
		virtual std::wstring Name();
		virtual std::wstring Description();
		virtual short Code() { return code; }

	private:
		short code;
		std::shared_ptr<std::vector<char>> m_internalvector;
		bool hasEncoded;
		__int64 function;
		__int64 thread;
		__int64 m_timestamp;
	};

}