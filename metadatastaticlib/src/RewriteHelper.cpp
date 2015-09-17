#include "stdafx.h"
#include "RewriteHelper.h"

HRESULT RewriteHelper::DecompressSignature(COR_SIGNATURE * originalSignature, ULONG originalSignaureLen, COR_SIGNATURE * newSignature, ULONG * newSignatureLen)
{
	COR_SIGNATURE* newSig = newSignature;
	PCCOR_SIGNATURE sigPtr = originalSignature;
	auto sigStr = &(*sigPtr);
	auto sigEnd = (sigStr + originalSignaureLen);
	// Get the calling convention of this signature
	*newSig++ = CorSigUncompressCallingConv(sigPtr);
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

	do
	{
		// Decompress the element type and advance the sigPtr
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
			--newSig; // Go back one to properly grab the type (need to fix this)
			DecompressTypeAndToken(newSig, sigPtr);
			break;
		case ELEMENT_TYPE_GENERICINST:
			*newSig++ = ElementType;
			DecompressTypeAndToken(newSig, sigPtr); // Determine the type of the generic
			DecompressData(sigPtr, newSig); // Grab the count
			break;
		case ELEMENT_TYPE_ARRAY:
		{
			*newSig++ = ElementType;
			DecompressArray(newSig, sigPtr);
		}
		break;
		}
	} while (sigPtr < sigEnd);
	return E_NOTIMPL;
}

void RewriteHelper::DecompressArray(COR_SIGNATURE * &newSig, PCCOR_SIGNATURE &sigPtr)
{
	DecompressTypeAndToken(newSig, sigPtr); // Determine the type of the array
	ULONG rank = DecompressData(sigPtr, newSig); // Grab the count

	ULONG numsizes;
	ULONG arraysize = (sizeof(ULONG) * 2 * rank);

	numsizes = DecompressData(sigPtr, newSig); // Grab the number of sizes
	if (numsizes <= rank)
	{
		for (ULONG i = 0; i < numsizes; i++)
			DecompressData(sigPtr, newSig); // Grab the actual size value

		ULONG numlower = DecompressData(sigPtr, newSig); // The number of lower bound arrays

		if (numlower <= rank)
		{
			for (ULONG i = 0; i < numlower; i++)
				DecompressData(sigPtr, newSig); //Grab the lower bound number

		}
	}
}

inline ULONG RewriteHelper::DecompressData(PCCOR_SIGNATURE & sigPtr, COR_SIGNATURE *& newSig)
{
	ULONG value = CorSigUncompressData(sigPtr);
	*(ULONG*)newSig = value;
	newSig += sizeof(ULONG);
	return value;
}

inline mdToken RewriteHelper::DecompressTypeAndToken(COR_SIGNATURE *& newSig, PCCOR_SIGNATURE & sigPtr)
{
	
	*newSig++ = CorSigUncompressElementType(sigPtr); // Grab the ELEMENT_TYPE_CLASS
	if (*newSig != ELEMENT_TYPE_CLASS) // In the event we're checking an ELEMENT_TYPE_ARRAY or ELEMENT_TYPE_SZARRAY of some primative type
	{
		return *newSig;
	}
	mdToken	token = CorSigUncompressToken(sigPtr);
	*(mdToken*)newSig = token;
	newSig += sizeof(mdToken);
	return token;
}

inline void RewriteHelper::DecompressGeneric(COR_SIGNATURE *& newSig, const CorElementType & val, PCCOR_SIGNATURE & sigPtr)
{
	DecompressTypeAndToken(newSig, sigPtr); // Grab the type of the generic
	ULONG length = DecompressData(sigPtr, newSig); // Grab the Generic Count
}
