/*
This class will take in var args of a trait type and output a method signature PInvoke as well as the Managed equivalent.

*/
#include "MethodCreator.h"
#include <cor.h>
#include <corprof.h>


template<typename T>
struct cee_trait
{
	static int cee_value();
};

template<>
struct cee_trait < char >
{
	static int cee_value()
	{
		return ELEMENT_TYPE_CHAR;
	}
};

template<>
struct cee_trait < void >
{
	static int cee_value()
	{
		return ELEMENT_TYPE_VOID;
	}
};



template<>
struct cee_trait < unsigned char >
{
	static int cee_value()
	{
		return ELEMENT_TYPE_U1;
	}
};

template<>
struct cee_trait < short >
{
	static int cee_value()
	{
		return ELEMENT_TYPE_I2;
	}
};

template<>
struct cee_trait < int >
{
	static int cee_value()
	{
		return ELEMENT_TYPE_I4;
	}
};

template<>
struct cee_trait < long >
{
	static int cee_value()
	{
		return ELEMENT_TYPE_I4;
	}
};

template<>
struct cee_trait < long long >
{
	static int cee_value()
	{
		return ELEMENT_TYPE_I8;
	}
};

template<>
struct cee_trait < unsigned short >
{
	static int cee_value()
	{
		return ELEMENT_TYPE_U2;
	}
};

template<>
struct cee_trait < unsigned int >
{
	static int cee_value()
	{
		return ELEMENT_TYPE_U4;
	}
};

template<>
struct cee_trait < unsigned long >
{
	static int cee_value()
	{
		return ELEMENT_TYPE_U4;
	}
};

template<>
struct cee_trait < unsigned long long >
{
	static int cee_value()
	{
		return ELEMENT_TYPE_U8;
	}
};

template<>
struct cee_trait < float >
{
	static int cee_value()
	{
		return ELEMENT_TYPE_R4;
	}
};

template<>
struct cee_trait < float long>
{
	static int cee_value()
	{
		return ELEMENT_TYPE_R8;
	}
};

struct ModuleInfo;

namespace MethodCreator
{
	template<COR_SIGNATURE... Targs>
	class DynamicMethodBuilder
	{
		typedef std::vector<COR_SIGNATURE> Corsig;
	public:
		DynamicMethodBuilder() : sigArray(Corsig{ Targs... })
		{}
		Corsig sigArray;

		void CreateMethodDef(IMetaDataAssemblyImport * pAssemblyImport, IMetaDataAssemblyEmit * pAssemblyEmit,
			IMetaDataEmit * pEmit, ModuleInfo * pModuleInfo)
		{
			Corsig thisArray{ IMAGE_CEE_CS_CALLCONV_DEFAULT, 0, ELEMENT_TYPE_VOID };
			thisArray.at(1) = sizeof...(Targs);
			thisArray.insert(thisArray.end(), sigArray.begin(), sigArray.end());
			COR_SIGNATURE *sigFunctionProbe = thisArray.begin()._Ptr;

			// Probes are being added to mscorlib. Find existing mscorlib assemblyRef.

			HCORENUM hEnum = NULL;
			mdAssemblyRef rgAssemblyRefs[20];
			ULONG cAssemblyRefsReturned;
			mdAssemblyRef assemblyRef = mdTokenNil;

			HRESULT hr;

			do
			{
				hr = pAssemblyImport->EnumAssemblyRefs(
					&hEnum,
					rgAssemblyRefs,
					_countof(rgAssemblyRefs),
					&cAssemblyRefsReturned);

				/*if (FAILED(hr))
				{
				LOG_APPEND(L"EnumAssemblyRefs failed, hr = " << HEX(hr));
				return;
				}

				if (cAssemblyRefsReturned == 0)
				{
				LOG_APPEND(L"Could not find an AssemblyRef to mscorlib");
				return;
				}*/
			} while (true);

			pAssemblyImport->CloseEnum(hEnum);
			hEnum = NULL;

			assert(assemblyRef != mdTokenNil);

			// Generate typeRef to ILRewriteProfilerHelper.ProfilerHelper or the pre-existing mscorlib type
			// that we're adding the managed helpers to.

			mdTypeRef typeRef;

			LPCWSTR wszTypeToReference =
				L"Test";

			hr = pEmit->DefineTypeRefByName(
				assemblyRef,
				wszTypeToReference,
				&typeRef);

			/*if (FAILED(hr))
			{
			LOG_APPEND(L"DefineTypeRefByName to " << wszTypeToReference << L" failed, hr = " << HEX(hr));
			}*/

			mdMemberRef mdref;

			hr = pEmit->DefineMemberRef(
				typeRef,
				L"Test",
				sigFunctionProbe,
				thisArray.size(),
				&mdref);

			/*if (FAILED(hr))
			{
			LOG_APPEND(L"DefineMemberRef to " << "Test" <<
			L" failed, hr = " << HEX(hr));
			}*/

		}
	};
}
