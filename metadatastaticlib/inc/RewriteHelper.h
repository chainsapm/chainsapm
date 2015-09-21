#pragma once
#include "ModuleMetadataHelpers.h"

class RewriteHelper
{
public:
	RewriteHelper(ModuleMetadataHelpers MetaDataHelpers);
	~RewriteHelper();
	ULONG DecompressSignature(const COR_SIGNATURE* originalSignature, ULONG originalSignaureLen, COR_SIGNATURE *newSignature, ULONG *newSignatureLen);

	void DecompressSignature(COR_SIGNATURE * &newSig, PCCOR_SIGNATURE &sigPtr);

	ULONG CompressSignature(COR_SIGNATURE* originalSignature, ULONG originalSignaureLen, COR_SIGNATURE *newSignature, ULONG *newSignatureLen);

	void CompressSignature(COR_SIGNATURE *& newSig, PCCOR_SIGNATURE & sigPtr);


private:

	void DecompressArray(COR_SIGNATURE * &newSig, PCCOR_SIGNATURE &sigPtr);

	ULONG DecompressData(COR_SIGNATURE *& newSig, PCCOR_SIGNATURE & sigPtr);

	mdToken DecompressTypeAndToken(COR_SIGNATURE * &newSig, PCCOR_SIGNATURE &sigPtr);

	void DecompressGeneric(COR_SIGNATURE * &newSig, PCCOR_SIGNATURE &sigPtr);

	void CompressData(COR_SIGNATURE *& newSig, PCCOR_SIGNATURE & sigPtr);

	void CompressTypeAndToken(COR_SIGNATURE *& newSig, PCCOR_SIGNATURE & sigPtr);

	void CompressGeneric(COR_SIGNATURE *& newSig, PCCOR_SIGNATURE & sigPtr);

	void CompressArray(COR_SIGNATURE *& newSig, PCCOR_SIGNATURE & sigPtr);

};

