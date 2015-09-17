#pragma once
#include "ModuleMetadataHelpers.h"
class RewriteHelper
{
public:
	RewriteHelper(ModuleMetadataHelpers MetaDataHelpers);
	~RewriteHelper();
	HRESULT DecompressSignature(COR_SIGNATURE* originalSignature, ULONG originalSignaureLen, COR_SIGNATURE *newSignature, ULONG *newSignatureLen);
	void DecompressArray(COR_SIGNATURE * &newSig, PCCOR_SIGNATURE &sigPtr);
	HRESULT CompressSignature(COR_SIGNATURE* originalSignature, ULONG originalSignaureLen, COR_SIGNATURE *newSignature, ULONG *newSignatureLen);

private:
	std::shared_ptr<ModuleMetadataHelpers> m_MetadataHelper;

	ULONG DecompressData(PCCOR_SIGNATURE &sigPtr, COR_SIGNATURE * &newSig);

	mdToken DecompressTypeAndToken(COR_SIGNATURE * &newSig, PCCOR_SIGNATURE &sigPtr);

	void DecompressGeneric(COR_SIGNATURE * &newSig, const CorElementType &val, PCCOR_SIGNATURE &sigPtr);

};

