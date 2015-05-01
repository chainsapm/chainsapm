#pragma once

namespace Commands{


	class AssemblyDefinition
	{
	public:
		AssemblyDefinition();
		~AssemblyDefinition();
		virtual std::shared_ptr<std::vector<char>> Encode();
		virtual std::shared_ptr<AssemblyDefinition> Decode(std::shared_ptr<std::vector<char>> &data);
		virtual std::wstring Name();
		virtual std::wstring Description();

	private:
		FunctionID function;
		std::vector<char> m_internalvector;
		bool hasEncoded;
	};

}
