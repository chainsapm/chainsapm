// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the METADATADISPENSER_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// METADATADISPENSER_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef METADATADISPENSER_EXPORTS
#define METADATADISPENSER_API __declspec(dllexport) 
#else
#define METADATADISPENSER_API __declspec(dllimport)
#endif

// ATL LIbraries
#include <atlbase.h>
#include <atlcom.h>
#include <atlctl.h>

#include <cor.h>
#include <corhlpr.h>
#pragma comment(lib, "corguids.lib")
// This class is exported from the MetadataDispenser.dll
class METADATADISPENSER_API CMetadataDispenser {
public:
	CMetadataDispenser(void);
	// TODO: add your methods here.
};
