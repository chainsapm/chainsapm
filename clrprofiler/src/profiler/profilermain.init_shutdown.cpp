#include "stdafx.h"
#include "profilermain.h"

#include "..\src\exportedfunctions\exported_functions.cpp"


STDMETHODIMP Cprofilermain::Initialize(IUnknown *pICorProfilerInfoUnk)
{
	ReceievedMetaDataForInjection = CreateEvent(NULL, TRUE, FALSE, L"ReceievedMetaDataForInjection");
	// Get things like the process name, working directory, command line, etc.
	this->SetProcessName();

	this->m_Container = new ContainerClass();

	// We're using this as a quick exit so the profiler doesn't actually load.
	if (this->DoWeProfile() == S_OK)
	{
		//_CrtSetDbgFlag(_CRTDBG_CHECK_ALWAYS_DF);
		//_CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE | _CRTDBG_MODE_DEBUG);
		//_CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);
		SetUpAgent();



		this->m_HighestProfileInfo = 1; // If we don't fail, start off by assuming we're at the highest version we support

										// get the ICorProfilerInfo interface

		HRESULT hr;

		// determine if this object implements ICorProfilerInfo4
		hr = pICorProfilerInfoUnk->QueryInterface(IID_ICorProfilerInfo4, (LPVOID*)&this->m_pICorProfilerInfo4);
		if (FAILED(hr))
		{
			// we still want to work if this call fails, might be an older .NET version
			this->m_pICorProfilerInfo4 = nullptr;
			// determine if this object implements ICorProfilerInfo3
			hr = pICorProfilerInfoUnk->QueryInterface(IID_ICorProfilerInfo3, (LPVOID*)&this->m_pICorProfilerInfo3);
			if (FAILED(hr))
			{
				// we still want to work if this call fails, might be an older .NET version
				this->m_pICorProfilerInfo3 = nullptr;
				// determine if this object implements ICorProfilerInfo2
				hr = pICorProfilerInfoUnk->QueryInterface(IID_ICorProfilerInfo2, (LPVOID*)&this->m_pICorProfilerInfo2);
				if (FAILED(hr))
				{
					// we still want to work if this call fails, might be an older .NET version
					this->m_pICorProfilerInfo2 = nullptr;
					hr = pICorProfilerInfoUnk->QueryInterface(IID_ICorProfilerInfo, (LPVOID*)&this->m_pICorProfilerInfo);
					if (FAILED(hr))
						return E_FAIL;
				}
				else {
					this->m_HighestProfileInfo = 2;
					this->m_pICorProfilerInfo = m_pICorProfilerInfo2;
				}
			}
			else {
				this->m_HighestProfileInfo = 3;
				this->m_pICorProfilerInfo2 = m_pICorProfilerInfo3;
				this->m_pICorProfilerInfo = m_pICorProfilerInfo3;
			}
		}
		else {
			this->m_HighestProfileInfo = 4;
			this->m_pICorProfilerInfo3 = m_pICorProfilerInfo4;
			this->m_pICorProfilerInfo2 = m_pICorProfilerInfo4;
			this->m_pICorProfilerInfo = m_pICorProfilerInfo4;
		}


		UINT_PTR * clientData = new UINT_PTR(0xDEADBEEF); // We should never see this in our map. This is basically a bounds checker.

		if (m_pICorProfilerInfo2 != NULL)
		{
			clientData = new UINT_PTR(20); // Obviously we're not using any 
			m_pICorProfilerInfo2->SetFunctionIDMapper((FunctionIDMapper*)&Cprofilermain::Mapper1);
#ifdef _WIN64 
			m_pICorProfilerInfo2->SetEnterLeaveFunctionHooks2((FunctionEnter2*)&FunctionEnter2_Wrapper_x64, (FunctionLeave2*)&FunctionLeave2_Wrapper_x64, (FunctionTailcall2*)&FunctionTail2_Wrapper_x64);
#else
			m_pICorProfilerInfo2->SetEnterLeaveFunctionHooks2((FunctionEnter2*)&FunctionEnter2_x86, (FunctionLeave2*)&FunctionLeave2_x86, (FunctionTailcall2*)&FunctionTail2_x86);
#endif
		}

		if (m_pICorProfilerInfo3 != NULL)
		{
			// .NET40
			clientData = new UINT_PTR(40);
			m_pICorProfilerInfo3->SetFunctionIDMapper2((FunctionIDMapper2*)&Cprofilermain::Mapper2, this);
#ifdef _WIN64 

			m_pICorProfilerInfo3->SetEnterLeaveFunctionHooks2((FunctionEnter2*)&FunctionEnter2_Wrapper_x64, (FunctionLeave2*)&FunctionLeave2_Wrapper_x64, (FunctionTailcall2*)&FunctionTail2_Wrapper_x64);
#else

			m_pICorProfilerInfo3->SetEnterLeaveFunctionHooks2((FunctionEnter2*)&FunctionEnter2_x86, (FunctionLeave2*)&FunctionLeave2_x86, (FunctionTailcall2*)&FunctionTail2_x86);
			//m_pICorProfilerInfo3->SetEnterLeaveFunctionHooks3();
#endif
		}
		Cprofilermain::g_StaticContainerClass->insert(std::pair<UINT_PTR, Cprofilermain*>(0x0, this));

		SetMask();

		return S_OK;
	}
	else {
		return E_FAIL;
	}


}

STDMETHODIMP Cprofilermain::Shutdown(void)
{

	WaitForSingleObject(&NetworkClient::DataToBeSent, 5000); // Wait for data to be sent or 5 seconds
	WaitForSingleObject(&NetworkClient::DataSent, 5000); // Wait for data to be sent or 5 seconds
	m_NetworkClient->Shutdown();
	return S_OK;
}
