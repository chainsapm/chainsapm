#include "stdafx.h"

#include "MetadataHelpers.h"


// CRITICAL 1 Research thread safety around metadata functions.
MetadataHelpers::MetadataHelpers()
{
	InitializeCriticalSection(&m_ThreadCS);
}


MetadataHelpers::MetadataHelpers(std::shared_ptr<ICorProfilerInfo> profilerInfo) : MetadataHelpers() 
{
	m_pICorProfilerInfo = profilerInfo;
}

MetadataHelpers::MetadataHelpers(std::shared_ptr<ICorProfilerInfo2> profilerInfo) : MetadataHelpers()
{
	m_pICorProfilerInfo = profilerInfo;
	m_pICorProfilerInfo2 = profilerInfo;
}

MetadataHelpers::MetadataHelpers(std::shared_ptr<ICorProfilerInfo3> profilerInfo) :MetadataHelpers()
{
	m_pICorProfilerInfo = profilerInfo;
	m_pICorProfilerInfo2 = profilerInfo;
	m_pICorProfilerInfo3 = profilerInfo;
}

MetadataHelpers::MetadataHelpers(std::shared_ptr<ICorProfilerInfo4> profilerInfo) : MetadataHelpers()
{
	m_pICorProfilerInfo = profilerInfo;
	m_pICorProfilerInfo2 = profilerInfo;
	m_pICorProfilerInfo3 = profilerInfo;
	m_pICorProfilerInfo4 = profilerInfo;
}


MetadataHelpers::~MetadataHelpers()
{
	DeleteCriticalSection(&m_ThreadCS);
	if (m_pICorProfilerInfo != NULL)
	{
		m_pICorProfilerInfo.reset();
	}
	if (m_pICorProfilerInfo2 != NULL)
	{
		m_pICorProfilerInfo2.reset();
	}
	if (m_pICorProfilerInfo3 != NULL)
	{
		m_pICorProfilerInfo3.reset();
	}
	if (m_pICorProfilerInfo4 != NULL)
	{
		m_pICorProfilerInfo4.reset();
	}
}

STDMETHODIMP MetadataHelpers::InjectFieldToModule(const ModuleID& ModuleId, const mdTypeDef& classTypeDef,
	const std::wstring& fieldName)
{
	std::unique_ptr<IMetaDataEmit> _MetaDataEmit;
	std::unique_ptr<IMetaDataEmit2> _MetaDataEmit2;
	std::unique_ptr<IMetaDataImport> _MetaDataImport;
	std::unique_ptr<IMetaDataImport2> _MetaDataImport2;

	this->GetMetaDataEmitInterFaceFromModule(ModuleId, _MetaDataEmit, _MetaDataEmit2);
	this->GetMetaDataImportInterfaceFromModule(ModuleId, _MetaDataImport, _MetaDataImport2);

	if (_MetaDataImport == NULL)
	{
		_MetaDataImport.reset(_MetaDataImport2.get());
	}

	if (_MetaDataEmit == NULL)
	{
		_MetaDataEmit.reset(_MetaDataEmit2.get());
	}

	if (_MetaDataEmit != NULL && _MetaDataImport != NULL)
	{
		MDUTF8CSTR typeDefName;

		_MetaDataImport->GetNameFromToken(classTypeDef, &typeDefName);
		std::string typeDefString(typeDefName);

		WCHAR szClassName[MAX_LENGTH];
		DWORD flags;
		ULONG typeDefPointer;
		mdToken extends;

		_MetaDataImport->GetTypeDefProps(
			classTypeDef,
			szClassName,
			2048,
			&typeDefPointer,
			&flags,
			&extends);
		std::wstring moduleString(szClassName);

		if (moduleString == L"System.Threading.Thread")
		{
			COR_SIGNATURE testSig[] = {
				ELEMENT_TYPE_I
			};

			mdFieldDef fieldOut;

			_MetaDataEmit->DefineField(
				classTypeDef,
				fieldName.c_str(),
				CorFieldAttr::fdPrivate,
				testSig,
				sizeof(testSig),
				0,
				0,
				0,
				&fieldOut);

			_MetaDataEmit->Save(L"C:\\test.dll", NULL);
		}
	}
	_MetaDataEmit.release();
	_MetaDataEmit2.release();
	_MetaDataImport.release();
	_MetaDataImport2.release();
	return S_OK;
}

