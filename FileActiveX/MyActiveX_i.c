

/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */


 /* File created by MIDL compiler version 7.00.0500 */
/* at Fri Apr 27 18:08:39 2012
 */
/* Compiler settings for .\MyActiveX.idl:
    Oicf, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

#pragma warning( disable: 4049 )  /* more than 64k source lines */


#ifdef __cplusplus
extern "C"{
#endif 


#include <rpc.h>
#include <rpcndr.h>

#ifdef _MIDL_USE_GUIDDEF_

#ifndef INITGUID
#define INITGUID
#include <guiddef.h>
#undef INITGUID
#else
#include <guiddef.h>
#endif

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        DEFINE_GUID(name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8)

#else // !_MIDL_USE_GUIDDEF_

#ifndef __IID_DEFINED__
#define __IID_DEFINED__

typedef struct _IID
{
    unsigned long x;
    unsigned short s1;
    unsigned short s2;
    unsigned char  c[8];
} IID;

#endif // __IID_DEFINED__

#ifndef CLSID_DEFINED
#define CLSID_DEFINED
typedef IID CLSID;
#endif // CLSID_DEFINED

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        const type name = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}

#endif !_MIDL_USE_GUIDDEF_

MIDL_DEFINE_GUID(IID, LIBID_MyActiveXLib,0xCBE9E0B0,0x95DB,0x4495,0x8B,0xE8,0xE4,0xCB,0x41,0x6E,0xF0,0x7D);


MIDL_DEFINE_GUID(IID, DIID__DMyActiveX,0x98AF8A06,0x7DFE,0x472D,0xA7,0x13,0xA0,0xC6,0xF2,0x2F,0xDC,0x3D);


MIDL_DEFINE_GUID(IID, DIID__DMyActiveXEvents,0x78099343,0xA8FF,0x4867,0xA5,0x37,0x06,0xF3,0x10,0x34,0xF2,0x2B);


MIDL_DEFINE_GUID(CLSID, CLSID_MyActiveX,0x36299202,0x09EF,0x4ABF,0xAD,0xB9,0x47,0xC5,0x99,0xDB,0xE7,0x78);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif



