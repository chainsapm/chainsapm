#pragma once

#pragma warning( disable : 4091 )
#include <corhlpr.cpp>
#include <cor.h>
#include <corprof.h>
#pragma warning( default : 4091 )
#include "sigparse.inl"

#include <assert.h>

#include <map>
#include <unordered_map>
#include <stack>
#include <array>
#include <string>
#include <sstream>
#include <vector>
#include <queue>
#include <stdexcept>
#include <unordered_set>
#include <locale>
#include <memory>
#include <allocators>

#include "ModuleMetadataHelpers.h"
#pragma runtime_checks("", off)

void __fastcall UnmanagedInspectObject(void* pv)
{
	void* pv2 = pv;
}

#undef IfFailRet
#define IfFailRet(EXPR) do { HRESULT hr = (EXPR); if(FAILED(hr)) { return (hr); } } while (0)

#undef IfNullRet
#define IfNullRet(EXPR) do { if ((EXPR) == NULL) return E_OUTOFMEMORY; } while (0)

struct ILInstr
{
	ILInstr *       m_pNext;
	ILInstr *       m_pPrev;

	unsigned        m_opcode;
	unsigned        m_offset;

	union
	{
		ILInstr *   m_pTarget;
		INT8        m_Arg8;
		INT16       m_Arg16;
		INT32       m_Arg32;
		INT64       m_Arg64;
	};
};

struct EHClause
{
	CorExceptionFlag            m_Flags;
	ILInstr *                   m_pTryBegin;
	ILInstr *                   m_pTryEnd;
	ILInstr *                   m_pHandlerBegin;    // First instruction inside the handler
	ILInstr *                   m_pHandlerEnd;      // Last instruction inside the handler
	union
	{
		DWORD                   m_ClassToken;   // use for type-based exception handlers
		ILInstr *               m_pFilter;      // use for filter-based exception handlers (COR_ILEXCEPTION_CLAUSE_FILTER is set)
	};
};

typedef enum
{
#define OPDEF(c,s,pop,push,args,type,l,s1,s2,ctrl) c,
#include "opcode.def"
#undef OPDEF
	CEE_COUNT,
	CEE_SWITCH_ARG, // special internal instructions
} OPCODE;

#define OPCODEFLAGS_SizeMask        0x0F
#define OPCODEFLAGS_BranchTarget    0x10
#define OPCODEFLAGS_Switch          0x20

static const BYTE s_OpCodeFlags[] =
{
#define InlineNone           0
#define ShortInlineVar       1
#define InlineVar            2
#define ShortInlineI         1
#define InlineI              4
#define InlineI8             8
#define ShortInlineR         4
#define InlineR              8
#define ShortInlineBrTarget  1 | OPCODEFLAGS_BranchTarget
#define InlineBrTarget       4 | OPCODEFLAGS_BranchTarget
#define InlineMethod         4
#define InlineField          4
#define InlineType           4
#define InlineString         4
#define InlineSig            4
#define InlineRVA            4
#define InlineTok            4
#define InlineSwitch         0 | OPCODEFLAGS_Switch

#define OPDEF(c,s,pop,push,args,type,l,s1,s2,flow) args,
#include "opcode.def"
#undef OPDEF

#undef InlineNone
#undef ShortInlineVar
#undef InlineVar
#undef ShortInlineI
#undef InlineI
#undef InlineI8
#undef ShortInlineR
#undef InlineR
#undef ShortInlineBrTarget
#undef InlineBrTarget
#undef InlineMethod
#undef InlineField
#undef InlineType
#undef InlineString
#undef InlineSig
#undef InlineRVA
#undef InlineTok
#undef InlineSwitch
	0,                              // CEE_COUNT
	4 | OPCODEFLAGS_BranchTarget,   // CEE_SWITCH_ARG
};

