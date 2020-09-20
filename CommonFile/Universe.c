

/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */


 /* File created by MIDL compiler version 8.01.0622 */
/* at Tue Jan 19 11:14:07 2038
 */
/* Compiler settings for ..\CommonFile\Universe.idl:
    Oicf, W1, Zp8, env=Win64 (32b run), target_arch=AMD64 8.01.0622 
    protocol : all , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */



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
        EXTERN_C __declspec(selectany) const type name = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}

#endif // !_MIDL_USE_GUIDDEF_

MIDL_DEFINE_GUID(IID, IID_IHubbleExtender,0x19631222,0x1992,0x0612,0x19,0x65,0x06,0x01,0x20,0x16,0x01,0x01);


MIDL_DEFINE_GUID(IID, IID_IEclipseExtender,0x19631222,0x1992,0x0612,0x19,0x65,0x06,0x01,0x20,0x16,0x09,0x18);


MIDL_DEFINE_GUID(IID, IID_IOfficeExtender,0x19631222,0x1992,0x0612,0x19,0x65,0x06,0x01,0x19,0x63,0x12,0x22);


MIDL_DEFINE_GUID(IID, IID_IVSExtender,0x19631222,0x1992,0x0612,0x19,0x65,0x06,0x01,0x19,0x89,0x07,0x01);


MIDL_DEFINE_GUID(IID, IID_IVSDocument,0x19631222,0x1992,0x0612,0x19,0x65,0x06,0x01,0x20,0x17,0x08,0x24);


MIDL_DEFINE_GUID(IID, IID_IHubbleTreeNode,0x19631222,0x1992,0x0612,0x19,0x65,0x06,0x01,0x19,0x82,0xC9,0x51);


MIDL_DEFINE_GUID(IID, IID_IHubbleTreeViewCallBack,0x19631222,0x1992,0x0612,0x19,0x65,0x06,0x01,0x19,0x82,0x83,0xA6);


MIDL_DEFINE_GUID(IID, IID_IHubbleTreeView,0x19631222,0x1992,0x0612,0x19,0x65,0x06,0x01,0x19,0x82,0x83,0xA5);


MIDL_DEFINE_GUID(IID, IID_IHubbleEventObj,0x19631222,0x1992,0x0612,0x19,0x65,0x06,0x01,0x20,0x16,0x09,0x28);


MIDL_DEFINE_GUID(IID, IID_IGrid,0x19631222,0x1992,0x0612,0x19,0x65,0x06,0x01,0x19,0x82,0x00,0x04);


MIDL_DEFINE_GUID(IID, IID_IAppExtender,0x19631222,0x1992,0x0612,0x19,0x65,0x06,0x01,0x19,0x82,0x20,0x07);


MIDL_DEFINE_GUID(IID, IID_IHubbleEditor,0x19631222,0x1992,0x0612,0x19,0x65,0x06,0x01,0x19,0x82,0x66,0x88);


MIDL_DEFINE_GUID(IID, IID_IHubbleDoc,0x19631222,0x1992,0x0612,0x19,0x65,0x06,0x01,0x20,0x16,0x11,0x01);


MIDL_DEFINE_GUID(IID, IID_IHubbleDocTemplate,0x19631222,0x1992,0x0612,0x19,0x65,0x06,0x01,0x20,0x17,0x07,0x07);


MIDL_DEFINE_GUID(IID, IID_IHubble,0x19631222,0x1992,0x0612,0x19,0x65,0x06,0x01,0x19,0x82,0x00,0x01);


MIDL_DEFINE_GUID(IID, IID_IQuasar,0x19631222,0x1992,0x0612,0x19,0x65,0x06,0x01,0x19,0x82,0x00,0x03);


MIDL_DEFINE_GUID(IID, IID_IGridCollection,0x19631222,0x1992,0x0612,0x19,0x65,0x06,0x01,0x19,0x82,0x00,0x17);


MIDL_DEFINE_GUID(IID, IID_IGalaxyCluster,0x19631222,0x1992,0x0612,0x19,0x65,0x06,0x01,0x19,0x82,0x00,0x02);


MIDL_DEFINE_GUID(IID, IID_IWorkBenchWindow,0x19631222,0x1992,0x0612,0x19,0x65,0x06,0x01,0x19,0x82,0x5D,0x34);


MIDL_DEFINE_GUID(IID, IID_IBrowser,0x19631222,0x1992,0x0612,0x19,0x65,0x06,0x01,0x20,0x18,0x08,0x28);


MIDL_DEFINE_GUID(IID, IID_IWebPage,0x19631222,0x1992,0x0612,0x19,0x65,0x06,0x01,0x20,0x18,0x09,0x03);


MIDL_DEFINE_GUID(IID, IID_IOfficeObject,0x19631222,0x1992,0x0612,0x19,0x65,0x06,0x01,0x19,0x65,0x06,0x06);


MIDL_DEFINE_GUID(IID, IID_IHubbleCtrl,0x19631222,0x1992,0x0612,0x19,0x65,0x06,0x01,0x19,0x82,0x31,0xDC);


MIDL_DEFINE_GUID(IID, IID_IHubbleAppCtrl,0x19631222,0x1992,0x0612,0x19,0x65,0x06,0x01,0x20,0x16,0x10,0x01);


MIDL_DEFINE_GUID(IID, IID_IEclipseCtrl,0x19631222,0x1992,0x0612,0x19,0x65,0x06,0x01,0x20,0x16,0x09,0x12);


MIDL_DEFINE_GUID(IID, LIBID_Universe,0x19631222,0x1992,0x0612,0x19,0x65,0x06,0x01,0x19,0x82,0x09,0x11);


MIDL_DEFINE_GUID(IID, DIID__IHubble,0x19631222,0x1992,0x0612,0x19,0x65,0x06,0x01,0x19,0x82,0x10,0x02);


MIDL_DEFINE_GUID(CLSID, CLSID_Universe,0x19631222,0x1992,0x0612,0x19,0x65,0x06,0x01,0x19,0x99,0x06,0x06);


MIDL_DEFINE_GUID(IID, DIID__IGridEvents,0x19631222,0x1992,0x0612,0x19,0x65,0x06,0x01,0x19,0x82,0x10,0x01);


MIDL_DEFINE_GUID(IID, DIID__IHubbleObjEvents,0x19631222,0x1992,0x0612,0x19,0x65,0x06,0x01,0x19,0x82,0x19,0x92);


MIDL_DEFINE_GUID(IID, DIID__IHubbleAppEvents,0x19631222,0x1992,0x0612,0x19,0x65,0x06,0x01,0x20,0x16,0x09,0x30);


MIDL_DEFINE_GUID(CLSID, CLSID_HubbleCtrl,0x19631222,0x1992,0x0612,0x19,0x65,0x06,0x01,0x20,0x07,0x10,0x01);


MIDL_DEFINE_GUID(CLSID, CLSID_UniverseExtender,0x19631222,0x1992,0x0612,0x19,0x65,0x06,0x01,0x00,0x00,0x00,0x00);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif



