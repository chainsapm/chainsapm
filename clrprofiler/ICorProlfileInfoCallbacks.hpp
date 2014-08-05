#pragma once
#include "stdafx.h"
#include <cor.h>
#include <corprof.h>
#include "profilermain.h"
//#include "commonstructures.h"

#define NAME_BUFFER_SIZE 2048


// Global definition of profling main class to be reused when we need to call shutdown
Cprofilermain * g_ProfilerCallback = NULL;
MetadataHelpers * g_MetadataHelpers = NULL;

std::map<FunctionID, FunctionInformation> * g_FunctionSet = NULL;
std::unordered_set<std::string> * g_FunctionNameSet = NULL;

EXTERN_C void FunctionEnter2_Wrapper_x64(FunctionID funcId, UINT_PTR clientData, COR_PRF_FRAME_INFO func, COR_PRF_FUNCTION_ARGUMENT_RANGE *argumentInfo);

EXTERN_C void FunctionLeave2_Wrapper_x64(FunctionID funcId, UINT_PTR clientData, COR_PRF_FRAME_INFO func, COR_PRF_FUNCTION_ARGUMENT_RANGE *argumentInfo);

EXTERN_C void FunctionTail2_Wrapper_x64(FunctionID funcId, UINT_PTR clientData, COR_PRF_FRAME_INFO func, COR_PRF_FUNCTION_ARGUMENT_RANGE *argumentInfo);

namespace staticMethods
{


	UINT_PTR __stdcall Mapper2(FunctionID funcId, void * clientData, BOOL *pbHookFunction)
	{
		return NULL;
	}

	UINT_PTR __stdcall Mapper1(FunctionID funcId, BOOL *pbHookFunction)
	{
		std::map<FunctionID, FunctionInformation>::const_iterator it = g_FunctionSet->find(funcId);
		if (it == g_FunctionSet->end())
		{
			// declared in this block so they are not created if the function is found
			std::string s = "s";
			s.find("Console");

			std::string findString;
			FunctionInformation funcInfo;

			g_MetadataHelpers->GetFunctionInformation(funcId, &funcInfo);
			//g_ProfilerCallback->GetFullMethodName(funcId, &findString, NAME_BUFFER_SIZE);

			if (funcInfo.FunctionName->find(TEXT("WriteLine")) != std::wstring::npos )
			{
				g_FunctionSet->insert(std::pair<FunctionID, FunctionInformation>(funcId, funcInfo));
				*pbHookFunction = TRUE;
			}
			else {
				*pbHookFunction = FALSE;
			}

		}

		return (UINT_PTR)funcId;
	}
	
	/*
	MSDN Article that describes the ELT methods and what COR flags need to be set.
	http://msdn.microsoft.com/en-us/magazine/cc300553.aspx
	*/
	EXTERN_C void FunctionEnter2_CPP_Helper_x64(FunctionID funcId, UINT_PTR clientData, 
		COR_PRF_FRAME_INFO func, COR_PRF_FUNCTION_ARGUMENT_INFO *argumentInfo)
	{
		std::map<FunctionID, FunctionInformation>::const_iterator it = g_FunctionSet->find(funcId);
		if (it != g_FunctionSet->end())
		{
			std::string findString;
			g_ProfilerCallback->GetFuncArgs(funcId, func);
		}
		//TODO Implement function callbacks
	}

	EXTERN_C void FunctionLeave2_CPP_Helper_x64(FunctionID funcId, UINT_PTR clientData, 
		COR_PRF_FRAME_INFO func, COR_PRF_FUNCTION_ARGUMENT_RANGE *argumentRange)
	{
		
		//TODO Implement function callbacks
	}

	EXTERN_C void FunctionTail2_CPP_Helper_x64(FunctionID funcId, UINT_PTR clientData, 
		COR_PRF_FRAME_INFO func, COR_PRF_FUNCTION_ARGUMENT_INFO *argumentInfo)
	{
		//TODO Implement function callbacks
	}
}

