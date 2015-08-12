#pragma once
#include "ICommand.h"
#pragma once
#include "../../metadatastaticlib/inc/FunctionInfo.h"
namespace Commands
{
	class FunctionEnterLong :
		public virtual ICommand
	{
	public:
		FunctionEnterLong(InformationClasses::FunctionInfo funcinfo);
		FunctionEnterLong::FunctionEnterLong(FunctionEnterLong&& funcinfo);
		FunctionEnterLong::FunctionEnterLong(FunctionEnterLong& funcinfo) = delete;
		~FunctionEnterLong();
		virtual std::shared_ptr<std::vector<char>> Encode();
		virtual std::shared_ptr<ICommand> Decode(std::shared_ptr<std::vector<char>> &data);
		virtual std::wstring Name();
		virtual std::wstring Description();
		virtual short Code() { return code; }

	private:
		InformationClasses::FunctionInfo m_data;
		std::vector<char> m_internalvector;
		short code;
		bool hasEncoded;
	};

}