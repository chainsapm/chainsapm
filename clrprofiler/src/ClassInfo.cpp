#include "stdafx.h"
#include "../../metadatastaticlib/inc/ClassInfo.h"

namespace InformationClasses {
	ClassInfo::ClassInfo()
	{
	}


	ClassInfo::~ClassInfo()
	{
	}

	std::wstring& ClassInfo::ClassName()
	{
		return this->m_ClassName;
	}

	void ClassInfo::ClassName(const std::wstring& className)
	{
		this->m_ClassName.assign(className);
	}

	ClassID& ClassInfo::ClassId()
	{
		return this->m_classId;
	}

	void ClassInfo::ClassId(const ClassID& classId)
	{
		this->m_classId = classId;
	}
}
