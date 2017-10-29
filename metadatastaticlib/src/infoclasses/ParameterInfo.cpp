#pragma once
#include "stdafx.h"
#include "../inc/infoclasses/ParameterInfo.h"

namespace InformationClasses {
	ParameterInfo::ParameterInfo()
	{
		std::wstring m_ParameterName;
		std::wstring m_ParameterTypeString;
		std::wstring m_ParameterValue;
	}

	ParameterInfo::~ParameterInfo()
	{

	}

	const std::wstring& ParameterInfo::ParameterName()
	{
		return this->m_ParameterName;
	}

	void ParameterInfo::ParameterName(const std::wstring& string)
	{
		this->m_ParameterName = *new std::wstring(string);
	}

	const std::wstring& ParameterInfo::ParameterTypeString()
	{
		return this->m_ParameterTypeString;
	}

	void ParameterInfo::ParameterTypeString(const std::wstring& paramTypeString)
	{
		this->m_ParameterTypeString = *new std::wstring(paramTypeString);
	}

	const CorElementType& ParameterInfo::ParameterType()
	{
		return this->m_ReturnType;
	}

	void ParameterInfo::ParameterType(CorElementType elementType)
	{
		this->m_ReturnType = elementType;
	}

}