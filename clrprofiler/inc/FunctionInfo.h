
#pragma once
#include "commonstructures.h"
#include "ParameterInfo.h"

namespace InformationClasses {

	class FunctionInfo
	{
	public:
		FunctionInfo();
		~FunctionInfo();

		// Getter and setter for class name
		const std::wstring& ClassName();
		void ClassName(const std::wstring&);

		// Getter and setter for the function name
		const std::wstring& FunctionName();
		void FunctionName(const std::wstring&);

		// Getter and setter for the function ID
		const FunctionID& FunctionId();
		void FunctionId(FunctionID);

		// Getter for generated signature string
		const std::wstring&  SignatureString();

		const PCCOR_SIGNATURE& SignatureRaw();
		void SignatureRaw(const PCCOR_SIGNATURE&);

		// Getter and setter for the return type
		const std::wstring& ReturnType();
		void ReturnType(const std::wstring&);

		// Getter and setter for type name
		const std::wstring& CallingConvention();
		void CallingConvention(const std::wstring&);

		const std::vector<InformationClasses::ParameterInfo>& Parameters();
		void AddParameters(const InformationClasses::ParameterInfo& addParameter);

		const BOOL& IsStatic();
		void IsStatic(BOOL);

		const BOOL& IsEntryPoint();
		void IsEntryPoint(BOOL);

		const BOOL& AlwaysCreateNewEntryPoint();
		void AlwaysCreateNewEntryPoint(BOOL);

	private:
		std::wstring m_ClassName;
		std::wstring m_FunctionName;
		FunctionID m_FunctionID;
		PCCOR_SIGNATURE m_RawSignature;
		std::wstring m_ReturnType;
		std::wstring m_SignatureString;
		std::wstring m_CallingConvention;
		std::vector<InformationClasses::ParameterInfo> m_Parameters;
		BOOL m_IsStatic;
		BOOL m_IsEntryPoint;
		BOOL m_AlwaysCreateEntryPoint;
	};

}