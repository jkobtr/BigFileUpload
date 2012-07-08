

/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */


 /* File created by MIDL compiler version 7.00.0555 */
/* at Fri Jul 06 14:52:01 2012
 */
/* Compiler settings for FileActiveX.idl:
    Oicf, W1, Zp8, env=Win32 (32b run), target_arch=X86 7.00.0555 
    protocol : dce , ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

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

MIDL_DEFINE_GUID(IID, LIBID_FileActiveXLib,0xCBE9E0B0,0x95DB,0x4495,0x8B,0xE8,0xE4,0xCB,0x41,0x6E,0xF0,0x7D);


MIDL_DEFINE_GUID(IID, DIID__DFileActiveX,0xACCA2641,0x9C12,0x409A,0x8D,0xD8,0x98,0x6E,0x5A,0x12,0xC5,0xD4);


MIDL_DEFINE_GUID(IID, DIID__DFileActiveXEvents,0x78099343,0xA8FF,0x4867,0xA5,0x37,0x06,0xF3,0x10,0x34,0xF2,0x2B);


MIDL_DEFINE_GUID(CLSID, CLSID_FileActiveX,0x36299202,0x09EF,0x4ABF,0xAD,0xB9,0x47,0xC5,0x99,0xDB,0xE7,0x78);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif



