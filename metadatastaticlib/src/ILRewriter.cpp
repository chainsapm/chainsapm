// ==++==
// 
//   Copyright (c) Microsoft Corporation.  All rights reserved.
// 
// ==--==
// 
// #include this cpp file to get the definition and implementation of the ILRewriter
// class.  This class contains a lot of general-purpose IL rewriting functionality,
// which parses an IL stream into a structured linked list of IL instructions.  This
// list can then be manipulated (items added / removed), and then rewritten back into an
// IL stream, with things like branches automatically updated.
// 
// Refer to the C functions at the bottom of this file for examples of how the
// ILRewriter class can be used
// 

#pragma once
#include "ILRewriter.h"

ILRewriter::ILRewriter(ICorProfilerInfo * pICorProfilerInfo, ICorProfilerFunctionControl * pICorProfilerFunctionControl, ModuleID moduleID, mdToken tkMethod)
	: m_pICorProfilerInfo(pICorProfilerInfo), m_pICorProfilerFunctionControl(pICorProfilerFunctionControl),
	m_moduleId(moduleID), m_tkMethod(tkMethod), m_fGenerateTinyHeader(false),
	m_pEH(NULL), m_pOffsetToInstr(NULL), m_pOutputBuffer(NULL), m_pIMethodMalloc(NULL),
	pMetaDataImport(NULL), pMetaDataEmit(NULL)
{
	m_IL.m_pNext = &m_IL;
	m_IL.m_pPrev = &m_IL;

	m_nInstrs = 0;
}

ILRewriter::ILRewriter(std::shared_ptr<ModuleMetadataHelpers> mdHelper)
	: m_fGenerateTinyHeader(false),
	m_pEH(NULL), m_pOffsetToInstr(NULL), m_pOutputBuffer(NULL), m_pIMethodMalloc(NULL)
{
	pMetaDataEmit = mdHelper->pMetaDataEmit;
	pMetaDataImport = mdHelper->pMetaDataImport;

	m_IL.m_pNext = &m_IL;
	m_IL.m_pPrev = &m_IL;

	m_nInstrs = 0;
}

ILRewriter::~ILRewriter()
{
	ILInstr * p = m_IL.m_pNext;
	while (p != &m_IL)
	{
		ILInstr * t = p->m_pNext;
		delete p;
		p = t;
	}
	delete[] m_pEH;
	delete[] m_pOffsetToInstr;
	delete[] m_pOutputBuffer;

	if (m_pIMethodMalloc)
		m_pIMethodMalloc->Release();
	if (pMetaDataImport)
		pMetaDataImport.Release();
	if (pMetaDataEmit)
		pMetaDataEmit.Release();
}

HRESULT ILRewriter::Initialize()
{
	// Get metadata interfaces ready

	IfFailRet(m_pICorProfilerInfo->GetModuleMetaData(
		m_moduleId, ofRead | ofWrite, IID_IMetaDataImport, (IUnknown**)&pMetaDataImport));

	IfFailRet(pMetaDataImport->QueryInterface(IID_IMetaDataEmit, (void **)&pMetaDataEmit));

	return S_OK;
}