STDMETHODIMP MetadataHelpers::GetFunctionInformation(FunctionID funcId, InformationClasses::FunctionInfo* funcInfo)
{

	mdMethodDef funcToken;
	ModuleID modID;
	ClassID classID;

	std::unique_ptr<IMetaDataImport> _MetaDataImport;
	std::unique_ptr<IMetaDataImport2> _MetaDataImport2;

	this->GetMetaDataImportInterfaceFromFunction(funcId, &funcToken, _MetaDataImport, _MetaDataImport2);
	
	

	if (_MetaDataImport == NULL)
	{
		_MetaDataImport.reset(_MetaDataImport2.get());
	}

	if (_MetaDataImport != NULL)
	{
		/*
		MSDN link for GetMethodProps
		http://msdn.microsoft.com/en-us/library/ms233163(v=vs.110).aspx

		Use this to get the method name and the class TypeDef (mdTypeDef). This also will
		return the signature blob and it's size.

		In order to get the number of parameters in the signature we will divide the blob
		size by sizeof(COR_SIGNATURE).

		We will use that size to get the parameter from the GetParamForMethodIndex.

		*/

		mdTypeDef classTypeDef;
		WCHAR szMethodName[MAX_LENGTH];
		ULONG stringLen;
		DWORD methodAttr;
		PCCOR_SIGNATURE sigBlob;
		ULONG sigBlobBytes;
		ULONG codeRVA;
		DWORD implFlags;

		HRESULT hr = _MetaDataImport->GetMethodProps(
			funcToken,
			&classTypeDef,
			szMethodName,
			MAX_LENGTH,
			&stringLen,
			&methodAttr,
			&sigBlob,
			&sigBlobBytes,
			&codeRVA,
			&implFlags
			);
		DWORD NumberOfParams = sigBlobBytes;
		
		funcInfo->FunctionId(funcId);
		std::wstring copyString = std::wstring(szMethodName);
		funcInfo->FunctionName(copyString);
		funcInfo->SignatureRaw(sigBlob); // Create a clone


		m_pICorProfilerInfo->GetFunctionInfo(funcId, &modID, &classID, &funcToken);

		//auto classExists = m_ContainerClass->g_ClassSet->find(classID);
		//if (classExists == m_ContainerClass->g_ClassSet->end())
		//{
		//	InformationClasses::ClassInfo * classInfo = nullptr; // Get a reference to the classInfo
		//	GetClassInformation(classID, classInfo);
		//	funcInfo->ClassInformation(classInfo);
		//}


	

		

		/*
		MSDN link for GetParamForMethodIndex
		http://msdn.microsoft.com/en-us/library/ms231012(v=vs.110).aspx

		Use this to get the proper names of the parameters
		*/

		if (SUCCEEDED(hr))
		{
			ULONG callConv;
			BOOL isStatic;
			ULONG argCount = 0;


			//
			// Is the method static ?
			//
			(isStatic) = (BOOL)((methodAttr & mdStatic) != 0);
			funcInfo->IsStatic(isStatic);

			//
			// Make sure we have a method signature.
			//


			sigBlob += CorSigUncompressData(sigBlob, &callConv);

			if (callConv != IMAGE_CEE_CS_CALLCONV_FIELD)
			{
				static WCHAR* callConvNames[8] =
				{
					TEXT(""),
					TEXT("unmanaged cdecl"),
					TEXT("unmanaged stdcall"),
					TEXT("unmanaged thiscall"),
					TEXT("unmanaged fastcall"),
					TEXT("vararg"),
					TEXT("<error>"),
					TEXT("<error>")
				};
				if ((callConv & 7) != 0)
					funcInfo->CallingConvention(std::wstring(callConvNames[callConv & 7]));

				//
				// Grab the argument count
				//
				sigBlob += CorSigUncompressData(sigBlob, &argCount);

				//
				// Get the return type
				//
				std::wstring *returnHolder = new std::wstring();
				sigBlob = ParseElementType(_MetaDataImport, sigBlob, returnHolder);

				//
				// if the return type returned back empty, write void
				//

				funcInfo->ReturnType(*returnHolder);
				if (funcInfo->ReturnType().length() == 0)
					funcInfo->ReturnType(TEXT("void"));

				//
				// Get the parameters
				//			
				std::wstring sigHolder;
				for (ULONG i = 0;
					(SUCCEEDED(hr) && (sigBlob != NULL) && (i < (argCount)));
					i++)
				{
					
					InformationClasses::ParameterInfo paramInfo;
					sigHolder.clear();
					sigBlob = ParseElementType(_MetaDataImport, sigBlob, &sigHolder);
					paramInfo.ParameterTypeString(sigHolder);

					mdParamDef paramDef;
					mdMethodDef funcTokenOut;
					ULONG sequence;
					DWORD paramAttributes;
					DWORD cplusTypeFlags;
					WCHAR szParamName[MAX_LENGTH];
					ULONG szParamNameLen;
					UVCP_CONSTANT constVal;
					ULONG constValueLen;

					_MetaDataImport->GetParamForMethodIndex(funcToken, i + 1, &paramDef);

					_MetaDataImport->GetParamProps(paramDef,
						&funcTokenOut,
						&sequence,
						szParamName,
						MAX_LENGTH,
						&szParamNameLen,
						&paramAttributes,
						&cplusTypeFlags,
						&constVal,
						&constValueLen);

					
					paramInfo.ParameterName(szParamName);
					funcInfo->AddParameters(paramInfo);
					/*auto foundEntrypoint = this->m_ContainerClass->g_FunctionSet->find(funcId);
					auto endOfSet = this->m_ContainerClass->g_FunctionSet->end();
					if (foundEntrypoint != endOfSet)
					{
						funcInfo->IsEntryPoint(TRUE);
					}*/

				}
			}
		}
		_MetaDataImport.release();
		_MetaDataImport2.release();
		return S_OK;
	}
	_MetaDataImport.release();
	_MetaDataImport2.release();
	return E_FAIL;
}
STDMETHODIMP MetadataHelpers::GetClassInformation(mdTypeDef ClassMD, InformationClasses::ClassInfo* classInfo)
{
	return S_OK;
	///*
	//MSDN link for GetTypeDefProps
	//http://msdn.microsoft.com/en-us/library/ms230143(v=vs.110).aspx

	//Use this to get they type of an object. In this case we are getting the class
	//name.
	//*/
	//WCHAR szClassName[MAX_LENGTH];
	//DWORD flags;
	//ULONG typeDefPointer;
	//mdToken extends;

	//HRESULT hr = _MetaDataImport->GetTypeDefProps(
	//	classTypeDef,
	//	szClassName,
	//	2048,
	//	&typeDefPointer,
	//	&flags,
	//	&extends);

	//funcInfo->ClassInformation()->ClassName(std::wstring(szClassName));
	//return S_OK;
	
}
STDMETHODIMP MetadataHelpers::GetModuleInformation(mdTypeDef moduleMD, InformationClasses::ModuleInfo* moduleInfo)
{

	return S_OK;
}
STDMETHODIMP MetadataHelpers::GetAssemblyInformation(mdTypeDef assemblyMD, InformationClasses::AssemblyInfo* assemblyInfo)
{

	return S_OK;
}


