#include <cor.h>
#include <corprof.h>

typedef struct __THREADINFORMATION
{
	ThreadID threadId;
	std::wstring *ThreadName;
	UINT parentThread;
	UINT osThread;
} ThreadInformation, *pThreadInformation;

typedef struct __SIGNATUREINFORMATION
{
	mdParamDef paramDef;
	LPWSTR paramName;
} SignatureInfo, *pSignatureInfo;

typedef struct __FUNCTIONINFORMATION
{
	FunctionID funcID;
	std::wstring *TypeName;
	std::wstring *FunctionName;
	std::wstring *FunctionSignature;
	std::wstring *FunctionReturnType;
	std::wstring *FunctionCallingConvention;
	PCCOR_SIGNATURE signature;
} FunctionInformation, *pFunctionInformation;