#include "stdafx.h"
#include "MetadataHelpers.h"


MetadataHelpers::MetadataHelpers()
{
	// TODO Check to see if containers are valid.
}

MetadataHelpers::MetadataHelpers(ICorProfilerInfo *profilerInfo) : MetadataHelpers()
{
	m_pICorProfilerInfo = profilerInfo;
}

MetadataHelpers::MetadataHelpers(ICorProfilerInfo2 *profilerInfo) : MetadataHelpers()
{
	m_pICorProfilerInfo2 = profilerInfo;
}

MetadataHelpers::MetadataHelpers(ICorProfilerInfo3 *profilerInfo) : MetadataHelpers()
{
	m_pICorProfilerInfo3 = profilerInfo;
}

MetadataHelpers::MetadataHelpers(ICorProfilerInfo4 *profilerInfo) : MetadataHelpers()
{
	m_pICorProfilerInfo4 = profilerInfo;
}


MetadataHelpers::~MetadataHelpers()
{
	if (m_pICorProfilerInfo != NULL)
	{
		delete m_pICorProfilerInfo;
	}
	if (m_pICorProfilerInfo2 != NULL)
	{
		delete m_pICorProfilerInfo2;
	}
	if (m_pICorProfilerInfo3 != NULL)
	{
		delete m_pICorProfilerInfo3;
	}
	if (m_pICorProfilerInfo4 != NULL)
	{
		delete m_pICorProfilerInfo4;
	}

}

STDMETHODIMP MetadataHelpers::GetFunctionInformation(FunctionID funcId, pFunctionInformation funcInfo)
{
	std::shared_ptr<IMetaDataImport> _MetaDataImport;
	std::shared_ptr<IMetaDataImport2> _MetaDataImport2;

	mdMethodDef funcToken;


	HRESULT hr;
	if (m_pICorProfilerInfo4.p != NULL)
	{
		hr = m_pICorProfilerInfo4->GetTokenAndMetaDataFromFunction(funcId, IID_IMetaDataImport2,
			(IUnknown **)&_MetaDataImport2, &funcToken);
		if (SUCCEEDED(hr))
		{
			_MetaDataImport = _MetaDataImport2;
		}
		else {
			DWORD err = GetLastError();
		}

	}
	else if (m_pICorProfilerInfo4.p == NULL && m_pICorProfilerInfo3.p != NULL)
	{
		hr = m_pICorProfilerInfo3->GetTokenAndMetaDataFromFunction(funcId, IID_IMetaDataImport2,
			(IUnknown **)&_MetaDataImport2, &funcToken);

		if (SUCCEEDED(hr))
		{
			_MetaDataImport = _MetaDataImport2;
		}
		else {
			DWORD err = GetLastError();
		}
	}
	else if (m_pICorProfilerInfo3.p == NULL && m_pICorProfilerInfo2.p != NULL)
	{

		hr = m_pICorProfilerInfo2->GetTokenAndMetaDataFromFunction(funcId, IID_IMetaDataImport2,
			(IUnknown **)&_MetaDataImport2, &funcToken);
		if (SUCCEEDED(hr))
		{
			_MetaDataImport = _MetaDataImport2;
		}
		else {
			DWORD err = GetLastError();
		}
	}
	else if (m_pICorProfilerInfo2.p == NULL && m_pICorProfilerInfo.p != NULL)
	{
		m_pICorProfilerInfo->GetTokenAndMetaDataFromFunction(funcId, IID_IMetaDataImport,
			(IUnknown **)&_MetaDataImport, &funcToken);
	}

	if (&_MetaDataImport != NULL)
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
		WCHAR szMethodName[2048];
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
			2048,
			&stringLen,
			&methodAttr,
			&sigBlob,
			&sigBlobBytes,
			&codeRVA,
			&implFlags
			);
		DWORD NumberOfParams = sigBlobBytes;
		funcInfo->funcID = funcId;
		funcInfo->FunctionName = new std::wstring(szMethodName);
		funcInfo->FunctionSignature = new std::wstring();
		funcInfo->signature = PCCOR_SIGNATURE(sigBlob); // Create a clone
		/*
		MSDN link for GetTypeDefProps
		http://msdn.microsoft.com/en-us/library/ms230143(v=vs.110).aspx

		Use this to get they type of an object. In this case we are getting the class
		name.
		*/
		WCHAR szClassName[2048];
		DWORD flags;
		ULONG typeDefPointer;
		mdToken extends;

		hr = _MetaDataImport->GetTypeDefProps(
			classTypeDef,
			szClassName,
			2048,
			&typeDefPointer,
			&flags,
			&extends);

		funcInfo->FunctionReturnType = new std::wstring();
		funcInfo->TypeName = new std::wstring(szClassName);

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
					funcInfo->FunctionCallingConvention = new std::wstring(callConvNames[callConv & 7]);
				//
				// Grab the argument count
				//
				sigBlob += CorSigUncompressData(sigBlob, &argCount);

				//
				// Get the return type
				//
				sigBlob = ParseElementType(_MetaDataImport, sigBlob, funcInfo->FunctionReturnType);

				//
				// if the return type returned back empty, write void
				//
				if (funcInfo->FunctionReturnType->length() == 0)
					wsprintf((LPWSTR)funcInfo->FunctionReturnType->c_str(), TEXT("void"));

				//
				// Get the parameters
				//								
				for (ULONG i = 0;
					(SUCCEEDED(hr) && (sigBlob != NULL) && (i < (argCount)));
					i++)
				{
					if (i != 0)
					{
						funcInfo->FunctionSignature->append(TEXT(", "));
					}

					sigBlob = ParseElementType(_MetaDataImport, sigBlob, funcInfo->FunctionSignature);

					mdParamDef paramDef;
					mdMethodDef funcTokenOut;
					ULONG sequence;
					DWORD paramAttributes;
					DWORD cplusTypeFlags;
					WCHAR szParamName[2048];
					ULONG szParamNameLen;
					UVCP_CONSTANT constVal = new UVCP_CONSTANT();
					ULONG constValueLen;

					_MetaDataImport->GetParamForMethodIndex(funcToken, i + 1, &paramDef);

					_MetaDataImport->GetParamProps(paramDef,
						&funcTokenOut,
						&sequence,
						szParamName,
						2048,
						&szParamNameLen,
						&paramAttributes,
						&cplusTypeFlags,
						&constVal,
						&constValueLen);


					funcInfo->FunctionSignature->append(szParamName, szParamNameLen - 1);

					delete constVal;

				}
			}
		}


		//	if (NumberOfParams > 0)
		//	{
		//
		//		funcInfo->FunctionSignature = new std::wstring();
		//		for (DWORD parameterOrdinal = 1; parameterOrdinal <= NumberOfParams; parameterOrdinal++)
		//		{
		//			mdParamDef paramDef;
		//			mdMethodDef funcTokenOut;
		//			ULONG sequence;
		//			DWORD paramAttributes;
		//			DWORD cplusTypeFlags;
		//			WCHAR szParamName[2048];
		//			ULONG szParamNameLen;
		//			UVCP_CONSTANT constVal = new UVCP_CONSTANT();
		//			ULONG constValueLen;
		//
		//			_MetaDataImport->GetParamForMethodIndex(funcToken, parameterOrdinal, &paramDef);
		//			CorElementType elementType = (CorElementType)sigBlob[parameterOrdinal - 1];
		//
		//			_MetaDataImport->GetParamProps(paramDef,
		//				&funcTokenOut,
		//				&sequence,
		//				szParamName,
		//				2048,
		//				&szParamNameLen,
		//				&paramAttributes,
		//				&cplusTypeFlags,
		//				&constVal,
		//				&constValueLen);
		//			if (parameterOrdinal != 1)
		//			{
		//				funcInfo->FunctionSignature->append(TEXT(", "));
		//			}
		//
		//			funcInfo->FunctionSignature->append(szParamName, szParamNameLen - 1);
		//
		//			delete constVal;
		//		}
		//	}
		//}
		//else {
		//	return E_FAIL;
		//}

		/*if (_MetaDataImport != NULL)
		{
			delete _MetaDataImport;
		}

		if (_MetaDataImport2 != NULL)
		{
			_MetaDataImport2->Release();
		}*/
		return S_OK;
	}
}

