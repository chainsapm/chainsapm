#pragma once
#include <string>
#include "ModuleMetadataHelpers.h"

class SignatureHelper
{
public:
	SignatureHelper(std::shared_ptr<ModuleMetadataHelpers> MDHelpers) :
		MetaDataHelpers(MDHelpers)	{};
	~SignatureHelper() {};

	// Decompress signature to use full ULONG and mdToken instead of compressed versions, optionally replce the token
	ULONG DecompressMethodSignature(const COR_SIGNATURE* originalSignature, ULONG originalSignaureLen, COR_SIGNATURE *newSignature, ULONG *newSignatureLen, bool ReplaceToken = false);
	// Compress signature for use in actual metadata
	ULONG CompressMethodSignature(COR_SIGNATURE* originalSignature, ULONG originalSignaureLen, COR_SIGNATURE *newSignature, ULONG *newSignatureLen);
	// [DEBUG] Used for sanity check when debugging signature issues
	PCCOR_SIGNATURE ParseElementType(PCCOR_SIGNATURE signature, std::wstring* buffer);


	ULONG CompressSignature(COR_SIGNATURE *& newSig, PCCOR_SIGNATURE & sigPtr);

	void DecompressSignature(COR_SIGNATURE * &newSig, PCCOR_SIGNATURE &sigPtr, bool ReplaceToken);

	void DecompressArray(COR_SIGNATURE * &newSig, PCCOR_SIGNATURE &sigPtr, bool ReplaceToken);

	ULONG DecompressData(COR_SIGNATURE *& newSig, PCCOR_SIGNATURE & sigPtr, bool ReplaceToken);

	mdToken DecompressTypeAndToken(COR_SIGNATURE * &newSig, PCCOR_SIGNATURE &sigPtr, bool ReplaceToken);

	void DecompressGeneric(COR_SIGNATURE * &newSig, PCCOR_SIGNATURE &sigPtr, bool ReplaceToken);

	void CompressData(COR_SIGNATURE *& newSig, PCCOR_SIGNATURE & sigPtr);

	void CompressTypeAndToken(COR_SIGNATURE *& newSig, PCCOR_SIGNATURE & sigPtr);

	void CompressGeneric(COR_SIGNATURE *& newSig, PCCOR_SIGNATURE & sigPtr);

	void CompressArray(COR_SIGNATURE *& newSig, PCCOR_SIGNATURE & sigPtr);

	std::shared_ptr<ModuleMetadataHelpers> MetaDataHelpers;
};


