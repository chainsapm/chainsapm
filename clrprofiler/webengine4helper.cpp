#include "stdafx.h"
#include "webengine4helper.h"


// CRITICAL 1 Research thread safety of webengine classes.
// TODO: Add in more functions to extract more details from the web request.
webengine4helper& webengine4helper::createhelper()
{
	static webengine4helper m_Instance;
	
	return m_Instance;
}

webengine4helper::webengine4helper()
{
	this->m_WE4Module = GetModuleHandle(TEXT("webengine4.dll"));
	this->GetServerVariableW = (MgdGetServerVariableW)GetProcAddress(this->m_WE4Module, "MgdGetServerVariableW");
	this->GetRequestBasics = (MgdGetRequestBasics)GetProcAddress(this->m_WE4Module, "MgdGetRequestBasics");
	this->SetUnkHeader = (MgdSetUnknownHeader)GetProcAddress(this->m_WE4Module, "MgdSetUnknownHeader");
}

webengine4helper::~webengine4helper()
{

}