/* static public */
PCCOR_SIGNATURE MetadataHelpers::ParseElementType(const std::unique_ptr<IMetaDataImport>&  pMDImport,
	PCCOR_SIGNATURE signature, std::wstring *buffer)
{
	switch (*signature++)
	{
	case ELEMENT_TYPE_VOID:
		buffer->append(TEXT("void"));
		break;


	case ELEMENT_TYPE_BOOLEAN:
		buffer->append(TEXT("bool"));
		break;


	case ELEMENT_TYPE_CHAR:
		buffer->append(TEXT("wchar"));
		break;


	case ELEMENT_TYPE_I1:
		buffer->append(TEXT("int8"));
		break;


	case ELEMENT_TYPE_U1:
		buffer->append(TEXT("unsigned int8"));
		break;


	case ELEMENT_TYPE_I2:
		buffer->append(TEXT("int16"));
		break;


	case ELEMENT_TYPE_U2:
		buffer->append(TEXT("unsigned int16"));
		break;


	case ELEMENT_TYPE_I4:
		buffer->append(TEXT("int32"));
		break;


	case ELEMENT_TYPE_U4:
		buffer->append(TEXT("unsigned int32"));
		break;


	case ELEMENT_TYPE_I8:
		buffer->append(TEXT("int64"));
		break;


	case ELEMENT_TYPE_U8:
		buffer->append(TEXT("unsigned int64"));
		break;


	case ELEMENT_TYPE_R4:
		buffer->append(TEXT("float32"));
		break;


	case ELEMENT_TYPE_R8:
		buffer->append(TEXT("float64"));
		break;


	case ELEMENT_TYPE_U:
		buffer->append(TEXT("unsigned int"));
		break;


	case ELEMENT_TYPE_I:
		buffer->append(TEXT("int"));
		break;


	case ELEMENT_TYPE_OBJECT:
		buffer->append(TEXT("Object"));
		break;


	case ELEMENT_TYPE_STRING:
		buffer->append(TEXT("String"));
		break;


	case ELEMENT_TYPE_TYPEDBYREF:
		buffer->append(TEXT("refany "));
		break;

	case ELEMENT_TYPE_CLASS:
	case ELEMENT_TYPE_VALUETYPE:
	case ELEMENT_TYPE_CMOD_REQD:
	case ELEMENT_TYPE_CMOD_OPT:

		mdToken	token;
		signature += CorSigUncompressToken(signature, &token);
		if (TypeFromToken(token) != mdtTypeRef)
		{
			HRESULT	hr;
			WCHAR zName[MAX_LENGTH];
			ULONG zLen;

			hr = pMDImport->GetTypeDefProps(token,
				zName,
				MAX_LENGTH,
				&zLen,
				NULL,
				NULL);
			buffer->append(zName, zLen - 1);
		}



		break;


	case ELEMENT_TYPE_SZARRAY:
		signature = this->ParseElementType(pMDImport, signature, buffer);
		buffer->append(TEXT("[]"));
		break;


	case ELEMENT_TYPE_ARRAY:
	{
							   ULONG rank;


							   signature = this->ParseElementType(pMDImport, signature, buffer);
							   rank = CorSigUncompressData(signature);
							   if (rank == 0)
								   buffer->append(TEXT("[?]"));

							   else
							   {
								   ULONG *lower;
								   ULONG *sizes;
								   ULONG numsizes;
								   ULONG arraysize = (sizeof (ULONG)* 2 * rank);


								   lower = (ULONG *)_alloca(arraysize);
								   memset(lower, 0, arraysize);
								   sizes = &lower[rank];

								   numsizes = CorSigUncompressData(signature);
								   if (numsizes <= rank)
								   {
									   ULONG numlower;


									   for (ULONG i = 0; i < numsizes; i++)
										   sizes[i] = CorSigUncompressData(signature);


									   numlower = CorSigUncompressData(signature);
									   if (numlower <= rank)
									   {
										   for (ULONG i = 0; i < numlower; i++)
											   lower[i] = CorSigUncompressData(signature);


										   buffer->append(TEXT("["));
										   for (ULONG i = 0; i < rank; i++)
										   {
											   if ((sizes[i] != 0) && (lower[i] != 0))
											   {
												   if (lower[i] == 0)
													   wsprintf((LPWSTR)buffer->c_str(), TEXT("%d"), sizes[i]);

												   else
												   {
													   wsprintf((LPWSTR)buffer->c_str(), TEXT("%d"), lower[i]);
													   buffer->append(TEXT("..."));

													   if (sizes[i] != 0)
														   wsprintf((LPWSTR)buffer->c_str(), TEXT("%d"), (lower[i] + sizes[i] + 1));
												   }
											   }

											   if (i < (rank - 1))
												   buffer->append(TEXT(","));
										   }

										   buffer->append(TEXT("]"));
									   }
								   }
							   }
	}
		break;

	case ELEMENT_TYPE_PINNED:
		signature = this->ParseElementType(pMDImport, signature, buffer);
		buffer->append(TEXT("pinned"));
		break;


	case ELEMENT_TYPE_PTR:
		signature = this->ParseElementType(pMDImport, signature, buffer);
		buffer->append(TEXT("*"));
		break;


	case ELEMENT_TYPE_BYREF:
		signature = this->ParseElementType(pMDImport, signature, buffer);
		buffer->append(TEXT("&"));
		break;


	default:
	case ELEMENT_TYPE_END:
	case ELEMENT_TYPE_SENTINEL:
		buffer->append(TEXT("<UNKNOWN>"));
		break;

	} // switch	


	return signature;

} // BASEHELPER::ParseElementType



