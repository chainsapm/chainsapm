#pragma once
#include "ModuleInfo.h"

namespace InformationClasses {
	class ClassInfo
	{
	public:
		ClassInfo();
		~ClassInfo();
		// Getter and setter for the function name
		std::wstring& ClassName();
		void ClassName(const std::wstring&);

		ClassID& ClassId();
		void ClassId(const ClassID&);

	private:
		std::wstring m_ClassName;
		ClassID m_classId;
	};
}

