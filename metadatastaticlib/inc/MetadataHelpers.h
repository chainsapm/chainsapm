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


class ModuleMetadataHelpers
{
public:
	ModuleMetadataHelpers(std::shared_ptr<ICorProfilerInfo> profilerInfo, ModuleID moduleID);
	ModuleMetadataHelpers(std::shared_ptr<ICorProfilerInfo2> profilerInfo, ModuleID moduleID);
	ModuleMetadataHelpers(std::shared_ptr<ICorProfilerInfo3> profilerInfo, ModuleID moduleID);
	ModuleMetadataHelpers(std::shared_ptr<ICorProfilerInfo4> profilerInfo, ModuleID moduleID);
	~ModuleMetadataHelpers();

	std::wstring& GetModuleName();
	mdMethodDef GetMethodMdTokenFromName(std::wstring& TypeName, std::wstring& MethodName);
	mdMethodDef GetMethodMdTokenFromNameAndSig(std::wstring& TypeName, std::wstring& MethodName, COR_SIGNATURE* Signature);
	STDMETHOD(GetCurrentThread)(ThreadID* threadId);
	STDMETHOD(GetFunctionInformation)(FunctionID funcId, InformationClasses::FunctionInfo* funcInfo);
	STDMETHOD(GetClassInformation)(mdTypeDef classMD, InformationClasses::ClassInfo* classInfo);
	STDMETHOD(GetModuleInformation)(mdTypeDef moduleMD, InformationClasses::ModuleInfo* moduleInfo);
	STDMETHOD(GetAssemblyInformation)(mdTypeDef assemblyMD, InformationClasses::AssemblyInfo* assemblyInfo);
	STDMETHOD(GetArguments)(FunctionID funcId, mdToken MethodDataToken);


	STDMETHOD(InjectFieldToModule)(const mdTypeDef& classTypeDef,
		const std::wstring& fieldName);

	STDMETHOD(GetMetaDataEmitInterFaceFromModule)(const std::unique_ptr<IMetaDataEmit>& _MetaDataEmit, const std::unique_ptr<IMetaDataEmit2>& _MetaDataEmit2);

	STDMETHOD(GetMetaDataImportInterfaceFromModule)(const std::unique_ptr<IMetaDataImport>& _MetaDataImport,
		const std::unique_ptr<IMetaDataImport2>& _MetaDataImport2);


	PCCOR_SIGNATURE ParseElementType(const std::unique_ptr<IMetaDataImport>& pMDImport, PCCOR_SIGNATURE signature, std::wstring* buffer);
private:
	ModuleID m_moduleID;
	// container for ICorProfilerInfo reference
	std::shared_ptr<ICorProfilerInfo> m_pICorProfilerInfo;
	// container for ICorProfilerInfo2 reference
	std::shared_ptr<ICorProfilerInfo2> m_pICorProfilerInfo2;
	// container for ICorProfilerInfo3 reference
	std::shared_ptr<ICorProfilerInfo3> m_pICorProfilerInfo3;
	// container for ICorProfilerInfo4 reference
	std::shared_ptr<ICorProfilerInfo4> m_pICorProfilerInfo4;

	std::unique_ptr<IMetaDataEmit> pMetaDataEmit;
	std::unique_ptr<IMetaDataImport> pMetaDataImport;
	std::unique_ptr<IMetaDataAssemblyEmit> pMetaDataAssemblyEmit;
	std::unique_ptr<IMetaDataAssemblyImport> pMetaDataAssemblyImport;
	//boost::mutex m_ThreadIdMutex;
	CRITICAL_SECTION m_ThreadCS;
	// Saving reference to the containers in order to provide specific function lookups, eg. Is this method an entry point

};

#endif
