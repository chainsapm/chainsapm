#include <cor.h>
#include <corprof.h>
#include <map>
#include <memory>
#include "commonstructures.h"

#define MAX_LENGTH 2048
#pragma once
class MetadataHelpers
{
public:
	MetadataHelpers();
	MetadataHelpers(ICorProfilerInfo *profilerInfo);
	MetadataHelpers(ICorProfilerInfo2 *profilerInfo);
	MetadataHelpers(ICorProfilerInfo3 *profilerInfo);
	MetadataHelpers(ICorProfilerInfo4 *profilerInfo);
	~MetadataHelpers();

	STDMETHOD(GetFunctionInformation)(FunctionID funcId, pFunctionInformation funcInfo);
	PCCOR_SIGNATURE ParseElementType(std::shared_ptr<IMetaDataImport> pMDImport, PCCOR_SIGNATURE signature, std::wstring *buffer);
private:
	// container for ICorProfilerInfo reference
	ATL::CComQIPtr<ICorProfilerInfo> m_pICorProfilerInfo;
	// container for ICorProfilerInfo2 reference
	ATL::CComQIPtr<ICorProfilerInfo2> m_pICorProfilerInfo2;
	// container for ICorProfilerInfo3 reference
	ATL::CComQIPtr<ICorProfilerInfo3> m_pICorProfilerInfo3;
	// container for ICorProfilerInfo4 reference
	ATL::CComQIPtr<ICorProfilerInfo4> m_pICorProfilerInfo4;

	// Map of function information
	std::map<FunctionID, pFunctionInformation> m_MethodLookupMap;
	



};

