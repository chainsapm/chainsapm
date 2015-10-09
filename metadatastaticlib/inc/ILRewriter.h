#pragma once
#ifndef ilrw
#define ilrw

#pragma warning( disable : 4091 )
#include <cor.h>
#include <corprof.h>
#include <corhlpr.h>
#pragma warning( default : 4091 )

#include <assert.h>

#include <map>
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
	signed          m_alterstack;
	signed          m_stackposition;
	bool			m_marked;
	bool			m_insidebranch;
	bool			m_conditional;
	bool			m_branchprevious;

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

static const BYTE s_OpCodePush[] =
{
#define Push0    0
#define Push1    1
#define PushI    1
#define PushI4   1
#define PushR4   1
#define PushI8   1
#define PushR8   1
#define PushRef  1
#define VarPush  1          // Test code doesn't call vararg fcns, so this should not be used

#define OPDEF(c,s,pop,push,args,type,l,s1,s2,flow) { push },
#include "opcode.def"
#undef OPDEF

#undef Push0   
#undef Push1   
#undef PushI   
#undef PushI4  
#undef PushR4  
#undef PushI8  
#undef PushR8  
#undef PushRef 
#undef VarPush 
	0,                   // DUMMY TO CLEAR WARNING 
};

static const BYTE s_OpCodePop[] =
{
#define Pop0    0
#define Pop1    1
#define PopI    1
#define PopI4   1
#define PopR4   1
#define PopI8   1
#define PopR8   1
#define PopRef  1
#define VarPop  1          // Test code doesn't call vararg fcns, so this should not be used

#define OPDEF(c,s,pop,push,args,type,l,s1,s2,flow) {pop},
#include "opcode.def"
#undef OPDEF

#undef Pop0   
#undef Pop1   
#undef PopI   
#undef PopI4  
#undef PopR4  
#undef PopI8  
#undef PopR8  
#undef PopRef 
#undef VarPop 
	0,                   // DUMMY TO CLEAR WARNING 
};

static char* s_OpCodeName[] =
{
#define OPDEF(c,s,pop,push,args,type,l,s1,s2,flow) s,
#include "opcode.def"
#undef OPDEF
	0,                   // DUMMY TO CLEAR WARNING 
};

typedef enum
{
	InlineBrTarget = 1,
	InlineField,
	InlineI,
	InlineI8,
	InlineMethod,
	InlineNone,
	InlineR,
	InlineSig,
	InlineString,
	InlineSwitch,
	InlineTok,
	InlineType,
	InlineVar,
	ShortInlineBrTarg,
	ShortInlineI,
	ShortInlineR,
	ShortInlineVar,
	ShortInlineBrTarget
} OPCODE_VARIABLE;

static BYTE s_OpCodeVar[] =
{
#define InlineBrTarget   1
#define InlineField      2
#define InlineI          3
#define InlineI8         4
#define InlineMethod     5
#define InlineNone       6
#define InlineR          7
#define InlineSig        8
#define InlineString     9
#define InlineSwitch     10
#define InlineTok        11
#define InlineType       12
#define InlineVar        13
#define ShortInlineBrTarg 14
#define ShortInlineI     15
#define ShortInlineR     16
#define ShortInlineVar   17
#define ShortInlineBrTarget 18

#define OPDEF(c,s,pop,push,args,type,l,s1,s2,flow) args,
#include "opcode.def"
#undef OPDEF
	0,                   // DUMMY TO CLEAR WARNING 
#undef InlineBrTarget    
#undef InlineField       
#undef InlineI           
#undef InlineI8          
#undef InlineMethod      
#undef InlineNone        
#undef InlineR           
#undef InlineSig         
#undef InlineString      
#undef InlineSwitch      
#undef InlineTok         
#undef InlineType        
#undef InlineVar         
#undef ShortInlineBrTarg 
#undef ShortInlineI     
#undef ShortInlineR     
#undef ShortInlineVar   
#undef ShortInlineBrTarget 
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

	ATL::CComPtr<IMetaDataEmit2> pMetaDataEmit;
	ATL::CComPtr<IMetaDataImport2> pMetaDataImport;

public:

	ILRewriter(ICorProfilerInfo * pICorProfilerInfo, ICorProfilerFunctionControl * pICorProfilerFunctionControl, ModuleID moduleID, mdToken tkMethod);
	ILRewriter(std::shared_ptr<ModuleMetadataHelpers> mdHelper);
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

	HRESULT Import(UINT_PTR pIL, mdSignature & LocalSig);

	HRESULT ImportIL(LPCBYTE pIL);

	HRESULT ImportEH(const COR_ILMETHOD_SECT_EH* pILEH, unsigned nEH);

	ILInstr* NewILInstr();
	ILInstr* NewILInstr(ILInstr);

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

	HRESULT WriteILToConsole(std::shared_ptr<ModuleMetadataHelpers> mdHelper, bool ShowBytes, bool ShowTokens);

	HRESULT SetILFunctionBody(unsigned size, LPBYTE pBody);

	LPBYTE AllocateILMemory(unsigned size);

	void DeallocateILMemory(LPBYTE pBody);

	HRESULT ReplaceTokens(std::shared_ptr<ModuleMetadataHelpers>  mdHelper);

	HRESULT FixUpLocals(std::shared_ptr<ModuleMetadataHelpers> mdHelper, std::map<ULONG, ULONG> &localsFixup);

	HRESULT FixUpTypes(std::shared_ptr<ModuleMetadataHelpers> mdHelper, std::map<std::wstring, std::wstring> &typeFixup);

	/////////////////////////////////////////////////////////////////////////////////////////////////
	//
	// R E W R I T E
	//
	////////////////////////////////////////////////////////////////////////////////////////////////

	// Probe_XXX are the callbacks to be called from the JITed code

	static void __fastcall Probe_LDSFLD(WCHAR * pFieldName);
	static void __fastcall Probe_SDSFLD(WCHAR * pFieldName);

	void AddILEnterProbe(ILRewriter & il);

	void AddILEnterProbe(ILRewriter & il, int SafePointOffset);

	void AddILProbe(ILInstr * pFirstIL);

	void AddILExitProbe(ILRewriter & il);

	UINT AddNewULONGLocal();

	UINT AddNewDateTimeLocal();

	WCHAR* GetNameFromToken(mdToken tk);

	void RecursiveBranchCheck(ILInstr * previousInstr, ILInstr * pInstr, std::map<int, ILInstr*>& branchstack, bool conditional);

	ILInstr * NewLDC(LPVOID p);

	std::wstring ModuleName;
	std::wstring TypeName;
	std::wstring MethodName;
};

#endif