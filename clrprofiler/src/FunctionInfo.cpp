#pragma once
#include "stdafx.h"
#include "FunctionInfo.h"

namespace InformationClasses {
	FunctionInfo::FunctionInfo()
	{

	}

	FunctionInfo::~FunctionInfo()
	{
	}

	InformationClasses::ClassInfo* FunctionInfo::ClassInformation()
	{
		return this->m_ClassInfo;
	}

	void FunctionInfo::ClassInformation(InformationClasses::ClassInfo* classInfo)
	{
		this->m_ClassInfo = classInfo;
	}

	const std::wstring& FunctionInfo::FunctionName()
	{
		return this->m_FunctionName;
	}

	void FunctionInfo::FunctionName(const std::wstring& functionName)
	{
		this->m_FunctionName.assign(functionName);
	}

	const FunctionID& FunctionInfo::FunctionId()
	{
		return this->m_FunctionID;
	}

	void FunctionInfo::FunctionId(FunctionID funcID)
	{
		this->m_FunctionID = funcID;
	}

	const std::wstring&  FunctionInfo::SignatureString()
	{
		if (this->m_SignatureString.empty())
		{
			this->m_SignatureString.append(this->m_ReturnType);
			this->m_SignatureString.append(TEXT(" "));
			this->m_SignatureString.append(this->ClassInformation()->ClassName());
			this->m_SignatureString.append(TEXT("::"));
			this->m_SignatureString.append(this->m_FunctionName);
			this->m_SignatureString.append(TEXT("("));
			for (std::vector<ParameterInfo>::iterator it = m_Parameters.begin(); it != this->m_Parameters.end(); it++)
			{

				this->m_SignatureString.append(it->ParameterTypeString());
				this->m_SignatureString.append(TEXT(" "));
				this->m_SignatureString.append(it->ParameterName());
				if (it + 1 != m_Parameters.end())
				{
					this->m_SignatureString.append(TEXT(", "));
				}
			}
			this->m_SignatureString.append(TEXT(")"));
		}
		return this->m_SignatureString;
	}

	const std::wstring& FunctionInfo::ReturnType()
	{
		return this->m_ReturnType;
	}

	void FunctionInfo::ReturnType(const std::wstring& returnType)
	{
		this->m_ReturnType.assign(returnType);
	}

	const std::wstring& FunctionInfo::CallingConvention()
	{
		return this->m_CallingConvention;
	}

	void FunctionInfo::CallingConvention(const std::wstring& callingConvention)
	{
		this->m_CallingConvention.assign(callingConvention);
	}

	const PCCOR_SIGNATURE& FunctionInfo::SignatureRaw()
	{
		return this->m_RawSignature;
	}

	void FunctionInfo::SignatureRaw(const PCCOR_SIGNATURE& rawSig)
	{
		this->m_RawSignature = rawSig;
	}

	const std::vector<ParameterInfo>& FunctionInfo::Parameters()
	{
		return this->m_Parameters;
	}

	void FunctionInfo::AddParameters(const ParameterInfo& addParameter)
	{
		this->m_Parameters.push_back(*new ParameterInfo(addParameter));
	}

	const BOOL& FunctionInfo::IsStatic()
	{
		return this->m_IsStatic;
	}

	void FunctionInfo::IsStatic(BOOL isStatic)
	{
		this->m_IsStatic = isStatic;
	}

	const BOOL& FunctionInfo::IsEntryPoint()
	{
		return this->m_IsEntryPoint;
	}

	void FunctionInfo::IsEntryPoint(BOOL isEntry)
	{
		this->m_IsEntryPoint = isEntry;
	}

	const BOOL& FunctionInfo::AlwaysCreateNewEntryPoint()
	{
		return this->m_AlwaysCreateEntryPoint;
	}

	void FunctionInfo::AlwaysCreateNewEntryPoint(BOOL alwaysCreate)
	{
		this->m_AlwaysCreateEntryPoint = alwaysCreate;
	}
}