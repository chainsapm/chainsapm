#include "stdafx.h"
#include "SignatureHelper.h"


//********************************************************************************
// Decompression routines
//********************************************************************************

ULONG SignatureHelper::DecompressMethodSignature(const COR_SIGNATURE * originalSignature, ULONG originalSignaureLen, COR_SIGNATURE * newSignature, ULONG * newSignatureLen, bool ReplaceToken)
{
	COR_SIGNATURE* newSig = newSignature;
	PCCOR_SIGNATURE sigPtr = originalSignature;
	auto sigStr = &(*sigPtr);
	auto sigEnd = (sigStr + originalSignaureLen);

	// Store this for use later
	CorCallingConvention callConv = (CorCallingConvention)*sigPtr;
	// Get the calling convention of this signature
	*newSig++ = CorSigUncompressCallingConv(sigPtr);

	if (callConv == IMAGE_CEE_CS_CALLCONV_GENERIC | callConv == IMAGE_CEE_CS_CALLCONV_GENERICINST)
	{
		// Special case where a generic method gets a count of generic instances before the
		// normal parameter list
		ULONG addToSigGeneric = CorSigUncompressedDataSize(sigPtr);
		ULONG paramCountGeneric = CorSigUncompressData(sigPtr);
		*(ULONG*)newSig = paramCountGeneric;
		newSig += sizeof(ULONG); // Skip the second byte telling us the number of parameters
	}

	// Get the parameter count of the signature
	// We're forcing it to be an integer here so the compression will be the same.
	ULONG addToSig = CorSigUncompressedDataSize(sigPtr);
	ULONG paramCount = CorSigUncompressData(sigPtr);
	*(ULONG*)newSig = paramCount;

	newSig += sizeof(ULONG); // Skip the second byte telling us the number of parameters

							 // Loop over the data decompressing it
							 // The idea is when we reach the top of the loop we should be at the next proper spot
							 // That means we will be at an element type, a class type, a generic type, and array, etc. 
							 // We should never be inside the middle of something like an array without being at a proper barrier

	while (sigPtr < sigEnd)
	{
		/*if (ElementType == ELEMENT_TYPE_END && sigPtr == sigEnd)
		{
		return (UINT_PTR)newSig - (UINT_PTR)newSignature;
		}*/
		// Decompress the element type and advance the sigPtr
		DecompressSignature(newSig, sigPtr, ReplaceToken);
	}
	return (UINT_PTR)newSig - (UINT_PTR)newSignature;
}

void SignatureHelper::DecompressSignature(COR_SIGNATURE * &newSig, PCCOR_SIGNATURE &sigPtr, bool ReplaceToken)
{
	CorElementType ElementType = CorSigUncompressElementType(sigPtr);
	switch (ElementType)
	{
	case ELEMENT_TYPE_VOID:
	case ELEMENT_TYPE_BOOLEAN:
	case ELEMENT_TYPE_CHAR:
	case ELEMENT_TYPE_I1:
	case ELEMENT_TYPE_U1:
	case ELEMENT_TYPE_I2:
	case ELEMENT_TYPE_U2:
	case ELEMENT_TYPE_I4:
	case ELEMENT_TYPE_U4:
	case ELEMENT_TYPE_I8:
	case ELEMENT_TYPE_U8:
	case ELEMENT_TYPE_R4:
	case ELEMENT_TYPE_R8:
	case ELEMENT_TYPE_U:
	case ELEMENT_TYPE_I:
	case ELEMENT_TYPE_OBJECT:
	case ELEMENT_TYPE_STRING:
	case ELEMENT_TYPE_TYPEDBYREF:
	case ELEMENT_TYPE_PINNED:
	case ELEMENT_TYPE_PTR:
	case ELEMENT_TYPE_BYREF:
	case ELEMENT_TYPE_END:
	case ELEMENT_TYPE_SENTINEL:
	case ELEMENT_TYPE_SZARRAY:
		*newSig++ = ElementType;
		break;
	case ELEMENT_TYPE_CLASS:
	case ELEMENT_TYPE_VALUETYPE:
	case ELEMENT_TYPE_CMOD_REQD:
	case ELEMENT_TYPE_CMOD_OPT:
		--sigPtr; // Go back one to properly grab the type (need to fix this)
		DecompressTypeAndToken(newSig, sigPtr, ReplaceToken);
		break;
	case ELEMENT_TYPE_GENERICINST:
		--sigPtr; // Go back one to properly grab the type (need to fix this)
		DecompressGeneric(newSig, sigPtr, ReplaceToken);
		break;
	case ELEMENT_TYPE_VAR:
	case ELEMENT_TYPE_MVAR:
		*newSig++ = ElementType;
		DecompressData(newSig, sigPtr, ReplaceToken); // Grab the count of generic arguments
		break;
	case ELEMENT_TYPE_ARRAY:
		--sigPtr; // Go back one to properly grab the type (need to fix this)
		DecompressArray(newSig, sigPtr, ReplaceToken);
		break;
	default:
		*newSig++ = ElementType;
		break;
	}
}

