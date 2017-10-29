#include "stdafx.h"
#include "profilermain.h"

STDMETHODIMP Cprofilermain::AppDomainCreationStarted(AppDomainID appDomainId)
{
	UNREFERENCED_PARAMETER(appDomainId);
	return S_OK;
}