#ifndef METADATAHELPERS_H
#define METADATAHELPERS_H
#include "commonstructures.h"


#define MAX_LENGTH 2048

class MetadataHelpers
{
public:
	MetadataHelpers();
	MetadataHelpers(ICorProfilerInfo *profilerInfo);
	MetadataHelpers(ICorProfilerInfo2 *profilerInfo);
	MetadataHelpers(ICorProfilerInfo3 *profilerInfo);
	MetadataHelpers(ICorProfilerInfo4 *profilerInfo);
	~MetadataHelpers();

	STDMETHOD(GetCurrentThread)(ThreadID* threadId);
	STDMETHOD(GetFunctionInformation)(FunctionID funcId, FunctionInfo* funcInfo);
	STDMETHOD(GetArguments)(FunctionID funcId, mdToken MethodDataToken);


	PCCOR_SIGNATURE ParseElementType(const std::unique_ptr<IMetaDataImport>& pMDImport, PCCOR_SIGNATURE signature, std::wstring* buffer);
private:
	// container for ICorProfilerInfo reference
	ATL::CComQIPtr<ICorProfilerInfo> m_pICorProfilerInfo;
	// container for ICorProfilerInfo2 reference
	ATL::CComQIPtr<ICorProfilerInfo2> m_pICorProfilerInfo2;
	// container for ICorProfilerInfo3 reference
	ATL::CComQIPtr<ICorProfilerInfo3> m_pICorProfilerInfo3;
	// container for ICorProfilerInfo4 reference
	ATL::CComQIPtr<ICorProfilerInfo4> m_pICorProfilerInfo4;

	void GetMetaDataInterfaceFromFunction(FunctionID funcId, mdMethodDef* funcToken, 
		const std::unique_ptr<IMetaDataImport>& _MetaDataImport, 
		const std::unique_ptr<IMetaDataImport2>& _MetaDataImport2);

	

};

#endif