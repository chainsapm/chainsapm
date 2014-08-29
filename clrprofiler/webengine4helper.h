#pragma once
class webengine4helper
{
public:
	typedef int(*MgdGetServerVariableW)
		(UINT_PTR pHandler,
		LPWSTR pszVarName,
		UINT_PTR *ppBuffer,
		int *pcchBufferSize);

	typedef int(*MgdGetRequestBasics)(
		UINT_PTR pRequestContext,
		UINT_PTR *pContentType,
		UINT_PTR *pContentTotalLength,
		UINT_PTR *pPathTranslated,
		UINT_PTR *pcchPathTranslated,
		UINT_PTR *pCacheUrl,
		UINT_PTR *pcchCacheUrl,
		UINT_PTR *pHttpMethod,
		UINT_PTR *pCookedUrl);

	typedef int (*MgdSetUnknownHeader)(UINT_PTR pRequestContext, bool fRequest, bool fReplace, char* header, char* value, unsigned short valueSize);


	static webengine4helper& createhelper();
	~webengine4helper();

	MgdGetServerVariableW GetServerVariableW;
	MgdGetRequestBasics GetRequestBasics;
	MgdSetUnknownHeader SetUnkHeader;


private:
	webengine4helper();
	webengine4helper(webengine4helper const &) { };
	void operator= (webengine4helper const &) { };

	HMODULE m_WE4Module;

};

