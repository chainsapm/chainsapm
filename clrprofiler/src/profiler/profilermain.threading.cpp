#include "stdafx.h"
#include "profilermain.h"

STDMETHODIMP Cprofilermain::ThreadCreated(ThreadID threadId)
{
	UNREFERENCED_PARAMETER(threadId);
	return S_OK;
}

STDMETHODIMP Cprofilermain::ThreadDestroyed(ThreadID threadId)
{
	UNREFERENCED_PARAMETER(threadId);
	return S_OK;
}

STDMETHODIMP Cprofilermain::ThreadNameChanged(ThreadID threadId, ULONG cchName, _In_reads_opt_(cchName) WCHAR name[])
{
	UNREFERENCED_PARAMETER(threadId);
	UNREFERENCED_PARAMETER(cchName);
	UNREFERENCED_PARAMETER(name);
	return S_OK;
}