/* static public */
PCCOR_SIGNATURE MetadataHelpers::ParseElementType(std::shared_ptr<IMetaDataImport>  pMDImport, PCCOR_SIGNATURE signature, std::wstring *buffer)
{
	switch (*signature++)
	{
	case ELEMENT_TYPE_VOID:
		buffer->append(TEXT("void "));
		break;


	case ELEMENT_TYPE_BOOLEAN:
		buffer->append(TEXT("bool "));
		break;


	case ELEMENT_TYPE_CHAR:
		buffer->append(TEXT("wchar "));
		break;


	case ELEMENT_TYPE_I1:
		buffer->append(TEXT("int8 "));
		break;


	case ELEMENT_TYPE_U1:
		buffer->append(TEXT("unsigned int8 "));
		break;


	case ELEMENT_TYPE_I2:
		buffer->append(TEXT("int16 "));
		break;


	case ELEMENT_TYPE_U2:
		buffer->append(TEXT("unsigned int16 "));
		break;


	case ELEMENT_TYPE_I4:
		buffer->append(TEXT("int32 "));
		break;


	case ELEMENT_TYPE_U4:
		buffer->append(TEXT("unsigned int32 "));
		break;


	case ELEMENT_TYPE_I8:
		buffer->append(TEXT("int64 "));
		break;


	case ELEMENT_TYPE_U8:
		buffer->append(TEXT("unsigned int64 "));
		break;


	case ELEMENT_TYPE_R4:
		buffer->append(TEXT("float32 "));
		break;


	case ELEMENT_TYPE_R8:
		buffer->append(TEXT("float64 "));
		break;


	case ELEMENT_TYPE_U:
		buffer->append(TEXT("unsigned int "));
		break;


	case ELEMENT_TYPE_I:
		buffer->append(TEXT("int "));
		break;


	case ELEMENT_TYPE_OBJECT:
		buffer->append(TEXT("Object "));
		break;


	case ELEMENT_TYPE_STRING:
		buffer->append(TEXT("String "));
		break;


	case ELEMENT_TYPE_TYPEDBYREF:
		buffer->append(TEXT("refany "));
		break;

	case ELEMENT_TYPE_CLASS:
	case ELEMENT_TYPE_VALUETYPE:
	case ELEMENT_TYPE_CMOD_REQD:
	case ELEMENT_TYPE_CMOD_OPT:

		mdToken	token;
		WCHAR classname[MAX_LENGTH];

		classname[0] = '\0';
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

