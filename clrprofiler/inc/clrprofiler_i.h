

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.00.0603 */
/* at Tue May 05 08:48:34 2015
 */
/* Compiler settings for defs\clrprofiler.idl:
    Oicf, W1, Zp8, env=Win64 (32b run), target_arch=AMD64 8.00.0603 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__


#ifndef __clrprofiler_i_h__
#define __clrprofiler_i_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __profilermain_FWD_DEFINED__
#define __profilermain_FWD_DEFINED__

#ifdef __cplusplus
typedef class profilermain profilermain;
#else
typedef struct profilermain profilermain;
#endif /* __cplusplus */

#endif 	/* __profilermain_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 



#ifndef __clrprofilerLib_LIBRARY_DEFINED__
#define __clrprofilerLib_LIBRARY_DEFINED__

/* library clrprofilerLib */
/* [version][uuid] */ 


EXTERN_C const IID LIBID_clrprofilerLib;

EXTERN_C const CLSID CLSID_profilermain;

#ifdef __cplusplus

class DECLSPEC_UUID("41DB4CB9-F3A1-44B2-87DC-52BF4E8E8EB2")
profilermain;
#endif
#endif /* __clrprofilerLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