STDMETHODIMP MetadataHelpers::GetCurrentThread(ThreadID* threadId)
{
	//CRITICAL 1 Make this more thread safe
	if (this->m_pICorProfilerInfo4 != nullptr)
	{
		this->m_pICorProfilerInfo4->GetCurrentThreadID(threadId);
		return S_OK;
	}
	else if (this->m_pICorProfilerInfo4 == nullptr && this->m_pICorProfilerInfo3 != nullptr)
	{
		 this->m_pICorProfilerInfo3->GetCurrentThreadID(threadId);
		 return S_OK;
	}
	else if (this->m_pICorProfilerInfo3 == nullptr && this->m_pICorProfilerInfo2 != nullptr)
	{
		 this->m_pICorProfilerInfo2->GetCurrentThreadID(threadId);
		 return S_OK;
	}
	else if (this->m_pICorProfilerInfo2 == nullptr && this->m_pICorProfilerInfo != nullptr)
	{
		 this->m_pICorProfilerInfo->GetCurrentThreadID(threadId);
		 return S_OK;
	}
	return E_FAIL;
	
}

STDMETHODIMP MetadataHelpers::GetArguments(FunctionID funcId, mdToken MethodDataToken)
{
	
	return S_OK;
}



STDMETHODIMP MetadataHelpers::GetMetaDataEmitInterFaceFromModule(ModuleID ModuleId, 
	const std::unique_ptr<IMetaDataEmit>& _MetaDataEmit, const std::unique_ptr<IMetaDataEmit2>& _MetaDataEmit2)
{
	if (_MetaDataEmit == nullptr)
	{
		HRESULT hr;
		if (m_pICorProfilerInfo4 != nullptr)
		{
			hr = this->m_pICorProfilerInfo4->GetModuleMetaData(ModuleId, CorOpenFlags::ofReadWriteMask,  IID_IMetaDataEmit2,
				(IUnknown **)&_MetaDataEmit2);
		}
		else if (this->m_pICorProfilerInfo4 == nullptr && this->m_pICorProfilerInfo3 != nullptr)
		{
			hr = this->m_pICorProfilerInfo4->GetModuleMetaData(ModuleId, CorOpenFlags::ofReadWriteMask, IID_IMetaDataEmit2,
				(IUnknown **)&_MetaDataEmit2);
		}
		else if (this->m_pICorProfilerInfo3 == nullptr && this->m_pICorProfilerInfo2 != nullptr)
		{

			hr = this->m_pICorProfilerInfo2->GetModuleMetaData(ModuleId, CorOpenFlags::ofReadWriteMask, IID_IMetaDataEmit2,
				(IUnknown **)&_MetaDataEmit2);
		}
		else if (this->m_pICorProfilerInfo2 == nullptr && this->m_pICorProfilerInfo != nullptr)
		{
			hr = this->m_pICorProfilerInfo->GetModuleMetaData(ModuleId, CorOpenFlags::ofReadWriteMask, IID_IMetaDataEmit,
				(IUnknown **)&_MetaDataEmit2);
		}
		return S_OK;
	}
	return E_FAIL;
}