inline ULONG SignatureHelper::DecompressData(COR_SIGNATURE *& newSig, PCCOR_SIGNATURE & sigPtr, bool ReplaceToken)
{
	ULONG value = CorSigUncompressData(sigPtr);
	*(ULONG*)newSig = value;
	newSig += sizeof(ULONG);
	return value;
}

inline mdToken SignatureHelper::DecompressTypeAndToken(COR_SIGNATURE *& newSig, PCCOR_SIGNATURE & sigPtr, bool ReplaceToken)
{

	*newSig = CorSigUncompressElementType(sigPtr); // Grab the ELEMENT_TYPE_CLASS
	if (*newSig != ELEMENT_TYPE_CLASS && *newSig != ELEMENT_TYPE_VALUETYPE && *newSig != ELEMENT_TYPE_CMOD_REQD && *newSig != ELEMENT_TYPE_CMOD_OPT) // In the event we're checking an ELEMENT_TYPE_ARRAY or ELEMENT_TYPE_SZARRAY of some primative type
	{
		return *newSig++;
	}

	newSig++;
	mdToken	token = CorSigUncompressToken(sigPtr);
	if (ReplaceToken)
	{
		token = MetaDataHelpers->GetMappedToken(token);
	}
	*(mdToken*)newSig = token;
	newSig += sizeof(mdToken);
	return token;
}

inline void SignatureHelper::DecompressGeneric(COR_SIGNATURE *& newSig, PCCOR_SIGNATURE & sigPtr, bool ReplaceToken)
{
	*newSig++ = CorSigUncompressElementType(sigPtr);
	DecompressSignature(newSig, sigPtr, ReplaceToken); // Grab the type of the generic
	ULONG length = DecompressData(newSig, sigPtr, ReplaceToken); // Grab the Generic Count
	for (size_t i = 0; i < length; i++)
	{
		DecompressSignature(newSig, sigPtr, ReplaceToken);
	}

}

inline void SignatureHelper::DecompressArray(COR_SIGNATURE * &newSig, PCCOR_SIGNATURE &sigPtr, bool ReplaceToken)
{
	*newSig++ = CorSigUncompressElementType(sigPtr); // ELEMENT_TYPE_ARRAY

	DecompressSignature(newSig, sigPtr, ReplaceToken); // Revcursively get the array type 

	ULONG rank = DecompressData(newSig, sigPtr, ReplaceToken); // Grab the count

	ULONG numsizes;
	ULONG arraysize = (sizeof(ULONG) * 2 * rank);

	numsizes = DecompressData(newSig, sigPtr, ReplaceToken); // Grab the number of sizes
	if (numsizes <= rank)
	{
		for (ULONG i = 0; i < numsizes; i++)
			DecompressData(newSig, sigPtr, ReplaceToken); // Grab the actual size value

		ULONG numlower = DecompressData(newSig, sigPtr, ReplaceToken); // The number of lower bound arrays

		if (numlower <= rank)
		{
			for (ULONG i = 0; i < numlower; i++)
				DecompressData(newSig, sigPtr, ReplaceToken); //Grab the lower bound number

		}
	}

}

//********************************************************************************
// Compression routines
//********************************************************************************

ULONG SignatureHelper::CompressMethodSignature(COR_SIGNATURE * originalSignature, ULONG originalSignaureLen, COR_SIGNATURE * newSignature, ULONG * newSignatureLen)
{

	COR_SIGNATURE* newSig = newSignature;
	PCCOR_SIGNATURE sigPtr = originalSignature;
	auto sigStr = &(*sigPtr);
	auto sigEnd = (sigStr + originalSignaureLen);
	// Store this for use later
	CorCallingConvention callConv = (CorCallingConvention)*sigPtr;

	// Calling convention of this signature
	*newSig++ = *sigPtr++;

	if (callConv == IMAGE_CEE_CS_CALLCONV_GENERIC | callConv == IMAGE_CEE_CS_CALLCONV_GENERICINST)
	{
		// Special case where a generic method gets a count of generic instances before the
		// normal parameter list
		ULONG bytesToAddGeneric = CorSigCompressData(*(ULONG*)sigPtr, newSig);
		sigPtr += sizeof(ULONG);
		newSig += bytesToAddGeneric;
	}

	// Parameter count of the signature

	ULONG bytesToAdd = CorSigCompressData(*(ULONG*)sigPtr, newSig);
	sigPtr += sizeof(ULONG);
	newSig += bytesToAdd;

	// Loop over the data compressing it
	// The idea is when we reach the top of the loop we should be at the next proper spot
	// That means we will be at an element type, a class type, a generic type, and array, etc. 
	// We should never be inside the middle of something like an array without being at a proper barrier

	while (sigPtr < sigEnd)
	{
		CompressSignature(newSig, sigPtr);
	}
	return (UINT_PTR)newSig - (UINT_PTR)newSignature;
}

