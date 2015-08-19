#ifndef METADATAHELPERS_H
#define METADATAHELPERS_H
#include <cor.h>
#include <corprof.h>
#include <string>
#include <map>
#include <stack>
#include <memory>
#include "allinfo.h"


#define MAX_LENGTH 2048


class MetadataHelpers
{
public:
	MetadataHelpers();
	MetadataHelpers(std::shared_ptr<ICorProfilerInfo> profilerInfo);
	MetadataHelpers(std::shared_ptr<ICorProfilerInfo2> profilerInfo);
	MetadataHelpers(std::shared_ptr<ICorProfilerInfo3> profilerInfo);
	MetadataHelpers(std::shared_ptr<ICorProfilerInfo4> profilerInfo);
	~MetadataHelpers();

	STDMETHOD(GetCurrentThread)(ThreadID* threadId);
	STDMETHOD(GetFunctionInformation)(FunctionID funcId, InformationClasses::FunctionInfo* funcInfo);
	STDMETHOD(GetClassInformation)(mdTypeDef classMD, InformationClasses::ClassInfo* classInfo);
	STDMETHOD(GetModuleInformation)(mdTypeDef moduleMD, InformationClasses::ModuleInfo* moduleInfo);
	STDMETHOD(GetAssemblyInformation)(mdTypeDef assemblyMD, InformationClasses::AssemblyInfo* assemblyInfo);
	STDMETHOD(GetArguments)(FunctionID funcId, mdToken MethodDataToken);


	STDMETHOD(InjectFieldToModule)(const ModuleID& ModuleId, const mdTypeDef& classTypeDef,
		const std::wstring& fieldName);

	STDMETHOD(GetMetaDataEmitInterFaceFromModule)(ModuleID ModuleId,
		const std::unique_ptr<IMetaDataEmit>& _MetaDataEmit, const std::unique_ptr<IMetaDataEmit2>& _MetaDataEmit2);

	STDMETHOD(GetMetaDataImportInterfaceFromModule)(ModuleID ModuleId,
		const std::unique_ptr<IMetaDataImport>& _MetaDataImport,
		const std::unique_ptr<IMetaDataImport2>& _MetaDataImport2);

	STDMETHOD(GetMetaDataImportInterfaceFromFunction)(FunctionID funcId, mdMethodDef* funcToken,
		const std::unique_ptr<IMetaDataImport>& _MetaDataImport,
		const std::unique_ptr<IMetaDataImport2>& _MetaDataImport2);

	PCCOR_SIGNATURE ParseElementType(const std::unique_ptr<IMetaDataImport>& pMDImport, PCCOR_SIGNATURE signature, std::wstring* buffer);
private:
	// container for ICorProfilerInfo reference
	std::shared_ptr<ICorProfilerInfo> m_pICorProfilerInfo;
	// container for ICorProfilerInfo2 reference
	std::shared_ptr<ICorProfilerInfo2> m_pICorProfilerInfo2;
	// container for ICorProfilerInfo3 reference
	std::shared_ptr<ICorProfilerInfo3> m_pICorProfilerInfo3;
	// container for ICorProfilerInfo4 reference
	std::shared_ptr<ICorProfilerInfo4> m_pICorProfilerInfo4;
	//boost::mutex m_ThreadIdMutex;
	CRITICAL_SECTION m_ThreadCS;
	// Saving reference to the containers in order to provide specific function lookups, eg. Is this method an entry point

};

#endif