STDMETHODIMP MetadataHelpers::GetMetaDataImportInterfaceFromModule(ModuleID ModuleId,
	const std::unique_ptr<IMetaDataImport>& _MetaDataImport,
	const std::unique_ptr<IMetaDataImport2>& _MetaDataImport2)
{

	if (_MetaDataImport == nullptr)
	{
		HRESULT hr;
		if (m_pICorProfilerInfo4 != nullptr)
		{
			hr = this->m_pICorProfilerInfo4->GetModuleMetaData(ModuleId, 0, IID_IMetaDataImport2,
				(IUnknown **)&_MetaDataImport2);
		}
		else if (this->m_pICorProfilerInfo4 == nullptr && this->m_pICorProfilerInfo3 != nullptr)
		{
			hr = this->m_pICorProfilerInfo4->GetModuleMetaData(ModuleId, 0, IID_IMetaDataImport2,
				(IUnknown **)&_MetaDataImport2);
		}
		else if (this->m_pICorProfilerInfo3 == nullptr && this->m_pICorProfilerInfo2 != nullptr)
		{

			hr = this->m_pICorProfilerInfo2->GetModuleMetaData(ModuleId, 0, IID_IMetaDataImport2,
				(IUnknown **)&_MetaDataImport2);
		}
		else if (this->m_pICorProfilerInfo2 == nullptr && this->m_pICorProfilerInfo != nullptr)
		{
			hr = this->m_pICorProfilerInfo->GetModuleMetaData(ModuleId, 0, IID_IMetaDataImport,
				(IUnknown **)&_MetaDataImport);
		}
		return S_OK;
	}
	return E_FAIL;
}


