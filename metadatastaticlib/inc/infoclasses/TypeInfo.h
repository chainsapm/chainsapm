#pragma once
#include "ModuleInfo.h"

namespace InformationClasses {
	class TypeInfo
	{
	public:
		TypeInfo();
		~TypeInfo();
		// Getter and setter for the function name
		std::wstring TypeName();
		void TypeName(const std::wstring&);

		mdToken& TypeMetadataToken();
		void TypeMetadataToken(const mdToken&);

	private:
		std::wstring m_TypeName;
		mdToken m_classId;
	};
}

