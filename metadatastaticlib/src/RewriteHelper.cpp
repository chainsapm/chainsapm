#include "stdafx.h"
#include "RewriteHelper.h"


ULONG RewriteHelper::DecompressSignature(const COR_SIGNATURE * originalSignature, ULONG originalSignaureLen, COR_SIGNATURE * newSignature, ULONG * newSignatureLen)
{
	COR_SIGNATURE* newSig = newSignature;
	PCCOR_SIGNATURE sigPtr = originalSignature;
	auto sigStr = &(*sigPtr);
	auto sigEnd = (sigStr + originalSignaureLen);

	// Store this for use later
	CorCallingConvention callConv = (CorCallingConvention)*sigPtr;
	// Get the calling convention of this signature
	*newSig++ = CorSigUncompressCallingConv(sigPtr);

	if (callConv == IMAGE_CEE_CS_CALLCONV_GENERIC)
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
		DecompressSignature(newSig, sigPtr);
	}
	return (UINT_PTR)newSig - (UINT_PTR)newSignature;
}

//********************************************************************************
// Decompression routines
//********************************************************************************

void RewriteHelper::DecompressSignature(COR_SIGNATURE * &newSig, PCCOR_SIGNATURE &sigPtr)
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
		DecompressTypeAndToken(newSig, sigPtr);
		break;
	case ELEMENT_TYPE_GENERICINST:
		--sigPtr; // Go back one to properly grab the type (need to fix this)
		DecompressGeneric(newSig, sigPtr);
		break;
	case ELEMENT_TYPE_VAR:
	case ELEMENT_TYPE_MVAR:
		*newSig++ = ElementType;
		DecompressData(newSig, sigPtr); // Grab the count of generic arguments
		break;
	case ELEMENT_TYPE_ARRAY:
		--sigPtr; // Go back one to properly grab the type (need to fix this)
		DecompressArray(newSig, sigPtr);
		break;
	default:
		*newSig++ = ElementType;
		break;
	}
}

inline ULONG RewriteHelper::DecompressData(COR_SIGNATURE *& newSig, PCCOR_SIGNATURE & sigPtr)
{
	ULONG value = CorSigUncompressData(sigPtr);
	*(ULONG*)newSig = value;
	newSig += sizeof(ULONG);
	return value;
}

inline mdToken RewriteHelper::DecompressTypeAndToken(COR_SIGNATURE *& newSig, PCCOR_SIGNATURE & sigPtr)
{

	*newSig = CorSigUncompressElementType(sigPtr); // Grab the ELEMENT_TYPE_CLASS
	if (*newSig != ELEMENT_TYPE_CLASS && *newSig != ELEMENT_TYPE_VALUETYPE && *newSig != ELEMENT_TYPE_CMOD_REQD && *newSig != ELEMENT_TYPE_CMOD_OPT) // In the event we're checking an ELEMENT_TYPE_ARRAY or ELEMENT_TYPE_SZARRAY of some primative type
	{
		return *newSig++;
	}

	newSig++;
	mdToken	token = CorSigUncompressToken(sigPtr);
	*(mdToken*)newSig = token;
	newSig += sizeof(mdToken);
	return token;
}

inline void RewriteHelper::DecompressGeneric(COR_SIGNATURE *& newSig, PCCOR_SIGNATURE & sigPtr)
{
	*newSig++ = CorSigUncompressElementType(sigPtr);
	DecompressSignature(newSig, sigPtr); // Grab the type of the generic
	ULONG length = DecompressData(newSig, sigPtr); // Grab the Generic Count
	for (size_t i = 0; i < length; i++)
	{
		DecompressSignature(newSig, sigPtr);
	}

}

inline void RewriteHelper::DecompressArray(COR_SIGNATURE * &newSig, PCCOR_SIGNATURE &sigPtr)
{
	*newSig++ = CorSigUncompressElementType(sigPtr); // ELEMENT_TYPE_ARRAY

	DecompressSignature(newSig, sigPtr); // Revcursively get the array type 

	ULONG rank = DecompressData(newSig, sigPtr); // Grab the count

	ULONG numsizes;
	ULONG arraysize = (sizeof(ULONG) * 2 * rank);

	numsizes = DecompressData(newSig, sigPtr); // Grab the number of sizes
	if (numsizes <= rank)
	{
		for (ULONG i = 0; i < numsizes; i++)
			DecompressData(newSig, sigPtr); // Grab the actual size value

		ULONG numlower = DecompressData(newSig, sigPtr); // The number of lower bound arrays

		if (numlower <= rank)
		{
			for (ULONG i = 0; i < numlower; i++)
				DecompressData(newSig, sigPtr); //Grab the lower bound number

		}
	}

}

//********************************************************************************
// Compression routines
//********************************************************************************

ULONG RewriteHelper::CompressSignature(COR_SIGNATURE * originalSignature, ULONG originalSignaureLen, COR_SIGNATURE * newSignature, ULONG * newSignatureLen)
{

	COR_SIGNATURE* newSig = newSignature;
	PCCOR_SIGNATURE sigPtr = originalSignature;
	auto sigStr = &(*sigPtr);
	auto sigEnd = (sigStr + originalSignaureLen);
	// Store this for use later
	CorCallingConvention callConv = (CorCallingConvention)*sigPtr;

	// Calling convention of this signature
	*newSig++ = *sigPtr++;

	if (callConv == IMAGE_CEE_CS_CALLCONV_GENERIC)
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

void RewriteHelper::CompressSignature(COR_SIGNATURE *& newSig, PCCOR_SIGNATURE & sigPtr)
{
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
}


inline void RewriteHelper::CompressData(COR_SIGNATURE *& newSig, PCCOR_SIGNATURE & sigPtr)
{
	ULONG bytesToAdd = CorSigCompressData(*(ULONG*)sigPtr, newSig);
	sigPtr += sizeof(ULONG);
	newSig += bytesToAdd;
}

inline void RewriteHelper::CompressTypeAndToken(COR_SIGNATURE *& newSig, PCCOR_SIGNATURE & sigPtr)
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

inline void RewriteHelper::CompressGeneric(COR_SIGNATURE *& newSig, PCCOR_SIGNATURE & sigPtr)
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

inline void RewriteHelper::CompressArray(COR_SIGNATURE *&newSig, PCCOR_SIGNATURE &sigPtr)
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