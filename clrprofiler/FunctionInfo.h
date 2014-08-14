#pragma once
#include "commonstructures.h"
#include "ParameterInfo.h"
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

	const std::vector<ParameterInfo>& Parameters();
	void AddParameters(const ParameterInfo& addParameter);

private:
	std::wstring m_ClassName;
	std::wstring m_FunctionName;
	FunctionID m_FunctionID;
	PCCOR_SIGNATURE m_RawSignature;
	std::wstring m_ReturnType;
	std::wstring m_SignatureString;
	std::wstring m_CallingConvention;
	std::vector<ParameterInfo> m_Parameters;
};