void ILRewriter::InitializeTiny()
{
	m_tkLocalVarSig = 0;
	m_maxStack = 8;
	m_flags = CorILMethod_TinyFormat;
	m_CodeSize = 0;
	m_nEH = 0;
	m_fGenerateTinyHeader = true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//
// I M P O R T
//
////////////////////////////////////////////////////////////////////////////////////////////////

HRESULT ILRewriter::Import()
{
	LPCBYTE pMethodBytes;

	IfFailRet(m_pICorProfilerInfo->GetILFunctionBody(
		m_moduleId, m_tkMethod, &pMethodBytes, NULL));

	COR_ILMETHOD_DECODER decoder((COR_ILMETHOD*)pMethodBytes);

	// Import the header flags
	m_tkLocalVarSig = decoder.GetLocalVarSigTok();
	m_maxStack = decoder.GetMaxStack();
	m_flags = (decoder.GetFlags() & CorILMethod_InitLocals);

	m_CodeSize = decoder.GetCodeSize();

	IfFailRet(ImportIL(decoder.Code));

	IfFailRet(ImportEH(decoder.EH, decoder.EHCount()));

	return S_OK;
}

HRESULT ILRewriter::Import(UINT_PTR pIL, mdSignature & LocalSig)
{
	COR_ILMETHOD_DECODER decoder((COR_ILMETHOD*)pIL);

	// Import the header flags
	m_tkLocalVarSig = decoder.GetLocalVarSigTok();
	LocalSig = m_tkLocalVarSig;
	m_maxStack = decoder.GetMaxStack();
	m_flags = (decoder.GetFlags() & CorILMethod_InitLocals);

	m_CodeSize = decoder.GetCodeSize();

	IfFailRet(ImportIL(decoder.Code));

	IfFailRet(ImportEH(decoder.EH, decoder.EHCount()));

	return S_OK;
}

HRESULT ILRewriter::ImportIL(LPCBYTE pIL)
{
	m_pOffsetToInstr = new ILInstr*[m_CodeSize + 1];
	IfNullRet(m_pOffsetToInstr);

	ZeroMemory(m_pOffsetToInstr, m_CodeSize * sizeof(ILInstr*));

	// Set the sentinel instruction
	m_pOffsetToInstr[m_CodeSize] = &m_IL;
	m_IL.m_opcode = 0xFFFFFFFF;

	bool fBranch = false;
	unsigned offset = 0;

	BYTE stackPosition = 0;

	while (offset < m_CodeSize)
	{
		unsigned startOffset = offset;
		unsigned opcode = pIL[offset++];

		if (opcode == CEE_PREFIX1)
		{
			if (offset >= m_CodeSize)
			{
				assert(false);
				return COR_E_INVALIDPROGRAM;
			}
			opcode = 0x100 + pIL[offset++];
		}

		if ((CEE_PREFIX7 <= opcode) && (opcode <= CEE_PREFIX2))
		{
			// NOTE: CEE_PREFIX2-7 are currently not supported
			assert(false);
			return COR_E_INVALIDPROGRAM;
		}

		if (opcode >= CEE_COUNT)
		{
			assert(false);
			return COR_E_INVALIDPROGRAM;
		}

		BYTE flags = s_OpCodeFlags[opcode];
		BYTE push = s_OpCodePush[opcode];
		BYTE pop = s_OpCodePop[opcode];

		int size = (flags & OPCODEFLAGS_SizeMask);
		if (offset + size > m_CodeSize)
		{
			assert(false);
			return COR_E_INVALIDPROGRAM;
		}

		ILInstr * pInstr = NewILInstr();
		IfNullRet(pInstr);

		pInstr->m_opcode = opcode;
		pInstr->m_offset = offset;

		InsertBefore(&m_IL, pInstr);

		m_pOffsetToInstr[startOffset] = pInstr;

		switch (flags)
		{
		case 0:
			break;
		case 1:
			pInstr->m_Arg8 = *(UNALIGNED INT8 *)&(pIL[offset]);
			break;
		case 2:
			pInstr->m_Arg16 = *(UNALIGNED INT16 *)&(pIL[offset]);
			break;
		case 4:
			pInstr->m_Arg32 = *(UNALIGNED INT32 *)&(pIL[offset]);
			break;
		case 8:
			pInstr->m_Arg64 = *(UNALIGNED INT64 *)&(pIL[offset]);
			break;
		case 1 | OPCODEFLAGS_BranchTarget:
			pInstr->m_Arg32 = offset + 1 + *(UNALIGNED INT8 *)&(pIL[offset]);

			fBranch = true;
			break;
		case 4 | OPCODEFLAGS_BranchTarget:
			pInstr->m_Arg32 = offset + 4 + *(UNALIGNED INT32 *)&(pIL[offset]);
			fBranch = true;
			break;
		case 0 | OPCODEFLAGS_Switch:
		{
			if (offset + sizeof(INT32) > m_CodeSize)
			{
				assert(false);
				return COR_E_INVALIDPROGRAM;
			}

			unsigned nTargets = *(UNALIGNED INT32 *)&(pIL[offset]);
			pInstr->m_Arg32 = nTargets;
			offset += sizeof(INT32);

			unsigned base = offset + nTargets * sizeof(INT32);

			for (unsigned iTarget = 0; iTarget < nTargets; iTarget++)
			{
				if (offset + sizeof(INT32) > m_CodeSize)
				{
					assert(false);
					return COR_E_INVALIDPROGRAM;
				}

				pInstr = NewILInstr();
				IfNullRet(pInstr);

				pInstr->m_opcode = CEE_SWITCH_ARG;

				pInstr->m_Arg32 = base + *(UNALIGNED INT32 *)&(pIL[offset]);
				offset += sizeof(INT32);

				InsertBefore(&m_IL, pInstr);
			}
			fBranch = true;
			break;
		}
		default:
			assert(false);
			break;
		}
		offset += size;

		if (pInstr->m_Arg32 < 0)
		{
			pInstr->m_branchprevious = true;
		}


		pInstr->m_insidebranch = true;
		mdToken parentClass = mdTokenNil;
		wchar_t methodDefNameBuffer[255];
		ULONG numChars = 0;
		DWORD attrFlags = 0;
		PCCOR_SIGNATURE originalSignature = NULL;
		ULONG originalMethodSignatureLen = 0;
		ULONG RVA;
		DWORD impFlags;
		int stackPosition = 0;
		pInstr->m_stackposition = 0;
		switch (opcode)
		{
		case CEE_CALL:
		case CEE_CALLI:
		case CEE_CALLVIRT:
			switch (TypeFromToken(pInstr->m_Arg32))
			{
			case mdtMethodDef:
				pMetaDataImport->GetMethodProps(
					pInstr->m_Arg32,
					&parentClass,
					methodDefNameBuffer,
					_countof(methodDefNameBuffer),
					&numChars,
					&attrFlags,
					&originalSignature,
					&originalMethodSignatureLen,
					&RVA,
					&impFlags);
				break;
			case mdtMemberRef:
				pMetaDataImport->GetMemberRefProps(
					pInstr->m_Arg32,
					&parentClass,
					methodDefNameBuffer,
					_countof(methodDefNameBuffer),
					&numChars,
					&originalSignature,
					&originalMethodSignatureLen);
				break;
			case mdtMethodSpec:
				pMetaDataImport->GetMethodSpecProps(
					pInstr->m_Arg32,
					&parentClass,
					&originalSignature,
					&originalMethodSignatureLen);
				break;
			default:
				break;
			}
			if (originalMethodSignatureLen > 2)
			{
				int i = 2;
				if ((originalSignature[0] & IMAGE_CEE_CS_CALLCONV_GENERIC) | (originalSignature[0] & IMAGE_CEE_CS_CALLCONV_GENERICINST))
				{
					i = 3;
				}
				if ((attrFlags & mdStatic) != mdStatic)
				{
					stackPosition -= 1;
				}
				stackPosition -= (originalSignature[i - 1]);
				if ((originalSignature[i] == ELEMENT_TYPE_VOID))
				{
					stackPosition += 0;
				}
				else {
					stackPosition += 1;
				}
			}
			break;
		default:
			stackPosition = (push + (-pop));
			break;
		}
		pInstr->m_alterstack = stackPosition;

	}

	assert(offset == m_CodeSize);
	std::map<int, ILInstr*> branchStack;
	if (fBranch)
	{
		// Go over all control flow instructions and resolve the targets
		for (ILInstr * pInstr = m_IL.m_pNext; pInstr != &m_IL; pInstr = pInstr->m_pNext)
		{
			if (s_OpCodeFlags[pInstr->m_opcode] & OPCODEFLAGS_BranchTarget)
			{
				pInstr->m_pTarget = GetInstrFromOffset(pInstr->m_Arg32);
				if ((pInstr->m_opcode != CEE_LEAVE) | (pInstr->m_opcode != CEE_LEAVE_S))
				{
					if (branchStack.find(pInstr->m_offset) == branchStack.end())
					{
						if (branchStack.find(pInstr->m_pTarget->m_offset) == branchStack.end())
						{
							switch (pInstr->m_opcode)
							{
							case CEE_BR:
							case CEE_BR_S:
							case CEE_BRTRUE:
							case CEE_BRTRUE_S:
								pInstr->m_pTarget->m_conditional = false;
								break;
							default:
								pInstr->m_pTarget->m_conditional = true;
								break;
							}
							branchStack.emplace(pInstr->m_pTarget->m_offset, pInstr->m_pTarget);
						}
					}


				}
			}

		}
		int branchdepth = 0;
		int stackdepth = 0;
		for (ILInstr * pInstr = m_IL.m_pNext; pInstr != &m_IL; pInstr = pInstr->m_pNext)
		{
			
			if (s_OpCodeFlags[pInstr->m_opcode] & OPCODEFLAGS_BranchTarget)
			{
				if (pInstr->m_opcode != CEE_LEAVE && pInstr->m_opcode != CEE_LEAVE_S)
				{
					
					switch (pInstr->m_opcode)
					{
					case CEE_BR:
					case CEE_BR_S:
						break;
					default:
						
						stackdepth = 0;
						branchdepth++;
						pInstr = pInstr->m_pNext;
						break;
					}
					
				}
			}
			if (branchStack.find(pInstr->m_offset) != branchStack.end())
			{
				stackdepth = 0;
				if (!pInstr->m_conditional)
				{
					branchdepth--;
				}
				
			}
			stackdepth += pInstr->m_alterstack;
			pInstr->m_stackposition = stackdepth;
			pInstr->m_insidebranch = (branchdepth > 0);
		}
	}



	return S_OK;
}

void ILRewriter::RecursiveBranchCheck(ILInstr * previousInstr, ILInstr * pInstr, std::map<int, ILInstr*>& branchStack, bool conditional)
{
	int	stackposition = 0;
	bool inside = conditional;
	bool cond = false;
	if (previousInstr != NULL)
	{
		inside = true;
	}
	for (; pInstr != &m_IL; pInstr = pInstr->m_pNext)
	{
		pInstr->m_marked = true;
		if ((s_OpCodeFlags[pInstr->m_opcode] & OPCODEFLAGS_BranchTarget))
		{
			switch (pInstr->m_opcode)
			{
			case CEE_BR:
			case CEE_BR_S:
				cond = false;
				break;
			default:
				cond = true;
				break;
			}
			if (!pInstr->m_pTarget->m_marked)
			{
				pInstr->m_pTarget->m_insidebranch = cond;
				pInstr->m_pTarget->m_marked = true;
				RecursiveBranchCheck(pInstr, pInstr->m_pTarget->m_pNext, branchStack, cond);
			}
			pInstr = pInstr->m_pNext;
		}

		if (branchStack.find(pInstr->m_offset) != branchStack.end())
		{
			pInstr->m_insidebranch = inside;
			if (previousInstr != NULL)
			{
				pInstr = previousInstr;
				return;
			}

		}
		pInstr->m_insidebranch = inside;
	}
}

HRESULT ILRewriter::ImportEH(const COR_ILMETHOD_SECT_EH* pILEH, unsigned nEH)
{
	assert(m_pEH == NULL);

	m_nEH = nEH;

	if (nEH == 0)
		return S_OK;

	IfNullRet(m_pEH = new EHClause[m_nEH]);
	for (unsigned iEH = 0; iEH < m_nEH; iEH++)
	{
		// If the EH clause is in tiny form, the call to pILEH->EHClause() below will
		// use this as a scratch buffer to expand the EH clause into its fat form.
		COR_ILMETHOD_SECT_EH_CLAUSE_FAT scratch;

		const COR_ILMETHOD_SECT_EH_CLAUSE_FAT* ehInfo;
		ehInfo = (COR_ILMETHOD_SECT_EH_CLAUSE_FAT*)pILEH->EHClause(iEH, &scratch);

		EHClause* clause = &(m_pEH[iEH]);
		clause->m_Flags = ehInfo->GetFlags();

		clause->m_pTryBegin = GetInstrFromOffset(ehInfo->GetTryOffset());
		clause->m_pTryEnd = GetInstrFromOffset(ehInfo->GetTryOffset() + ehInfo->GetTryLength());
		clause->m_pHandlerBegin = GetInstrFromOffset(ehInfo->GetHandlerOffset());
		clause->m_pHandlerEnd = GetInstrFromOffset(ehInfo->GetHandlerOffset() + ehInfo->GetHandlerLength())->m_pPrev;
		if ((clause->m_Flags & COR_ILEXCEPTION_CLAUSE_FILTER) == 0)
			clause->m_ClassToken = ehInfo->GetClassToken();
		else
			clause->m_pFilter = GetInstrFromOffset(ehInfo->GetFilterOffset());
	}

	return S_OK;
}

ILInstr* ILRewriter::NewILInstr(ILInstr pInstr)
{
	m_nInstrs++;
	return new ILInstr(pInstr);
}

ILInstr* ILRewriter::NewILInstr()
{
	m_nInstrs++;
	return new ILInstr();
}

ILInstr* ILRewriter::GetInstrFromOffset(unsigned offset)
{
	ILInstr * pInstr = NULL;

	if (offset <= m_CodeSize)
		pInstr = m_pOffsetToInstr[offset];

	assert(pInstr != NULL);
	return pInstr;
}

void ILRewriter::InsertBefore(ILInstr * pWhere, ILInstr * pWhat)
{
	pWhat->m_pNext = pWhere;
	pWhat->m_pPrev = pWhere->m_pPrev;

	pWhat->m_pNext->m_pPrev = pWhat;
	pWhat->m_pPrev->m_pNext = pWhat;

	AdjustState(pWhat);
}

void ILRewriter::InsertAfter(ILInstr * pWhere, ILInstr * pWhat)
{
	pWhat->m_pNext = pWhere->m_pNext;
	pWhat->m_pPrev = pWhere;

	pWhat->m_pNext->m_pPrev = pWhat;
	pWhat->m_pPrev->m_pNext = pWhat;

	AdjustState(pWhat);
}

void ILRewriter::AdjustState(ILInstr * pNewInstr)
{
	m_maxStack += k_rgnStackPushes[pNewInstr->m_opcode];
}

ILInstr * ILRewriter::GetILList()
{
	return &m_IL;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//
// E X P O R T
//
////////////////////////////////////////////////////////////////////////////////////////////////


HRESULT ILRewriter::Export()
{
	// One instruction produces 6 bytes in the worst case
	unsigned maxSize = m_nInstrs * 6;

	m_pOutputBuffer = new BYTE[maxSize];
	IfNullRet(m_pOutputBuffer);

again:
	BYTE * pIL = m_pOutputBuffer;

	bool fBranch = false;
	unsigned offset = 0;

	// Go over all instructions and produce code for them
	for (ILInstr * pInstr = m_IL.m_pNext; pInstr != &m_IL; pInstr = pInstr->m_pNext)
	{
		pInstr->m_offset = offset;

		unsigned opcode = pInstr->m_opcode;
		if (opcode < CEE_COUNT)
		{
			// CEE_PREFIX1 refers not to instruction prefixes (like tail.), but to
			// the lead byte of multi-byte opcodes. For now, the only lead byte
			// supported is CEE_PREFIX1 = 0xFE.
			if (opcode >= 0x100)
				m_pOutputBuffer[offset++] = CEE_PREFIX1;

			// This appears to depend on an implicit conversion from
			// unsigned opcode down to BYTE, to deliberately lose data and have
			// opcode >= 0x100 wrap around to 0.
			m_pOutputBuffer[offset++] = (opcode & 0xFF);
		}

		assert(pInstr->m_opcode < _countof(s_OpCodeFlags));
		BYTE flags = s_OpCodeFlags[pInstr->m_opcode];
		switch (flags)
		{
		case 0:
			break;
		case 1:
			*(UNALIGNED INT8 *)&(pIL[offset]) = pInstr->m_Arg8;
			break;
		case 2:
			*(UNALIGNED INT16 *)&(pIL[offset]) = pInstr->m_Arg16;
			break;
		case 4:
			*(UNALIGNED INT32 *)&(pIL[offset]) = pInstr->m_Arg32;
			break;
		case 8:
			*(UNALIGNED INT64 *)&(pIL[offset]) = pInstr->m_Arg64;
			break;
		case 1 | OPCODEFLAGS_BranchTarget:
			fBranch = true;
			break;
		case 4 | OPCODEFLAGS_BranchTarget:
			fBranch = true;
			break;
		case 0 | OPCODEFLAGS_Switch:
			*(UNALIGNED INT32 *)&(pIL[offset]) = pInstr->m_Arg32;
			offset += sizeof(INT32);
			break;
		default:
			assert(false);
			break;
		}
		offset += (flags & OPCODEFLAGS_SizeMask);
	}
	m_IL.m_offset = offset;

	if (fBranch)
	{
		bool fTryAgain = false;
		unsigned switchBase = 0;

		// Go over all control flow instructions and resolve the targets
		for (ILInstr * pInstr = m_IL.m_pNext; pInstr != &m_IL; pInstr = pInstr->m_pNext)
		{
			unsigned opcode = pInstr->m_opcode;

			if (pInstr->m_opcode == CEE_SWITCH)
			{
				switchBase = pInstr->m_offset + 1 + sizeof(INT32) * (pInstr->m_Arg32 + 1);
				continue;
			}
			if (opcode == CEE_SWITCH_ARG)
			{
				// Switch args are special
				*(UNALIGNED INT32 *)&(pIL[pInstr->m_offset]) = pInstr->m_pTarget->m_offset - switchBase;
				continue;
			}

			BYTE flags = s_OpCodeFlags[pInstr->m_opcode];

			if (flags & OPCODEFLAGS_BranchTarget)
			{
				int delta = pInstr->m_pTarget->m_offset - pInstr->m_pNext->m_offset;

				switch (flags)
				{
				case 1 | OPCODEFLAGS_BranchTarget:
					// Check if delta is too big to fit into an INT8.
					// 
					// (see #pragma at top of file)
					if ((INT8)delta != delta)
					{
						if (opcode == CEE_LEAVE_S)
						{
							pInstr->m_opcode = CEE_LEAVE;
						}
						else
						{
							assert(opcode >= CEE_BR_S && opcode <= CEE_BLT_UN_S);
							pInstr->m_opcode = opcode - CEE_BR_S + CEE_BR;
							assert(pInstr->m_opcode >= CEE_BR && pInstr->m_opcode <= CEE_BLT_UN);
						}
						fTryAgain = true;
						continue;
					}
					*(UNALIGNED INT8 *)&(pIL[pInstr->m_pNext->m_offset - sizeof(INT8)]) = (INT8)delta;
					break;
				case 4 | OPCODEFLAGS_BranchTarget:
					*(UNALIGNED INT32 *)&(pIL[pInstr->m_pNext->m_offset - sizeof(INT32)]) = (INT8)delta;
					break;
				default:
					assert(false);
					break;
				}
			}
		}

		// Do the whole thing again if we changed the size of some branch targets
		if (fTryAgain)
			goto again;
	}

	unsigned codeSize = offset;
	unsigned totalSize;
	LPBYTE pBody = NULL;
	if (m_fGenerateTinyHeader)
	{
		// Make sure we can fit in a tiny header
		if (codeSize >= 64)
			return E_FAIL;

		totalSize = sizeof(IMAGE_COR_ILMETHOD_TINY) + codeSize;
		pBody = AllocateILMemory(totalSize);
		IfNullRet(pBody);

		BYTE * pCurrent = pBody;

		// Here's the tiny header
		*pCurrent = (BYTE)(CorILMethod_TinyFormat | (codeSize << 2));
		pCurrent += sizeof(IMAGE_COR_ILMETHOD_TINY);

		// And the body
		CopyMemory(pCurrent, m_pOutputBuffer, codeSize);
	}
	else
	{
		// Use FAT header

		unsigned alignedCodeSize = (offset + 3) & ~3;

		totalSize = sizeof(IMAGE_COR_ILMETHOD_FAT) + alignedCodeSize +
			(m_nEH ? (sizeof(IMAGE_COR_ILMETHOD_SECT_FAT) + sizeof(IMAGE_COR_ILMETHOD_SECT_EH_CLAUSE_FAT) * m_nEH) : 0);

		pBody = AllocateILMemory(totalSize);
		IfNullRet(pBody);

		BYTE * pCurrent = pBody;

		IMAGE_COR_ILMETHOD_FAT *pHeader = (IMAGE_COR_ILMETHOD_FAT *)pCurrent;
		pHeader->Flags = m_flags | (m_nEH ? CorILMethod_MoreSects : 0) | CorILMethod_FatFormat;
		pHeader->Size = sizeof(IMAGE_COR_ILMETHOD_FAT) / sizeof(DWORD);
		pHeader->MaxStack = m_maxStack;
		pHeader->CodeSize = offset;
		pHeader->LocalVarSigTok = m_tkLocalVarSig;

		pCurrent = (BYTE*)(pHeader + 1);

		CopyMemory(pCurrent, m_pOutputBuffer, codeSize);
		pCurrent += alignedCodeSize;

		if (m_nEH != 0)
		{
			IMAGE_COR_ILMETHOD_SECT_FAT *pEH = (IMAGE_COR_ILMETHOD_SECT_FAT *)pCurrent;
			pEH->Kind = CorILMethod_Sect_EHTable | CorILMethod_Sect_FatFormat;
			pEH->DataSize = (unsigned)(sizeof(IMAGE_COR_ILMETHOD_SECT_FAT) + sizeof(IMAGE_COR_ILMETHOD_SECT_EH_CLAUSE_FAT) * m_nEH);

			pCurrent = (BYTE*)(pEH + 1);

			for (unsigned iEH = 0; iEH < m_nEH; iEH++)
			{
				EHClause *pSrc = &(m_pEH[iEH]);
				IMAGE_COR_ILMETHOD_SECT_EH_CLAUSE_FAT * pDst = (IMAGE_COR_ILMETHOD_SECT_EH_CLAUSE_FAT *)pCurrent;

				pDst->Flags = pSrc->m_Flags;
				pDst->TryOffset = pSrc->m_pTryBegin->m_offset;
				pDst->TryLength = pSrc->m_pTryEnd->m_offset - pSrc->m_pTryBegin->m_offset;
				pDst->HandlerOffset = pSrc->m_pHandlerBegin->m_offset;
				pDst->HandlerLength = pSrc->m_pHandlerEnd->m_pNext->m_offset - pSrc->m_pHandlerBegin->m_offset;
				if ((pSrc->m_Flags & COR_ILEXCEPTION_CLAUSE_FILTER) == 0)
					pDst->ClassToken = pSrc->m_ClassToken;
				else
					pDst->FilterOffset = pSrc->m_pFilter->m_offset;

				pCurrent = (BYTE*)(pDst + 1);
			}
		}
	}

	IfFailRet(SetILFunctionBody(totalSize, pBody));
	DeallocateILMemory(pBody);

	return S_OK;
}

HRESULT ILRewriter::WriteILToConsole(std::shared_ptr<ModuleMetadataHelpers> mdHelper, bool ShowBytes, bool ShowTokens)
{
	PCCOR_SIGNATURE signature = NULL;
	ULONG sigLen = 0;
	bool isPrefix = false;
	bool isPrefixMulti = false;
	bool wasPrefix = false;
	std::wstring prefixbuffer;
	std::wstring beforeitall;
	wchar_t prefixBuffer[512];
	const wchar_t* stringout = NULL;

	for (ILInstr * pInstr = m_IL.m_pNext; pInstr != &m_IL; pInstr = pInstr->m_pNext)
	{
		ZeroMemory(prefixBuffer, 512 * sizeof(WCHAR));
		switch (pInstr->m_opcode)
		{
		case CEE_VOLATILE:
		case CEE_TAILCALL:
		case CEE_CONSTRAINED:
		case CEE_UNALIGNED:
		case CEE_READONLY:
			wasPrefix = true;
			if (isPrefix)
			{
				isPrefixMulti = true;
			}
			else {
				isPrefix = true;
			}

			break;
		default:
			isPrefix = false;
			isPrefixMulti = false;
			break;
		}

		if (isPrefix && !isPrefixMulti)
		{
			wsprintf(prefixBuffer, L"IL_%04x:  %S", pInstr->m_offset, s_OpCodeName[pInstr->m_opcode]);
			prefixbuffer.append(prefixBuffer);
			continue;
		}
		else if (isPrefix && isPrefixMulti)
		{
			wsprintf(prefixBuffer, L"%S", s_OpCodeName[pInstr->m_opcode]);
			prefixbuffer.append(prefixBuffer);
			continue;
		}
		else if (wasPrefix)
		{
			wsprintf(prefixBuffer, L"%S", s_OpCodeName[pInstr->m_opcode]);
			prefixbuffer.append(prefixBuffer);
		}
		else {
			wsprintf(prefixBuffer, L"IL_%04x:  %S", pInstr->m_offset, s_OpCodeName[pInstr->m_opcode]);
			prefixbuffer.append(prefixBuffer);
		}
		wprintf(prefixbuffer.c_str());

		switch (s_OpCodeVar[pInstr->m_opcode])
		{

		case InlineBrTarget:
			wprintf(L" IL_%04x\n", pInstr->m_pTarget->m_offset);
			break;

		case 	InlineMethod:
		case 	InlineField:
		case 	InlineTok:
		case 	InlineType:
			wprintf(L" %s //(%#010x)\n", mdHelper->GetFullyQualifiedName(pInstr->m_Arg32, &signature, &sigLen).c_str(), pInstr->m_Arg32);
			break;
		case 	InlineString:
			wprintf(L" \"%s\" //(%#010x)\n", mdHelper->GetFullyQualifiedName(pInstr->m_Arg32, &signature, &sigLen).c_str(), pInstr->m_Arg32);
			break;
		case 	ShortInlineI:
		case 	InlineI:
			wprintf(L" %#010x\n", pInstr->m_Arg32);
			break;
		case 	InlineR:
		case 	ShortInlineR:
		case 	InlineI8:
			wprintf(L" %#010x %#010x\n", (int)((pInstr->m_Arg64 & 0xFFFFFFFF00000000) >> 32), (int)(pInstr->m_Arg64 & 0xFFFFFFFF));
			break;
		case 	InlineNone:
			wprintf(L"\n");
			break;
		case 	InlineSig:
			wprintf(L" %#010x\n", pInstr->m_Arg64);
			break;
		case 	InlineSwitch:
			wprintf(L" IL_%04x\n", pInstr->m_Arg32);
			break;
		case 	InlineVar:
			wprintf(L" %#010x\n", pInstr->m_Arg32);
			break;
		case 	ShortInlineBrTarg:
		case    ShortInlineBrTarget:
			wprintf(L" IL_%04x\n", pInstr->m_pTarget->m_offset);
			break;
		case 	ShortInlineVar:
			wprintf(L" %#x\n", pInstr->m_Arg16);
			break;
		default:
			wprintf(L"\n");
			break;
		}
		wasPrefix = false;
		prefixbuffer.clear();
	}
	return S_OK;
}

HRESULT ILRewriter::SetILFunctionBody(unsigned size, LPBYTE pBody)
{
	if (m_pICorProfilerFunctionControl != NULL)
	{
		// We're supplying IL for a rejit, so use the rejit mechanism
		IfFailRet(m_pICorProfilerFunctionControl->SetILFunctionBody(size, pBody));
	}
	else if (m_pICorProfilerInfo != NULL)
	{
		// "classic-style" instrumentation on first JIT, so use old mechanism
		IfFailRet(m_pICorProfilerInfo->SetILFunctionBody(m_moduleId, m_tkMethod, pBody));
	}

	return S_OK;
}

LPBYTE ILRewriter::AllocateILMemory(unsigned size)
{
	if (m_pICorProfilerFunctionControl != NULL)
	{
		// We're supplying IL for a rejit, so we can just allocate from
		// the heap
		return new BYTE[size];
	}

	if (m_pICorProfilerInfo == NULL && m_pICorProfilerFunctionControl == NULL)
	{
		return new BYTE[size];
	}
	// Else, this is "classic-style" instrumentation on first JIT, and
	// need to use the CLR's IL allocator

	if (FAILED(m_pICorProfilerInfo->GetILFunctionBodyAllocator(m_moduleId, &m_pIMethodMalloc)))
		return NULL;

	return (LPBYTE)m_pIMethodMalloc->Alloc(size);
}

void ILRewriter::DeallocateILMemory(LPBYTE pBody)
{
	if (m_pICorProfilerFunctionControl == NULL)
	{
		// Old-style instrumentation does not provide a way to free up bytes
		return;
	}

	delete[] pBody;
}

HRESULT ILRewriter::ReplaceTokens(std::shared_ptr<ModuleMetadataHelpers> mdHelper)
{
	for (ILInstr * pInstr = m_IL.m_pNext; pInstr != &m_IL; pInstr = pInstr->m_pNext)
	{
		switch (pInstr->m_opcode)
		{
		case CEE_BOX:
		case CEE_CALL:
		case CEE_CALLI:
		case CEE_CALLVIRT:
		case CEE_CASTCLASS:
		case CEE_CPOBJ:
		case CEE_INITOBJ:
		case CEE_ISINST:
		case CEE_JMP:
		case CEE_LDELEM:
		case CEE_LDFTN:
		case CEE_LDOBJ:
		case CEE_LDSFLD:
		case CEE_LDSFLDA:
		case CEE_LDTOKEN:
		case CEE_LDVIRTFTN:
		case CEE_NEWARR:
		case CEE_NEWOBJ:
		case CEE_REFANYVAL:
		case CEE_SIZEOF:
		case CEE_STELEM:
		case CEE_STFLD:
		case CEE_STOBJ:
		case CEE_STSFLD:
		case CEE_UNBOX:
		case CEE_UNBOX_ANY:
			pInstr->m_Arg32 = mdHelper->GetMappedToken(pInstr->m_Arg32);
		default:
			break;
		}
	}
	return S_OK;
}

HRESULT ILRewriter::FixUpLocals(std::shared_ptr<ModuleMetadataHelpers> mdHelper, std::map<ULONG, ULONG> &localsFixup)
{
	if (!localsFixup.empty())
	{

		for (ILInstr * pInstr = m_IL.m_pNext; pInstr != &m_IL; pInstr = pInstr->m_pNext)
		{
			switch (pInstr->m_opcode)
			{
			case CEE_LDLOC:
			case CEE_LDLOC_0:
			case CEE_LDLOC_1:
			case CEE_LDLOC_2:
			case CEE_LDLOC_3:
			case CEE_STLOC_0:
			case CEE_STLOC_1:
			case CEE_STLOC_2:
			case CEE_STLOC_3:
			case CEE_LDLOC_S:
			case CEE_LDLOCA_S:
			case CEE_STLOC_S:
				pInstr->m_Arg32 = localsFixup[pInstr->m_Arg32];
				break;
			default:
				break;
			}
		}
	}

	return S_OK;
}

HRESULT ILRewriter::FixUpTypes(std::shared_ptr<ModuleMetadataHelpers> mdHelper, std::map<std::wstring, std::wstring> &typeFixup)
{
	m_tkLocalVarSig = mdHelper->GetMappedToken(m_tkLocalVarSig);
	std::wstringstream namebuffer;
	std::wstringstream originalmemberbuffer;
	std::wstring originalfullname;
	std::wstring originalmodandtype;
	std::wstring originalmember;
	std::wstring fullname;
	std::wstring module;
	std::wstring assembly;
	std::wstring type;
	std::wstring member;
	char throwAway;
	WCHAR mod[255];
	WCHAR assem[255];
	WCHAR typ[255];
	WCHAR mem[255];
	PCCOR_SIGNATURE signature = NULL;
	ULONG sigLen = 0;
	mdToken newTokenOut;
	bool matchFound = false;
	bool replaceAllTypes = false;

	for (ILInstr * pInstr = m_IL.m_pNext; pInstr != &m_IL; pInstr = pInstr->m_pNext)
	{
		switch (pInstr->m_opcode)
		{
		case CEE_BOX:
		case CEE_CALL:
		case CEE_CALLI:
		case CEE_CALLVIRT:
		case CEE_CASTCLASS:
		case CEE_CPOBJ:
		case CEE_INITOBJ:
		case CEE_ISINST:
		case CEE_JMP:
		case CEE_LDELEM:
		case CEE_LDFTN:
		case CEE_LDOBJ:
		case CEE_LDSFLD:
		case CEE_LDSFLDA:
		case CEE_LDTOKEN:
		case CEE_LDVIRTFTN:
		case CEE_NEWARR:
		case CEE_NEWOBJ:
		case CEE_REFANYVAL:
		case CEE_SIZEOF:
		case CEE_STELEM:
		case CEE_STFLD:
		case CEE_STOBJ:
		case CEE_STSFLD:
		case CEE_UNBOX:
		case CEE_UNBOX_ANY:

			originalfullname.assign(mdHelper->GetFullyQualifiedName(pInstr->m_Arg32, &signature, &sigLen));
			originalmemberbuffer << originalfullname;
			std::getline(originalmemberbuffer, originalmodandtype, L':');
			originalmemberbuffer.get();
			std::getline(originalmemberbuffer, originalmember);

			if (typeFixup.find(originalfullname) != typeFixup.end())
			{
				matchFound = true;
			}
			else {
				if (typeFixup.find(originalmodandtype) != typeFixup.end())
				{
					matchFound = true;
					replaceAllTypes = true;
				}
			}

			if (matchFound)
			{
				namebuffer << typeFixup[fullname];
				namebuffer.get(); // Adavance 1 characater to remove '['
				if (fullname.find(L"!", 0) != std::wstring::npos)
				{
					std::getline(namebuffer, module, L'!');
				}
				std::getline(namebuffer, assembly, L']');

				if (fullname.find(L"::", 0) != std::wstring::npos)
				{
					std::getline(namebuffer, type, L':');
					namebuffer.get(); // Adavance 1 characater to remove ':'
					std::getline(namebuffer, member);
				}
				else {
					std::getline(namebuffer, type);
				}
				if (!member.empty())
				{
					if (mdHelper->FindMemberDefOrRef(module, type, member, signature, sigLen, newTokenOut) != S_OK)
					{
						mdHelper->AddMemberRefOrDef(type, member, signature, sigLen, newTokenOut, module, NULL, std::wstring());
					};
				}
				else {
					if (!originalmember.empty())
					{
						if (mdHelper->FindMemberDefOrRef(module, type, originalmember, signature, sigLen, newTokenOut) != S_OK)
						{
							mdHelper->AddMemberRefOrDef(type, originalmember, signature, sigLen, newTokenOut, module, NULL, std::wstring());
						};
					}
					if (mdHelper->FindTypeDefOrRef(module, type, newTokenOut) != S_OK)
					{
						mdHelper->AddTypeDefOrRef(type, newTokenOut, module);
					}
				}
			}
			else {
				newTokenOut = mdHelper->GetMappedToken(pInstr->m_Arg32);
			}
			pInstr->m_Arg32 = newTokenOut;
			break;

		default:
			break;
		}
		originalmemberbuffer.clear();
		originalfullname.clear();
		originalmodandtype.clear();
		originalmember.clear();
		fullname.clear();
		module.clear();
		assembly.clear();
		type.clear();
		member.clear();
	}
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//
// R E W R I T E
//
////////////////////////////////////////////////////////////////////////////////////////////////

// Probe_XXX are the callbacks to be called from the JITed code

void __fastcall ILRewriter::Probe_LDSFLD(WCHAR * pFieldName)
{
	printf("LDSFLD: %S\n", pFieldName);
}

void __fastcall ILRewriter::Probe_SDSFLD(WCHAR * pFieldName)
{
	printf("STSFLD: %S\n", pFieldName);
}

void ILRewriter::AddILEnterProbe(ILRewriter & il) {
	AddILEnterProbe(il, 0);
}

void ILRewriter::AddILEnterProbe(ILRewriter & il, int SafePointOffset) {
	ILInstr * pThisFirstIL = m_IL.m_pNext;
	ILInstr * pInstr = il.m_IL.m_pNext;
	ILInstr * pNewInstr = NULL;

	std::vector<ILInstr*> zeroStacks;
	for (; pThisFirstIL != &m_IL; pThisFirstIL = pThisFirstIL->m_pNext)
	{
		if (pThisFirstIL->m_stackposition == 0 & pThisFirstIL->m_insidebranch == false)
		{
			zeroStacks.emplace_back(pThisFirstIL);
		}
	}
	if (SafePointOffset > 0 && !zeroStacks.empty() && zeroStacks.at(SafePointOffset) != NULL)
	{
		pThisFirstIL = zeroStacks.at(SafePointOffset);
	}
	for (; pInstr != &il.m_IL; pInstr = pInstr->m_pNext)
	{
		pNewInstr = NewILInstr(*pInstr);
		InsertBefore(pThisFirstIL, pNewInstr);
	}

}

void ILRewriter::AddILProbe(ILInstr * pFirstIL) {

	for (ILInstr * pInstr = pFirstIL; pInstr != NULL; pInstr = pInstr->m_pNext)
	{
		if (pInstr->m_opcode != CEE_RET)
		{
			InsertBefore(pFirstIL, pInstr);
		}
	}

}

void ILRewriter::AddILExitProbe(ILRewriter & il) {
	BOOL fAtLeastOneProbeAdded = FALSE;

	// Find all RETs, and insert a call to the exit probe before each one.
	for (ILInstr * pInstr = GetILList()->m_pNext; pInstr != GetILList(); pInstr = pInstr->m_pNext)
	{
		switch (pInstr->m_opcode)
		{
		case CEE_RET:
		{
			// We want any branches or leaves that targeted the RET instruction to
			// actually target the epilog instructions we're adding. So turn the "RET"
			// into ["NOP", "RET"], and THEN add the epilog between the NOP & RET. That
			// ensures that any branches that went to the RET will now go to the NOP and
			// then execute our epilog.

			// RET->NOP
			pInstr->m_opcode = CEE_NOP;

			// Add the new RET after
			ILInstr * pNewRet = NewILInstr();
			pNewRet->m_opcode = CEE_RET;
			InsertAfter(pInstr, pNewRet);

			ILInstr * pThisFirstIL = pNewRet;
			ILInstr * pInstrOver = il.m_IL.m_pNext;
			ILInstr * pNewInstr = NULL;
			for (; pInstrOver != &il.m_IL; pInstrOver = pInstrOver->m_pNext)
			{
				if (pInstrOver->m_opcode != CEE_RET)
				{
					pNewInstr = NewILInstr(*pInstrOver);
					InsertBefore(pThisFirstIL, pNewInstr);
				}
			}
			fAtLeastOneProbeAdded = TRUE;

			// Advance pInstr after all this gunk so the for loop continues properly
			pInstr = pNewRet;
			break;
		}

		default:
			break;
		}
	}
}

UINT ILRewriter::AddNewULONGLocal() { return 0; }

UINT ILRewriter::AddNewDateTimeLocal() { return 0; }

WCHAR* ILRewriter::GetNameFromToken(mdToken tk) { UNREFERENCED_PARAMETER(tk); return L""; }

ILInstr * ILRewriter::NewLDC(LPVOID p) { UNREFERENCED_PARAMETER(p);  return NULL; }
