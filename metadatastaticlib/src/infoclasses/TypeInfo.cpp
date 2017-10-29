#include "stdafx.h"
#include "../inc/infoclasses/TypeInfo.h"

namespace InformationClasses {
	TypeInfo::TypeInfo()
	{
	}


	TypeInfo::~TypeInfo()
	{
	}

	std::wstring TypeInfo::TypeName()
	{
		return this->m_TypeName;
	}

	void TypeInfo::TypeName(const std::wstring& className)
	{
		this->m_TypeName.assign(className);
	}

	mdToken& TypeInfo::TypeMetadataToken()
	{
		return this->m_classId;
	}

	void TypeInfo::TypeMetadataToken(const mdToken& classId)
	{
		this->m_classId = classId;
	}
}