STDMETHODIMP MetadataHelpers::GetMetaDataImportInterfaceFromFunction(FunctionID funcId, mdMethodDef* funcToken,
	const std::unique_ptr<IMetaDataImport>& _MetaDataImport, const std::unique_ptr<IMetaDataImport2>& _MetaDataImport2)
{

	if (_MetaDataImport == nullptr)
	{
		HRESULT hr;
		if (m_pICorProfilerInfo4 != nullptr)
		{
			hr = this->m_pICorProfilerInfo4->GetTokenAndMetaDataFromFunction(funcId, IID_IMetaDataImport2,
				(IUnknown **)&_MetaDataImport2, funcToken);
		}
		else if (this->m_pICorProfilerInfo4 == nullptr && this->m_pICorProfilerInfo3 != nullptr)
		{
			hr = this->m_pICorProfilerInfo3->GetTokenAndMetaDataFromFunction(funcId, IID_IMetaDataImport2,
				(IUnknown **)&_MetaDataImport2, funcToken);
		}
		else if (this->m_pICorProfilerInfo3 == nullptr && this->m_pICorProfilerInfo2 != nullptr)
		{

			hr = this->m_pICorProfilerInfo2->GetTokenAndMetaDataFromFunction(funcId, IID_IMetaDataImport2,
				(IUnknown **)&_MetaDataImport2, funcToken);
		}
		else if (this->m_pICorProfilerInfo2 == nullptr && this->m_pICorProfilerInfo != nullptr)
		{
			this->m_pICorProfilerInfo->GetTokenAndMetaDataFromFunction(funcId, IID_IMetaDataImport,
				(IUnknown **)&_MetaDataImport, funcToken);
		}
		return S_OK;
	}
	return E_FAIL;
}