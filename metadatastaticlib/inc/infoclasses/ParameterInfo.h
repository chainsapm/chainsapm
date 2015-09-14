#pragma once
#include "commonstructures.h"

namespace InformationClasses {
	class ParameterInfo
	{
	public:
		ParameterInfo();
		~ParameterInfo();

		// Getter and setter for the parameter name
		const std::wstring& ParameterName();
		void ParameterName(const std::wstring&);

		// Getter and setter for the type string
		// This is mainly used as informational data when generating the function signature
		const std::wstring& ParameterTypeString();
		void  ParameterTypeString(const std::wstring&);

		// Getter and setter for the parameter type
		// this is used to extract the value from the args list
		// it is also used to set the format string for the ParameterValue
		const CorElementType& ParameterType();
		void ParameterType(CorElementType);

		// Getter and setter for the parameter value
		// this will really only be called from the Enter function and should
		// not be assumed not null
		const std::wstring& ParameterValue();
		void ParameterValue(std::wstring);



	private:
		std::wstring m_ParameterName;
		std::wstring m_ParameterTypeString;
		std::wstring m_ParameterValue;
		CorElementType m_ReturnType;
		size_t m_ParameterSize;
	};

}