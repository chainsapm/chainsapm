// dllmain.h : Declaration of module class.

class CclrprofilerModule : public ATL::CAtlDllModuleT< CclrprofilerModule >
{
public :
	DECLARE_LIBID(LIBID_clrprofilerLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_CLRPROFILER, "{60D0B41B-0C2C-40F5-8BCB-C50CEDEA2281}")
};

extern class CclrprofilerModule _AtlModule;