ULONG SignatureHelper::CompressSignature(COR_SIGNATURE *& newSig, PCCOR_SIGNATURE & sigPtr)
{
	PCCOR_SIGNATURE newSigStart = newSig;
	CorElementType ElementType = (CorElementType)*sigPtr++;
	switch (ElementType)
	{
	case ELEMENT_TYPE_VOID:
	case ELEMENT_TYPE_BOOLEAN:
	case ELEMENT_TYPE_CHAR:
	case ELEMENT_TYPE_I1:
	case ELEMENT_TYPE_U1:
	case ELEMENT_TYPE_I2:
	case ELEMENT_TYPE_U2:
	case ELEMENT_TYPE_I4:
	case ELEMENT_TYPE_U4:
	case ELEMENT_TYPE_I8:
	case ELEMENT_TYPE_U8:
	case ELEMENT_TYPE_R4:
	case ELEMENT_TYPE_R8:
	case ELEMENT_TYPE_U:
	case ELEMENT_TYPE_I:
	case ELEMENT_TYPE_OBJECT:
	case ELEMENT_TYPE_STRING:
	case ELEMENT_TYPE_TYPEDBYREF:
	case ELEMENT_TYPE_PINNED:
	case ELEMENT_TYPE_PTR:
	case ELEMENT_TYPE_BYREF:
	case ELEMENT_TYPE_END:
	case ELEMENT_TYPE_SENTINEL:
	case ELEMENT_TYPE_SZARRAY:
		*newSig++ = ElementType;
		break;
	case ELEMENT_TYPE_CLASS:
	case ELEMENT_TYPE_VALUETYPE:
	case ELEMENT_TYPE_CMOD_REQD:
	case ELEMENT_TYPE_CMOD_OPT:
		--sigPtr; // Go back one to properly grab the type (need to fix this)
		CompressTypeAndToken(newSig, sigPtr);
		break;
	case ELEMENT_TYPE_GENERICINST:
		--sigPtr; // Go back one to properly grab the type (need to fix this)
		CompressGeneric(newSig, sigPtr);
		break;
	case ELEMENT_TYPE_VAR:
	case ELEMENT_TYPE_MVAR:
		*newSig++ = ElementType;
		CompressData(newSig, sigPtr); // Grab the count of generic arguments
		break;
	case ELEMENT_TYPE_ARRAY:
		--sigPtr; // Go back one to properly grab the type (need to fix this)
		CompressArray(newSig, sigPtr);
		break;
	default:
		*newSig++ = ElementType;
		break;

	}
	return (UINT_PTR)newSig - (UINT_PTR)newSigStart;
}

inline void SignatureHelper::CompressData(COR_SIGNATURE *& newSig, PCCOR_SIGNATURE & sigPtr)
{
	ULONG bytesToAdd = CorSigCompressData(*(ULONG*)sigPtr, newSig);
	sigPtr += sizeof(ULONG);
	newSig += bytesToAdd;
}

inline void SignatureHelper::CompressTypeAndToken(COR_SIGNATURE *& newSig, PCCOR_SIGNATURE & sigPtr)
{
	ULONG bytesToAdd = CorSigCompressElementType((CorElementType)*sigPtr++, newSig); // Grab the ELEMENT_TYPE_CLASS
	if (*newSig == ELEMENT_TYPE_CLASS || *newSig == ELEMENT_TYPE_VALUETYPE || *newSig == ELEMENT_TYPE_CMOD_REQD || *newSig == ELEMENT_TYPE_CMOD_OPT) // In the event we're checking an ELEMENT_TYPE_ARRAY or ELEMENT_TYPE_SZARRAY of some primative type
	{
		newSig += bytesToAdd;
		bytesToAdd = CorSigCompressToken(*(mdToken*)sigPtr, newSig);
		sigPtr += sizeof(mdToken);
	}
	newSig += bytesToAdd;
}

