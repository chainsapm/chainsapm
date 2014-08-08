#pragma once
#include "stdafx.h"
#include "profilermain.h"
#include "MetadataHelpers.h"
#define NAME_BUFFER_SIZE 2048

Cprofilermain * g_ProfilerCallback = NULL;
MetadataHelpers * g_MetadataHelpers = NULL;

std::map<FunctionID, FunctionInfo> * g_FunctionSet = NULL;
std::map<ThreadID, std::queue<ThreadStackItem>> * g_ThreadStackMap = NULL;
std::unordered_set<std::wstring> * g_FunctionNameSet = NULL;

EXTERN_C void FunctionEnter2_Wrapper_x64(FunctionID funcId, UINT_PTR clientData, COR_PRF_FRAME_INFO func, COR_PRF_FUNCTION_ARGUMENT_RANGE *argumentInfo);

EXTERN_C void FunctionLeave2_Wrapper_x64(FunctionID funcId, UINT_PTR clientData, COR_PRF_FRAME_INFO func, COR_PRF_FUNCTION_ARGUMENT_RANGE *argumentInfo);

EXTERN_C void FunctionTail2_Wrapper_x64(FunctionID funcId, UINT_PTR clientData, COR_PRF_FRAME_INFO func, COR_PRF_FUNCTION_ARGUMENT_RANGE *argumentInfo);

namespace staticMethods
{


	UINT_PTR __stdcall Mapper1(FunctionID funcId, BOOL *pbHookFunction)
	{
		std::map<FunctionID, FunctionInfo>::const_iterator it = g_FunctionSet->find(funcId);
		
		if (it == g_FunctionSet->end())
		{
			
			// declared in this block so they are not created if the function is found
			
			std::string findString;
			FunctionInfo funcInfo;
			
			g_MetadataHelpers->GetFunctionInformation(funcId, &funcInfo);
			std::unordered_set<std::wstring>::const_iterator findName = 
				g_FunctionNameSet->find(funcInfo.FunctionName());
			if (findName != g_FunctionNameSet->end())
			{
				g_FunctionSet->insert(std::pair<FunctionID, FunctionInfo>(funcId, funcInfo));
				*pbHookFunction = TRUE;
			}
			else {
				*pbHookFunction = FALSE;
			}

		}

		return (UINT_PTR)funcId;

	}

	UINT_PTR __stdcall Mapper2(FunctionID funcId, void * clientData, BOOL *pbHookFunction)
	{
		return Mapper1(funcId, pbHookFunction);
	}

	
	
	/*
	MSDN Article that describes the ELT methods and what COR flags need to be set.
	http://msdn.microsoft.com/en-us/magazine/cc300553.aspx
	*/
	EXTERN_C void FunctionEnter2_CPP_Helper(FunctionID funcId, UINT_PTR clientData, 
		COR_PRF_FRAME_INFO func, COR_PRF_FUNCTION_ARGUMENT_INFO *argumentInfo)
	{
		TimerItem ti(ThreadStackReason::ENTER);
		std::map<FunctionID, FunctionInfo>::const_iterator it = g_FunctionSet->find(funcId);
		if (it != g_FunctionSet->end())
		{
			FunctionInfo fi = it->second;
			ThreadID threadId;
			g_MetadataHelpers->GetCurrentThread(&threadId);
			std::map<ThreadID, std::queue<ThreadStackItem>>::iterator it = g_ThreadStackMap->find(threadId);
			if (it != g_ThreadStackMap->end())
			{
				ThreadStackItem tsi(threadId, funcId, ThreadStackReason::ENTER, (byte*)argumentInfo);
				ti.AddThreadStackItem(&tsi);
				it->second.push(tsi);
				
			}
			// TODO extract argument 
		}
		//TODO Implement function callbacks
	}

	EXTERN_C void FunctionLeave2_CPP_Helper(FunctionID funcId, UINT_PTR clientData, 
		COR_PRF_FRAME_INFO func, COR_PRF_FUNCTION_ARGUMENT_RANGE *argumentRange)
	{
		TimerItem ti(ThreadStackReason::EXIT);

		std::map<FunctionID, FunctionInfo>::const_iterator it = g_FunctionSet->find(funcId);
		if (it != g_FunctionSet->end())
		{
			
			ThreadID threadId;
			g_MetadataHelpers->GetCurrentThread(&threadId);
			std::map<ThreadID, std::queue<ThreadStackItem>>::iterator itStack = g_ThreadStackMap->find(threadId);
			if (itStack != g_ThreadStackMap->end())
			{
				ThreadStackItem* tsi = &itStack->second.back();
				tsi->ItemStackReturnValue((byte*)argumentRange);
				ti.AddThreadStackItem(tsi);
			}
			// TODO extract argument 
		}
		//TODO Implement function callbacks
	}

	EXTERN_C void FunctionTail2_CPP_Helper(FunctionID funcId, UINT_PTR clientData, 
		COR_PRF_FRAME_INFO func, COR_PRF_FUNCTION_ARGUMENT_INFO *argumentInfo)
	{
		TimerItem tiOne(ThreadStackReason::TAIL);
		TimerItem tiTwo(ThreadStackReason::EXIT);
		std::map<FunctionID, FunctionInfo>::const_iterator it = g_FunctionSet->find(funcId);
		if (it != g_FunctionSet->end())
		{
			ThreadID threadId;
			g_MetadataHelpers->GetCurrentThread(&threadId);
			std::map<ThreadID, std::queue<ThreadStackItem>>::iterator it = g_ThreadStackMap->find(threadId);
			if (it != g_ThreadStackMap->end())
			{
				ThreadID threadId;
				g_MetadataHelpers->GetCurrentThread(&threadId);
				std::map<ThreadID, std::queue<ThreadStackItem>>::iterator itStack = g_ThreadStackMap->find(threadId);
				if (itStack != g_ThreadStackMap->end())
				{
					ThreadStackItem* previousItem = (((ThreadStackItem*)(&itStack->second.back())) - 1);
					tiTwo.AddThreadStackItem(previousItem);
					ThreadStackItem tsi(threadId, funcId, ThreadStackReason::TAIL, (byte*)argumentInfo);
					tiOne.AddThreadStackItem(&tsi);
					it->second.push(tsi);
					
				}
			}
			// TODO extract argument 
		}
		//TODO Implement function callbacks
	}
}

