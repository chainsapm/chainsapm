//---------------------------------------------------------------------------------------
// Exports that managed code from ProfilerHelper.dll will P/Invoke into
// 
// NOTE: Must keep these signatures in sync with the DllImports in ProfilerHelper.cs!
//---------------------------------------------------------------------------------------

#include "stdafx.h"
#include "profilermain.h"

EXTERN_C void STDAPICALLTYPE NtvEnteredHttp(
	unsigned __int64 moduleIDCur,
	unsigned __int64 mdCur,
	WCHAR* httpString)
{
	Cprofilermain * pContainerClass = nullptr;
	pContainerClass = Cprofilermain::g_StaticContainerClass->at(0x0);
	pContainerClass->NtvEnteredFunction(moduleIDCur, mdCur, 0);
}

EXTERN_C void STDAPICALLTYPE NtvEnteredFunction(
	unsigned __int64 moduleIDCur,
	unsigned __int64 mdCur,
	int nVersionCur)
{
	Cprofilermain * pContainerClass = nullptr;
	pContainerClass = Cprofilermain::g_StaticContainerClass->at(0x0);
	pContainerClass->NtvEnteredFunction(moduleIDCur, mdCur, nVersionCur);
}

EXTERN_C void STDAPICALLTYPE NtvExitedFunction(
	unsigned __int64 moduleIDCur,
	unsigned __int64 mdCur,
	int nVersionCur)
{
	Cprofilermain * pContainerClass = nullptr;
	pContainerClass = Cprofilermain::g_StaticContainerClass->at(0x0);
	pContainerClass->NtvExitedFunction(moduleIDCur, mdCur, nVersionCur);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Global methods for specific profiler callbacks. Namely the Mapper and Enter/Leave/Tail
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Enter hook function for creating shadow stacks
EXTERN_C void FunctionEnter2_CPP_STUB(FunctionID funcId, UINT_PTR clientData,
	COR_PRF_FRAME_INFO func, COR_PRF_FUNCTION_ARGUMENT_INFO *argumentInfo)
{
	Cprofilermain * pContainerClass = (Cprofilermain*)clientData;
	if (funcId == clientData)
	{
		pContainerClass = Cprofilermain::g_StaticContainerClass->at(0x0);
	}

	pContainerClass->FunctionEnterHook2(funcId, clientData, func, argumentInfo);
	//pContainerClass->FunctionEnterHook2(funcId, clientData, func, argumentInfo);
}

// Leave hook function for creating shadow stacks
EXTERN_C void FunctionLeave2_CPP_STUB(FunctionID funcId, UINT_PTR clientData,
	COR_PRF_FRAME_INFO func, COR_PRF_FUNCTION_ARGUMENT_RANGE *argumentRange)
{
	Cprofilermain * pContainerClass = (Cprofilermain*)clientData;
	if (funcId == clientData)
	{
		pContainerClass = Cprofilermain::g_StaticContainerClass->at(0x0);
	}

	pContainerClass->FunctionLeaveHook2(funcId, clientData, func, argumentRange);
}

// Tail hook function for creating shadow stacks
EXTERN_C void FunctionTail2_CPP_STUB(FunctionID funcId, UINT_PTR clientData,
	COR_PRF_FRAME_INFO func)
{

	Cprofilermain * pContainerClass = (Cprofilermain*)clientData;
	if (funcId == clientData)
	{
		pContainerClass = Cprofilermain::g_StaticContainerClass->at(0x0);
	}

	pContainerClass->FunctionTailHook2(funcId, clientData, func);
}

#if X64
#else
#pragma warning( disable : 4102 )
#include "../src/asm/etl_enter_x86.inc"
#include "../src/asm/etl_tail_x86.inc"
#include "../src/asm/etl_leave_x86.inc"
#pragma warning( default : 4102 )
#endif