static int k_rgnStackPushes[] = {

#define OPDEF(c,s,pop,push,args,type,l,s1,s2,ctrl) \
	{ push },

#define Push0    0
#define Push1    1
#define PushI    1
#define PushI4   1
#define PushR4   1
#define PushI8   1
#define PushR8   1
#define PushRef  1
#define VarPush  1          // Test code doesn't call vararg fcns, so this should not be used

#include "opcode.def"

#undef Push0   
#undef Push1   
#undef PushI   
#undef PushI4  
#undef PushR4  
#undef PushI8  
#undef PushR8  
#undef PushRef 
#undef VarPush 
#undef OPDEF
};

class ILRewriter
{
private:
	ICorProfilerInfo * m_pICorProfilerInfo;
	ICorProfilerFunctionControl * m_pICorProfilerFunctionControl;

	ModuleID    m_moduleId;
	mdToken     m_tkMethod;

	mdToken     m_tkLocalVarSig;
	unsigned    m_maxStack;
	unsigned    m_flags;
	bool        m_fGenerateTinyHeader;

	ILInstr m_IL; // Double linked list of all il instructions

	unsigned    m_nEH;
	EHClause *  m_pEH;

	// Helper table for importing.  Sparse array that maps BYTE offset of beginning of an
	// instruction to that instruction's ILInstr*.  BYTE offsets that don't correspond
	// to the beginning of an instruction are mapped to NULL.
	ILInstr **  m_pOffsetToInstr;
	unsigned    m_CodeSize;

	unsigned    m_nInstrs;

	BYTE *      m_pOutputBuffer;

	IMethodMalloc * m_pIMethodMalloc;

	IMetaDataImport * m_pMetaDataImport;
	IMetaDataEmit * m_pMetaDataEmit;

public:
	ILRewriter(ICorProfilerInfo * pICorProfilerInfo, ICorProfilerFunctionControl * pICorProfilerFunctionControl, ModuleID moduleID, mdToken tkMethod);
	ILRewriter();
	~ILRewriter();
	HRESULT Initialize();
	void InitializeTiny();

	/////////////////////////////////////////////////////////////////////////////////////////////////
	//
	// I M P O R T
	//
	////////////////////////////////////////////////////////////////////////////////////////////////

	HRESULT Import();

	HRESULT Import(ULONG pIL, std::shared_ptr<ModuleMetadataHelpers> mdHelper, mdSignature & LocalSig);

	HRESULT ImportIL(LPCBYTE pIL);

	HRESULT ImportEH(const COR_ILMETHOD_SECT_EH* pILEH, unsigned nEH);

	ILInstr* NewILInstr();

	ILInstr* GetInstrFromOffset(unsigned offset);

	void InsertBefore(ILInstr * pWhere, ILInstr * pWhat);

	void InsertAfter(ILInstr * pWhere, ILInstr * pWhat);

	void AdjustState(ILInstr * pNewInstr);

	ILInstr * GetILList();

	/////////////////////////////////////////////////////////////////////////////////////////////////
	//
	// E X P O R T
	//
	////////////////////////////////////////////////////////////////////////////////////////////////


	HRESULT Export();

	HRESULT SetILFunctionBody(unsigned size, LPBYTE pBody);

	LPBYTE AllocateILMemory(unsigned size);

	void DeallocateILMemory(LPBYTE pBody);

	HRESULT ReplaceTokens(std::shared_ptr<ModuleMetadataHelpers>  mdHelper);

	/////////////////////////////////////////////////////////////////////////////////////////////////
	//
	// R E W R I T E
	//
	////////////////////////////////////////////////////////////////////////////////////////////////

	// Probe_XXX are the callbacks to be called from the JITed code

	static void __fastcall Probe_LDSFLD(WCHAR * pFieldName);
	static void __fastcall Probe_SDSFLD(WCHAR * pFieldName);
	UINT AddNewInt32Local();

	UINT AddNewObjectArrayLocal();

	UINT AddNewString();

	WCHAR* GetNameFromToken(mdToken tk);

	ILInstr * NewLDC(LPVOID p);
};