inline void SignatureHelper::CompressGeneric(COR_SIGNATURE *& newSig, PCCOR_SIGNATURE & sigPtr)
{
	ULONG bytesToAdd = CorSigCompressElementType((CorElementType)*sigPtr++, newSig); // Grab the ELEMENT_TYPE_GENERICINST
	newSig += bytesToAdd;
	CompressTypeAndToken(newSig, sigPtr); // Grab the type of the generic
	ULONG length = *(ULONG*)sigPtr;
	CompressData(newSig, sigPtr); // Grab the Generic Count
	for (size_t i = 0; i < length; i++)
	{
		CompressSignature(newSig, sigPtr);
	}
}

inline void SignatureHelper::CompressArray(COR_SIGNATURE *&newSig, PCCOR_SIGNATURE &sigPtr)
{
	ULONG bytesToAdd = CorSigCompressElementType((CorElementType)*sigPtr++, newSig); // Grab the ELEMENT_TYPE_ARRAY
	newSig += bytesToAdd;

	CompressSignature(newSig, sigPtr); // Revcursively get the array type 

	ULONG rank = *(ULONG*)sigPtr;
	CompressData(newSig, sigPtr); // Compress the count

	ULONG numsizes = *(ULONG*)sigPtr;
	CompressData(newSig, sigPtr); // Compress NumSizes
	ULONG arraysize = (sizeof(ULONG) * 2 * rank);

	if (numsizes <= rank)
	{
		for (ULONG i = 0; i < numsizes; i++)
			CompressData(newSig, sigPtr); // Compress the actual size value

		ULONG numlower = *(ULONG*)sigPtr; // The number of lower bound arrays
		CompressData(newSig, sigPtr); // Compress the lower bound number
		if (numlower <= rank)
		{
			for (ULONG i = 0; i < numlower; i++)
				CompressData(newSig, sigPtr); //Grab the lower bound number

		}
	}
}

//********************************************************************************
// Debug and informational routines
//********************************************************************************

PCCOR_SIGNATURE SignatureHelper::ParseElementType(PCCOR_SIGNATURE signature, std::wstring *buffer)
{
	CComPtr<IMetaDataImport> pMetaDataImport;
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

			hr = pMetaDataImport->GetTypeDefProps(token,
				zName,
				MAX_LENGTH,
				&zLen,
				NULL,
				NULL);
			buffer->append(zName, zLen - 1);
		}
		else {
			HRESULT	hr;
			WCHAR zName[MAX_LENGTH];
			ULONG zLen;
			mdToken resolutionScope = mdTokenNil;
			hr = pMetaDataImport->GetTypeRefProps(token,
				&resolutionScope,
				zName,
				MAX_LENGTH,
				&zLen);
			buffer->append(zName, zLen - 1);
		}
		break;

	case ELEMENT_TYPE_GENERICINST:

		mdToken	genericToken;
		signature += CorSigUncompressToken(signature, &genericToken);
		if (TypeFromToken(token) != mdtTypeRef)
		{
			HRESULT	hr;
			WCHAR zName[MAX_LENGTH];
			ULONG zLen;

			hr = pMetaDataImport->GetTypeDefProps(token,
				zName,
				MAX_LENGTH,
				&zLen,
				NULL,
				NULL);
			buffer->append(zName, zLen - 1);
		}
		else {
			HRESULT	hr;
			WCHAR zName[MAX_LENGTH];
			ULONG zLen;
			mdToken resolutionScope = mdTokenNil;
			hr = pMetaDataImport->GetTypeRefProps(token,
				&resolutionScope,
				zName,
				MAX_LENGTH,
				&zLen);
			buffer->append(zName, zLen - 1);
		}
		ULONG genericCount;
		signature += CorSigUncompressData(signature, &genericCount);
		for (size_t i = 0; i < genericCount; i++)
		{
			signature = this->ParseElementType(signature, buffer);
		}
		break;

	case ELEMENT_TYPE_SZARRAY:
		signature = this->ParseElementType(signature, buffer);
		buffer->append(TEXT("[]"));
		break;


	case ELEMENT_TYPE_ARRAY:
	{
		ULONG rank;


		signature = this->ParseElementType(signature, buffer);
		rank = CorSigUncompressData(signature);
		if (rank == 0)
			buffer->append(TEXT("[?]"));

		else
		{
			ULONG *lower;
			ULONG *sizes;
			ULONG numsizes;
			ULONG arraysize = (sizeof(ULONG) * 2 * rank);


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
		signature = this->ParseElementType(signature, buffer);
		buffer->append(TEXT("pinned"));
		break;


	case ELEMENT_TYPE_PTR:
		signature = this->ParseElementType(signature, buffer);
		buffer->append(TEXT("*"));
		break;


	case ELEMENT_TYPE_BYREF:
		signature = this->ParseElementType(signature, buffer);
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

