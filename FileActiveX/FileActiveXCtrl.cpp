#pragma once
// FileActiveXCtrl.cpp : Implementation of the CFileActiveXCtrl ActiveX Control class.
#include "stdafx.h"
#include "FileActiveX.h"
#include "FileActiveXCtrl.h"
#include "FileActiveXPropPage.h" 
#include<afxpriv.h>
#include<iostream>
#include <sys/stat.h>
#include "../Common/md5.h" 
#include "../Common/InfoStruct.h"
#include "../Common/FileToken.h"
#include "../Common/AppConf.h"
#include "project.h"

#define WM_OPTSUCCESS WM_APP+101 //操作成功
#define WM_OPTFAILED WM_APP+102	//操作失败
#define WM_OPTPORGRESS WM_APP+103	//进度
#define WM_OPTCANELUP WM_APP+104	//取消
#define JSCALLONFINISH "OnSuccess"
#define JSCALLONPROGREESS "OnProgress"
#define JSCALLONFailed "OnFailed" 

AppConf  m_AppItem;						// APP配置表

using namespace std; 
IMPLEMENT_DYNCREATE(CFileActiveXCtrl, COleControl)


#define CHECK_POINTER(p)\
	ATLASSERT(p != NULL);\
	if(p == NULL)\
{\
	return false;\
}
#define COMRELEASE(p)\
	if(p != NULL)\
{\
	p->Release();\
	p=NULL;\
}

// Message map

BEGIN_MESSAGE_MAP(CFileActiveXCtrl, COleControl)
	ON_MESSAGE(OCM_COMMAND, &CFileActiveXCtrl::OnOcmCommand)
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
	ON_WM_CREATE()
END_MESSAGE_MAP()



// Dispatch map

BEGIN_DISPATCH_MAP(CFileActiveXCtrl, COleControl)
	DISP_PROPERTY_NOTIFY_ID(CFileActiveXCtrl, "InputFilePath", dispidInputFilePath, m_InputFilePath, OnInputParameterChanged, VT_BSTR)   //获取文件的路径
	DISP_PROPERTY_NOTIFY_ID(CFileActiveXCtrl, "OutputParameter", dispidOutputParameter, m_OutputParameter, OnOutputParameterChanged, VT_BSTR)  //输出的结果
	DISP_FUNCTION_ID(CFileActiveXCtrl, "InitUpload", dispidLoadParameter, InitUpload, VT_EMPTY, VTS_NONE)    //执行传输方法
	DISP_PROPERTY_NOTIFY_ID(CFileActiveXCtrl, "InputConnIP", dispidInputConnIP, m_ConnIP, OnConnIPChanged, VT_BSTR) //连接IP
	DISP_PROPERTY_NOTIFY_ID(CFileActiveXCtrl, "InputConnPort",dispidInputConnPort, m_ConnPort, OnConnPortChanged, VT_I4) //连接服务器端口
	DISP_PROPERTY_NOTIFY_ID(CFileActiveXCtrl, "OutputErrMSG", dispidOutputErrMSG, m_ErrMSG, OnErrMSGChanged, VT_BSTR)  //错误信息 
	DISP_PROPERTY_NOTIFY_ID(CFileActiveXCtrl, "OutputProgress", dispidOutputProgress, m_OutputProgress, OnOutputProgressChanged, VT_I4) //输出进度
	DISP_PROPERTY_NOTIFY_ID(CFileActiveXCtrl, "InputToken", dispidInputToken, m_InputToken, OnInputTokenChanged, VT_BSTR)//出入令牌
	DISP_FUNCTION_ID(CFileActiveXCtrl, "OnOutputOpenDialog", dispidOpenDialog, OnOutputOpenDialog, VT_BSTR, VTS_BSTR VTS_BSTR)
	DISP_PROPERTY_NOTIFY_ID(CFileActiveXCtrl, "OutPutFileLocation", dispidOutPutFileLocation, m_OutPutFileLocation, OnOutPutFileLocationChanged, VT_BSTR)
	DISP_PROPERTY_NOTIFY_ID(CFileActiveXCtrl, "OutPutFileName", dispidOutPutFileName, m_OutPutFileName, OnOutPutFileNameChanged, VT_BSTR)
	DISP_PROPERTY_NOTIFY_ID(CFileActiveXCtrl, "OutPutUpLdLog", dispidOutPutUpLdLog, m_OutPutUpLdLog, OnOutPutUpLdLogChanged, VT_BSTR)
	DISP_PROPERTY_NOTIFY_ID(CFileActiveXCtrl, "OutPutFileSize", dispidOutPutFileSize, m_OutPutFileSize, OnOutPutFileSizeChanged, VT_UI4)
	DISP_PROPERTY_NOTIFY_ID(CFileActiveXCtrl, "OutPutShowFileName", dispidOutPutShowFileName, m_OutPutShowFileName, OnOutPutShowFileNameChanged, VT_BSTR)
	DISP_PROPERTY_NOTIFY_ID(CFileActiveXCtrl, "OutPutErrorCode", dispidOutPutErrorCode, m_OutPutErrorCode, OnOutPutErrorCodeChanged, VT_I4)
	DISP_FUNCTION_ID(CFileActiveXCtrl, "OnCanelupLoad", dispidOnCanelupLoad, OnCanelupLoad, VT_I4, VTS_NONE)
END_DISPATCH_MAP()



// Event map

BEGIN_EVENT_MAP(CFileActiveXCtrl, COleControl)
	EVENT_CUSTOM_ID("ParameterLoaded", eventidParameterLoaded, FireParameterLoaded, VTS_NONE)
END_EVENT_MAP()



// Property pages

// TODO: Add more property pages as needed.  Remember to increase the count!
BEGIN_PROPPAGEIDS(CFileActiveXCtrl, 1)
PROPPAGEID(CFileActiveXPropPage::guid)
END_PROPPAGEIDS(CFileActiveXCtrl)



// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CFileActiveXCtrl, "MYACTIVEX.FileActiveXCtrl.1",
					   0x36299202, 0x9ef, 0x4abf, 0xad, 0xb9, 0x47, 0xc5, 0x99, 0xdb, 0xe7, 0x78)



					   // Type library ID and version

					   IMPLEMENT_OLETYPELIB(CFileActiveXCtrl, _tlid, _wVerMajor, _wVerMinor)



					   // Interface IDs

					   const IID BASED_CODE IID_DFileActiveX =
{ 0x98AF8A06, 0x7DFE, 0x472D, { 0xA7, 0x13, 0xA0, 0xC6, 0xF2, 0x2F, 0xDC, 0x3D } };
const IID BASED_CODE IID_DFileActiveXEvents =
{ 0x78099343, 0xA8FF, 0x4867, { 0xA5, 0x37, 0x6, 0xF3, 0x10, 0x34, 0xF2, 0x2B } };



// Control type information

static const DWORD BASED_CODE _dwFileActiveXOleMisc =
OLEMISC_ACTIVATEWHENVISIBLE |
OLEMISC_SETCLIENTSITEFIRST |
OLEMISC_INSIDEOUT |
OLEMISC_CANTLINKINSIDE |
OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(CFileActiveXCtrl, IDS_MYACTIVEX, _dwFileActiveXOleMisc)



// CFileActiveXCtrl::CFileActiveXCtrlFactory::UpdateRegistry -
// Adds or removes system registry entries for CFileActiveXCtrl

BOOL CFileActiveXCtrl::CFileActiveXCtrlFactory::UpdateRegistry(BOOL bRegister)
{
	// TODO: Verify that your control follows apartment-model threading rules.
	// Refer to MFC TechNote 64 for more information.
	// If your control does not conform to the apartment-model rules, then
	// you must modify the code below, changing the 6th parameter from
	// afxRegApartmentThreading to 0.

	if (bRegister)
		return AfxOleRegisterControlClass(
		AfxGetInstanceHandle(),
		m_clsid,
		m_lpszProgID,
		IDS_MYACTIVEX,
		IDB_MYACTIVEX,
		afxRegApartmentThreading,
		_dwFileActiveXOleMisc,
		_tlid,
		_wVerMajor,
		_wVerMinor);
	else
		return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}



// CFileActiveXCtrl::CFileActiveXCtrl - Constructor

CFileActiveXCtrl::CFileActiveXCtrl()
{
	InitializeIIDs(&IID_DFileActiveX, &IID_DFileActiveXEvents); 
	m_OutputProgress=-1; 
	// TODO: Initialize your control's instance data here. 
}
void CFileActiveXCtrl::cleanData()
{
	m_OutputProgress=-1; 
	m_OutPutFileLocation;                //文件服务器保持路径  
	//m_OutPutFileName="";
	m_OutPutUpLdLog="";
	m_OutPutFileSize=0;
	//m_OutPutShowFileName="";               //原始文件名
	m_OutPutErrorCode=0;
}


// CFileActiveXCtrl::~CFileActiveXCtrl - Destructor

CFileActiveXCtrl::~CFileActiveXCtrl()
{
	//OnClose();
	// TODO: Cleanup your control's instance data here.
}

void CFileActiveXCtrl::OnSetClientSite()
{
	HRESULT hr = S_OK;
	IServiceProvider *isp, *isp2 = NULL;
	if (!m_pClientSite)
	{
		COMRELEASE(pWebBrowser);
	}  
	else
	{
		hr = m_pClientSite->QueryInterface(IID_IServiceProvider, reinterpret_cast<void **>(&isp));
		if (FAILED(hr)) 
		{
			hr = S_OK;
			goto cleanup;
		}
		hr = isp->QueryService(SID_STopLevelBrowser, IID_IServiceProvider, reinterpret_cast<void **>(&isp2));
		if (FAILED(hr))
		{
			hr = S_OK;
			goto cleanup;
		}
		hr = isp2->QueryService(SID_SWebBrowserApp, IID_IWebBrowser2, reinterpret_cast<void **>(&pWebBrowser)); //查询IE浏览器接口
		if (FAILED(hr)) 
		{
			hr = S_OK;
			goto cleanup;
		}
		hr   =   pWebBrowser->get_Document((IDispatch**)&pHTMLDocument); //查询Web页面接口  
		if(FAILED(hr))   
		{   
			hr = S_OK;
			goto cleanup;
		}   
cleanup:
		// Free resources.
		COMRELEASE(isp);
		COMRELEASE(isp2);
	}
}
bool CFileActiveXCtrl::GetJScript(CComPtr<IDispatch>& spDisp)
{
	CHECK_POINTER(pHTMLDocument);
	HRESULT hr = pHTMLDocument->get_Script(&spDisp);
	ATLASSERT(SUCCEEDED(hr));
	return SUCCEEDED(hr);
}

bool CFileActiveXCtrl::GetJScripts(CComPtr<IHTMLElementCollection>& spColl)
{
	CHECK_POINTER(pHTMLDocument);
	HRESULT hr = pHTMLDocument->get_scripts(&spColl);
	ATLASSERT(SUCCEEDED(hr));
	return SUCCEEDED(hr);
}

bool CFileActiveXCtrl::CallJScript(const CString strFunc,CComVariant* pVarResult)
{
	CStringArray paramArray;
	return CallJScript(strFunc,paramArray,pVarResult);
}

bool CFileActiveXCtrl::CallJScript(const CString strFunc,const CString strArg1,CComVariant* pVarResult)
{
	CStringArray paramArray;
	paramArray.Add(strArg1);
	return CallJScript(strFunc,paramArray,pVarResult);
}

bool CFileActiveXCtrl::CallJScript(const CString strFunc,const CString strArg1,const CString strArg2,CComVariant* pVarResult)
{
	CStringArray paramArray;
	paramArray.Add(strArg1);
	paramArray.Add(strArg2);
	return CallJScript(strFunc,paramArray,pVarResult);
}

bool CFileActiveXCtrl::CallJScript(const CString strFunc,const CString strArg1,const CString strArg2,const CString strArg3,CComVariant* pVarResult)
{
	CStringArray paramArray;
	paramArray.Add(strArg1);
	paramArray.Add(strArg2);
	paramArray.Add(strArg3);
	return CallJScript(strFunc,paramArray,pVarResult);
}

bool CFileActiveXCtrl::CallJScript(const CString strFunc, const CStringArray& paramArray,CComVariant* pVarResult)
{
	CComPtr<IDispatch> spScript;
	if(!GetJScript(spScript))
	{
		//ShowError("Cannot GetScript");
		return false;
	}
	CComBSTR bstrMember(strFunc);
	DISPID dispid = NULL;
	HRESULT hr = spScript->GetIDsOfNames(IID_NULL,&bstrMember,1,
		LOCALE_SYSTEM_DEFAULT,&dispid);
	if(FAILED(hr))
	{
		//ShowError(GetSystemErrorMessage(hr));
		return false;
	}
	const int arraySize = paramArray.GetSize();
	DISPPARAMS dispparams;
	memset(&dispparams, 0, sizeof dispparams);
	dispparams.cArgs = arraySize;
	dispparams.rgvarg = new VARIANT[dispparams.cArgs];
	for( int i = 0; i < arraySize; i++)
	{
		CComBSTR bstr = paramArray.GetAt(arraySize - 1 - i); // back reading
		bstr.CopyTo(&dispparams.rgvarg[i].bstrVal);
		dispparams.rgvarg[i].vt = VT_BSTR;
	}
	dispparams.cNamedArgs = 0;
	EXCEPINFO excepInfo;
	memset(&excepInfo, 0, sizeof excepInfo);
	CComVariant vaResult;
	UINT nArgErr = (UINT)-1;  // initialize to invalid arg
	hr = spScript->Invoke(dispid,IID_NULL,0,
		DISPATCH_METHOD,&dispparams,&vaResult,&excepInfo,&nArgErr);
	delete [] dispparams.rgvarg;
	if(FAILED(hr))
	{
		//ShowError(GetSystemErrorMessage(hr));
		return false;
	}
	if(pVarResult)
	{
		*pVarResult = vaResult;
	}
	return true;
}
// CFileActiveXCtrl::OnDraw - Drawing function

void CFileActiveXCtrl::OnDraw(
							CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid)
{
	if (!pdc)
		return;

	DoSuperclassPaint(pdc, rcBounds);  
	//m_MainDialog.MoveWindow(rcBounds, TRUE); 
	CBrush brBackGnd(TranslateColor(AmbientBackColor()));
	pdc->FillRect(rcBounds, &brBackGnd);
}



// CFileActiveXCtrl::DoPropExchange - Persistence support

void CFileActiveXCtrl::DoPropExchange(CPropExchange* pPX)
{
	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	COleControl::DoPropExchange(pPX);

	// TODO: Call PX_ functions for each persistent custom property.
}
void CFileActiveXCtrl::OnClose(DWORD dwSaveOption)
{ 
	pfileupdatethread.Stop(true);//强行关闭工作线程
	COMRELEASE(pWebBrowser);
	COMRELEASE(pHTMLDocument); 
	COleControl::OnClose(dwSaveOption);
}
LRESULT CFileActiveXCtrl::WindowProc(UINT msg, WPARAM wParam, LPARAM lParam)
{
	m_OutPutErrorCode=m_ErrorCode;
	switch (msg)
	{
	case WM_OPTSUCCESS:
		{//操作成功，通知外部页面
			m_ErrMSG=pfileupdatethread.m_ErrMSG;
			m_OutPutUpLdLog=pfileupdatethread.m_SendLog;
			m_OutPutFileName=pfileupdatethread.m_FileName;  
			m_OutPutFileSize=pfileupdatethread.m_fileSize;
			m_OutPutFileLocation=pfileupdatethread.m_OutPutFileLocation;
			m_OutputProgress=100;
			CString strOnLoaded(JSCALLONFINISH);
			CString strOnProGress(JSCALLONPROGREESS);
			this->CallJScript(strOnLoaded); 
			this->CallJScript(strOnProGress);
			pfileupdatethread.Stop(); 
			//cleanData();
			return 0;
		}
	case WM_OPTPORGRESS:
		{//进度，通知外部页面
			m_ErrMSG=pfileupdatethread.m_ErrMSG;
			m_OutPutUpLdLog=pfileupdatethread.m_SendLog;
			m_OutputProgress=(int)wParam; 
			CString strOnProGress(JSCALLONPROGREESS);
			this->CallJScript(strOnProGress); 
			//pfileupdatethread.Stop(); 
			return 0;
		}
	case WM_OPTFAILED:
		{//操作失败，通知外部页面
			//这里不写了，同上面
			m_OutputProgress=100;
			m_ErrMSG=pfileupdatethread.m_ErrMSG;
			m_OutPutUpLdLog=pfileupdatethread.m_SendLog;
			CString strOnProGress(JSCALLONPROGREESS);
			CString strOnfailed(JSCALLONFailed); 
			this->CallJScript(strOnProGress); 
			this->CallJScript(strOnfailed); 
			pfileupdatethread.Stop(); 
			//cleanData();
			return 0;
		}
	case WM_OPTCANELUP:
		{//取消上传操作
			m_ErrMSG=pfileupdatethread.m_ErrMSG;
			m_OutPutUpLdLog=pfileupdatethread.m_SendLog; 
			pfileupdatethread.Stop(); 
			//cleanData();
			return 0;
		}
	}
	
	return  COleControl::WindowProc(msg,wParam,lParam);   
}
/////////////////////
//回调函数
/////////////////////////
void CFileActiveXCtrl::OnSuccesful()
{//操作成功
	this->PostMessage(WM_OPTSUCCESS,(WPARAM)NULL,(LPARAM)NULL);
	/*CString strOnLoaded("OnLoaded");
	this->CallJScript(strOnLoaded);*/
}
void CFileActiveXCtrl::OnProgress(int progressvalue)
{
	this->PostMessage(WM_OPTPORGRESS,(WPARAM)progressvalue,(LPARAM)NULL);
}
void CFileActiveXCtrl::OnFailed()
{//操作失败
	this->PostMessage(WM_OPTFAILED,(WPARAM)NULL,(LPARAM)NULL);
}
void CFileActiveXCtrl::OnCanel()
{//操作失败
	this->PostMessage(WM_OPTCANELUP,(WPARAM)NULL,(LPARAM)NULL);
}


// CFileActiveXCtrl::GetControlFlags -
// Flags to customize MFC's implementation of ActiveX controls.
//
DWORD CFileActiveXCtrl::GetControlFlags()
{
	DWORD dwFlags = COleControl::GetControlFlags();


	// The control will not be redrawn when making the transition
	// between the active and inactivate state.
	dwFlags |= noFlickerActivate;
	return dwFlags;
}



// CFileActiveXCtrl::OnResetState - Reset control to default state

void CFileActiveXCtrl::OnResetState()
{
	COleControl::OnResetState();  // Resets defaults found in DoPropExchange

	// TODO: Reset any other control state here.
}



// CFileActiveXCtrl::PreCreateWindow - Modify parameters for CreateWindowEx

BOOL CFileActiveXCtrl::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.lpszClass = _T("STATIC");
	return COleControl::PreCreateWindow(cs);
}



// CFileActiveXCtrl::IsSubclassedControl - This is a subclassed control

BOOL CFileActiveXCtrl::IsSubclassedControl()
{
	return TRUE;
}



// CFileActiveXCtrl::OnOcmCommand - Handle command messages

LRESULT CFileActiveXCtrl::OnOcmCommand(WPARAM wParam, LPARAM lParam)
{
#ifdef _WIN32
	WORD wNotifyCode = HIWORD(wParam);
#else
	WORD wNotifyCode = HIWORD(lParam);
#endif

	// TODO: Switch on wNotifyCode here.

	return 0;
}



// CFileActiveXCtrl message handlers

int CFileActiveXCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (COleControl::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	//m_MainDialog.Create(IDD_CONTROL, this);

	return 0;
}

void CFileActiveXCtrl::OnInputParameterChanged(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: Add your property handler code here

	SetModifiedFlag();
}

void CFileActiveXCtrl::OnOutputParameterChanged(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: Add your property handler code here

	SetModifiedFlag();
}
//BOOL CFileActiveXCtrl::SendInit(SOCKET &hSock,int nOrgSize)
//{
//	//////////////////////////////////////////////////////////////////////////
//	// 读取文件的MD5
//	//////////////////////////////////////////////////////////////////////////
//	string md5str; 
//	if(m_InputFilePath.IsEmpty()){
//		m_ErrMSG="Input File is Empty!";
//		return FALSE;
//	} else
//		md5str=MD5(ifstream(m_InputFilePath, ios::binary)).toString(); 
//	m_ErrMSG.AppendFormat("\nMD5:%s\n",md5str.c_str());
//	/////////////////////////////////////////////////////////////////////////////
//	//消息定义
//	//////////////////////////////////////////////////////////////////////////
//	char * bufHead,*bufBody;
//	int bufHeadLen,bufBodyLen;
//	MSG_REQ_COMMON_HEAD head1,head2;
//	MSG_REQ_INIT_BODY body1,body2;
//
//	//////////////////////////////////////////////////////////////////////////
//	//系统初始化
//	////////////////////////////////////////////////////////////////////////// 
//	//设置包头
//	head1.clear(); 
//	head1.nMsgId = MSGID_INIT;
//	head1.bUserType = '0';
//	//编码 
//	body1.clear();
//	body1.bFileCheckType = '0';
//	strcpy(body1.strVersion,"1.0.0");
//	body1.nFileSize = nOrgSize; //文件大小
//	//body1.nFileCheckSize =md5str;
//
//	memcpy(body1.strFileCheck,md5str.c_str(),md5str.length());
//	body1.nFileCheckSize=md5str.length();
//	body1.nTokenSize = m_InputToken.GetLength();
//	strcpy(body1.strToken,m_InputToken);  //token令牌
//	//编码
//	bufBody = body1.getNetBuf();
//	bufBodyLen = body1.getNetBufLen();
//
//	head1.nPkgSize=bufBodyLen;
//
//	bufHead = head1.getNetBuf();
//	bufHeadLen = head1.getNetBufLen();  
//
//	char * sdbuffer=NULL;
//	sdbuffer=(char *)malloc(bufHeadLen+bufBodyLen);
//	memcpy(sdbuffer,bufHead,bufHeadLen);
//	memcpy(sdbuffer+bufHeadLen,bufBody,bufBodyLen);
//	if (!SendBuffer(hSock, sdbuffer,bufHeadLen+bufBodyLen))
//	{
//		m_ErrMSG.Append("初始化头请求失败！");
//		return FALSE;  
//	}
//	head1.setNetBuf(sdbuffer,bufHeadLen);
//	body1.setNetBuf(sdbuffer+bufHeadLen,bufHeadLen+bufBodyLen);
//	delete[] sdbuffer;
//	sdbuffer=NULL;
//	delete []bufHead;
//	bufHead=NULL;
//	delete []bufBody;
//	bufBody=NULL;
//	/*if (!SendBuffer(hSock, bufHead,bufHeadLen))
//	{
//	m_ErrMSG.Append("初始化头请求失败！");
//	return FALSE;  
//	}  
//	if (!SendBuffer(hSock, bufBody,bufBodyLen))
//	{
//	m_ErrMSG.Append("初始化消息体请求失败！");
//	return FALSE; 
//	} */
//	return true;
//}
//BOOL CFileActiveXCtrl::SendPacketGetpart(SOCKET &hSock,char sncheck,const int snOffset,int fileSize,int nPartSize,char * pFileid)
//{ 
//	/*CFile ff;
//	ff.Open("c:/alog",CFile::modeCreate|CFile::modeReadWrite);
//	ff.Write(m_ErrMSG,m_ErrMSG.GetLength());
//	ff.Close();*/
//	//////////////////////////////////////////////////////////////////////////
//	//文件大小
//	////////////////////////////////////////////////////////////////////////// 
//	//int nOffset=snOffset;
//	//char ncheck=sncheck;
//	char * phead=NULL,*pbody=NULL;
//	char * pFileBuf=NULL;
//	int n_hlen,nbodylen;
//
//	
//	
//	MSG_REQ_SENDPART_BODY m_RQ_SendPart_body; //上传分片包体
//	MSG_REQ_COMMON_HEAD m_RQ_SendPart_head;  //包头
//	
//	MSG_RET_COMMON_HEAD mTrecvHead;
//	//MSG_RET_SENDPART_BODY mTrecvbody;
//
//	DWORD nSize;
//	CFile DatFile; 
//	
//	int nfilelen=0; //文件读取位置
//	//FILE *FataFile=fopen(m_InputFilePath,"rb");
//	if (!DatFile.Open(m_InputFilePath, CFile::modeRead | CFile::shareDenyNone | CFile::typeBinary))
//	{
//		m_ErrMSG.Append("读取文件失败！"); 
//		return FALSE;
//	}
//	bool isRun=true;
//	//至少读一次分片数据 
//	int ii=1; 
//	m_ErrMSG.Append("\n------------\n");
//	
//	//m_ErrMSG.AppendFormat("\nPartSize:%d\n",nPartSize);
//	do
//	{
//		try
//		{ 
//			m_RQ_SendPart_body.clear();
//			// 读入nPartSize多的数据 
//			nSize = (DWORD)DatFile.Read(m_RQ_SendPart_body.pData, nPartSize);
//			//nSize=(DWORD)fread(&pFileBuf,1,nPartSize,FataFile);
//			//m_ErrMSG.AppendFormat("\nData:%s\n",pFileBuf);
//			nfilelen += nSize;
//			//////////////////////////////////////////////////////////////////////////
//			//请求包体
//			//////////////////////////////////////////////////////////////////////////
//			m_RQ_SendPart_body.nOffset=ii; //文件偏移
//			m_RQ_SendPart_body.nPartSize=nSize;  //切片大小
//			strcpy(m_RQ_SendPart_body.strFileID,pFileid);  //文件ID  
//			pbody=m_RQ_SendPart_body.getNetBuf();
//			nbodylen=m_RQ_SendPart_body.getNetBufLen();
//			//if(nPartSize>fileSize  || ii>18)
//			//	m_ErrMSG.AppendFormat("\n%d\n",nSize);
//			//////////////////////////////////////////////////////////////////////////
//			//请求包头
//			//////////////////////////////////////////////////////////////////////////
//			m_RQ_SendPart_head.clear();
//			/*strcpy(m_RQ_SendPart_head.nCheck,GenCheck(pbody,nbodylen);*/
//			m_RQ_SendPart_head.nMsgId=MSGID_SEND_PART; //切片上传
//			//strcpy(m_RQ_SendPart_head.bUserType,'0');
//			strcpy(m_RQ_SendPart_head.sUserId,sUserId); //用户信息
//			m_RQ_SendPart_head.nPkgSize=nbodylen; 
//			phead=m_RQ_SendPart_head.getNetBuf();
//			n_hlen=m_RQ_SendPart_head.getNetBufLen(); 
//
//			//m_ErrMSG.AppendFormat("\nUser:%s\nCheck:%c\nPkgSize:%d\nSize:%d\n",m_RQ_SendPart_head.sUserId,m_RQ_SendPart_head.nCheck,m_RQ_SendPart_head.nPkgSize,nSize);
//			
//			
//			if(!SendBuffer(hSock,phead,n_hlen)){
//				m_ErrMSG.Append("\n上传分片包头失败!");
//				isRun=false;
//				break;
//			}
//			if(!SendBuffer(hSock,pbody,nbodylen)){
//				m_ErrMSG.Append("\n上传分片包体失败!");
//				isRun=false;
//				break;
//			} 
//			//////////////////////////////////////////////////////////////////////////
//			//初始化应答
//			////////////////////////////////////////////////////////////////////////// 
//			char * tbufHead,*tbufBody;
//			tbufHead=(char*)malloc(MSG_RET_COMMON_HEAD_LEN);
//			if(!RecvBuffer(hSock, tbufHead, MSG_RET_COMMON_HEAD_LEN)){
//				m_ErrMSG.Append("\n接受应答包头失败!");
//				isRun=false;
//				break;
//			}
//			mTrecvHead.setNetBuf(tbufHead,MSG_RET_COMMON_HEAD_LEN);  
//			if(!mTrecvHead.bResult){
//				m_ErrMSG.Append("\nbResult!");
//				isRun=false;
//				break;
//			}
//			tbufBody=(char*)malloc(mTrecvHead.nPkgSize);
//			if(!RecvBuffer(hSock, tbufBody, mTrecvHead.nPkgSize)){
//				m_ErrMSG.Append("\n接受应答包体失败!");
//				isRun=false;
//				break;
//			}
//			//mTrecvbody.setNetBuf(tbufBody,mTrecvHead.nPkgSize);  
//			delete []tbufHead;
//			delete []tbufBody;  
//			//delete []pFileBuf;
//			
//			if(nfilelen>=fileSize){ //判断
//				isRun=true;
//				break;
//			}
//		}
//		catch (CFileException *e)
//		{
//			m_ErrMSG.Append(e->m_strFileName);
//			e->Delete();
//			DatFile.Abort();
//			isRun=false;
//			break;
//		} 
//		//空文件
//		if (nSize <= 0){
//			isRun=false;
//			break; 
//		}
//		// 真实数据长度
//		ii++;
//	}while(true);
//	CString fb;
//	fb.Format("\nSendPartCount:%d\n",ii);
//	m_ErrMSG.Append(fb);
//	//fclose(FataFile);
//	DatFile.Close();
//	return isRun;
//}

 /*BOOL CFileActiveXCtrl::SendCompleteFile(SOCKET &hSock,char * pFileId)
{
	char * bufHead,*bufBody;
	int bufHeadLen,bufBodyLen;
	MSG_REQ_COMMON_HEAD head1;
	MSG_REQ_SEND_COMPLETE_BODY body1;
	设置包体
	body1.clear();
	strcpy(body1.strFileID,pFileId);
	编码
	bufBody = body1.getNetBuf();
	bufBodyLen = body1.getNetBufLen();

	设置包头
	head1.clear();
	head1.nPkgSize=bufBodyLen;
	head1.nMsgId = MSGID_SEND_COMPLETE;
	head1.bUserType = '0';
	strcpy(head1.sUserId,sUserId);
	编码
	bufHead = head1.getNetBuf();
	bufHeadLen = head1.getNetBufLen();
	if(!SendBuffer(hSock,bufHead,bufHeadLen))
		return FALSE;
	if(!SendBuffer(hSock,bufBody,bufBodyLen))
		return FALSE;


	char * bufHead2,*bufBody2;
	MSG_RET_COMMON_HEAD head2;
	MSG_RET_SEND_COMPLETE_BODY body2;

	bufHead2=(char*)malloc(MSG_RET_COMMON_HEAD_LEN);
	if(!RecvBuffer(hSock, bufHead2, MSG_RET_COMMON_HEAD_LEN))
		return FALSE;
	head2.setNetBuf(bufHead2,MSG_REQ_COMMON_HEAD_LEN); 

	if(!head2.bResult)
		return FALSE; 
	bufBody2=(char*)malloc(head2.nPkgSize);
	if(!RecvBuffer(hSock, bufBody2, head2.nPkgSize))
		return FALSE;
	body2.setNetBuf(bufBody2,head2.nPkgSize); 
	m_OutPutFileLocation=body2.strFileLocation;
	m_ErrMSG.Append(body2.strFileLocation);
	return TRUE;
}*/
void CFileActiveXCtrl::getIPORPort()
{ 
	char * dd=NULL;
	dd=(char *)malloc(500);
	strcpy(dd,m_InputToken);
	dd[m_InputToken.GetLength()]='\0';
	FileToken ftk(dd);
	m_ConnIP=ftk.getIP();
	m_ConnPort=ftk.getPort();
	delete []dd;
}
void CFileActiveXCtrl::InitParameter()
{
	FileUploadThread* ftu=new FileUploadThread();
	pfileupdatethread=*ftu;
	pfileupdatethread.m_InputFilePath=m_InputFilePath;
	pfileupdatethread.m_InputToken=m_InputToken; 

	m_ErrMSG="";
	m_OutPutFileName=""; 
	m_OutPutUpLdLog="";
	m_OutPutFileSize=0;
	m_OutPutFileLocation="";
	 m_OutPutErrorCode=0;
	 m_OutputProgress=-1;
}
void CFileActiveXCtrl::InitUpload(void)
{  
	AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
	cleanData();
	m_ErrorCode=0;
	InitParameter();
	//FireParameterLoaded();  
	pfileupdatethread.SetICallBack(this);
	pfileupdatethread.Start(); 
}
//void CFileActiveXCtrl::InitUpload(void)
//{ 
//	m_OutputProgress=1;
//	m_ErrMSG="";
//	m_OutPutFileLocation="";
//	m_OutputParameter="";
//	m_OutputProgress=0;
//	/*m_OutputParameter.Append(m_ConnIP); 
//	m_OutputParameter.Append(m_InputToken);
//	m_OutputParameter.Append(m_InputFilePath);*/
//	
//	AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
//	if(m_InputFilePath.IsEmpty())
//	{
//		m_ErrMSG="请输入有效的文件路径！";
//		goto cleanup;
//	} 
//	getIPORPort();
//	g_bConnected=false; 
//	int nOffset=0; //文件偏移
//	int nOrgSize=0; //文件大小
//	struct _stat insize;
//	_stat(m_InputFilePath,&insize);
//	nOrgSize=insize.st_size; 
//
//	//初始化应答声明
//	char * tbufHead,*tbufBody;
//	int tbufHeadLen,tbufBodyLen;
//	MSG_RET_COMMON_HEAD thead1;
//	MSG_RET_INIT_BODY tbody1;
//	//////////////////////////////////////////////////////////////////////////
//	// 初始化连接
//	//////////////////////////////////////////////////////////////////////////
//	
//	SOCKET client=NULL;  
//	m_OutputProgress=2;
//	//if(!InitSocket(client,m_ConnIP,m_ConnPort))
//	if (!Connect(client, m_ConnIP, (short)m_ConnPort))
//	{
//		m_ErrMSG="连接初始化失败！";
//		goto cleanup;
//	}
//	m_OutputProgress=3;
//	if(!SendInit(client,nOrgSize))
//	{
//		m_ErrMSG.Append("发送初始化失败！"); 
//		goto cleanup;
//	} 
//	//m_ErrMSG.Append("初始化成功！");
//	
//	m_OutputProgress=10;
//	//////////////////////////////////////////////////////////////////////////
//	//初始化应答
//	////////////////////////////////////////////////////////////////////////// 
//	tbufHead=(char*)malloc(MSG_RET_COMMON_HEAD_LEN);
//	if(!RecvBuffer(client, tbufHead, MSG_RET_COMMON_HEAD_LEN))
//		goto cleanup;
//
//	
//	
//	thead1.setNetBuf(tbufHead,MSG_REQ_COMMON_HEAD_LEN);  
//	tbufBody=(char*)malloc(thead1.nPkgSize);
//	m_OutputProgress=15;
//	if(!thead1.bResult)
//	{	
//		m_ErrMSG.Append("初始化验证失败！"); 
//		goto cleanup;
//	} 
//	if(!RecvBuffer(client, tbufBody, thead1.nPkgSize))
//	{
//		m_ErrMSG.Append("接受初始化包体失败！"); 
//		goto cleanup;
//	}
//	m_OutputProgress=20;
//	tbody1.setNetBuf(tbufBody,thead1.getNetBufLen());  
//	strcpy(sUserId,thead1.sUserId); 
//	 
//	m_ErrMSG.Format("\nFileSize:%d\nPartSize:%d\noffset:%d\nnCheck:%c\nUserID:",nOrgSize,tbody1.nPartSize,nOffset,thead1.nCheck);
//	m_ErrMSG.Append(thead1.sUserId); 
//	m_ErrMSG.Append("\nFileId:"); 
//	m_ErrMSG.Append(tbody1.strFileID); 
//		
//	//////////////////////////////////////////////////////////////////////////
//	//文件切片上传
//	////////////////////////////////////////////////////////////////////////// 
//	if(!SendPacketGetpart(client,thead1.nCheck,nOffset,nOrgSize,tbody1.nPartSize,tbody1.strFileID))
//	{
//		m_ErrMSG.Append("\n发送数据块请求失败");
//		goto cleanup;
//	}  
//	m_OutputProgress=98;
//	//////////////////////////////////////////////////////////////////////////
//	//文件上传完成请求
//	//////////////////////////////////////////////////////////////////////////
//	if(!SendCompleteFile(client,tbody1.strFileID)) {
//		m_ErrMSG.Append("\n文件上传完成请求失败！");
//		goto cleanup;
//	}
//	m_OutputProgress=100;
//	//m_OutputParameter=pReqGetpartBody->pData;  
//cleanup:
//	// 关闭连接
//	if (client != INVALID_SOCKET)
//	{
//		closesocket(client);
//		client = INVALID_SOCKET;
//	}
//	// Fire an event to notify web page
//	FireParameterLoaded();
//	
//	/*CString strOnLoaded("OnLoaded");
//	this->CallJScript(strOnLoaded);*/
//} 

void CFileActiveXCtrl::OnConnIPChanged(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: Add your property handler code here

	SetModifiedFlag();
}

void CFileActiveXCtrl::OnConnPortChanged(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: Add your property handler code here

	SetModifiedFlag();
}

void CFileActiveXCtrl::OnErrMSGChanged(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: Add your property handler code here

	SetModifiedFlag();
}


void CFileActiveXCtrl::OnOutputProgressChanged(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState()); 

	// TODO: Add your property handler code here

	SetModifiedFlag();
}

void CFileActiveXCtrl::OnInputTokenChanged(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: Add your property handler code here

	SetModifiedFlag();
}

BSTR CFileActiveXCtrl::OnOutputOpenDialog(LPCTSTR nFileName, LPCTSTR nFileStream)
{
	m_OutputProgress=-1;
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CString strResult;  
	CFileDialog MyDlg(TRUE, NULL,nFileStream,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, nFileStream, NULL );
	if (MyDlg.DoModal()==IDOK){
		strResult=MyDlg.GetPathName();
		m_OutPutShowFileName=MyDlg.GetFileName();
		//strResult.Append(MyDlg.GetFileName());
	}
/*
	CFileDialog DatFile;	

	if(!DatFile.Open(nFileName,CFile::modeCreate | CFile::modeReadWrite))
	{
		strResult ="打开或创建文件失败！";
	}
	try
	{
		DatFile.Write(nFileStream,sizeof(nFileStream));
		DatFile.Flush();
		DatFile.Close();
	}
	catch (CFileException* e)
	{
		e->Delete();
		DatFile.Abort();
		strResult="写文件失败"; 		
	}*/
 
	
	// TODO: Add your dispatch handler code here

	return strResult.AllocSysString();
}


void CFileActiveXCtrl::OnOutPutFileLocationChanged(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 在此添加属性处理程序代码

	SetModifiedFlag();
}


void CFileActiveXCtrl::OnOutPutFileNameChanged(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 在此添加属性处理程序代码

	SetModifiedFlag();
}


void CFileActiveXCtrl::OnOutPutUpLdLogChanged(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 在此添加属性处理程序代码

	SetModifiedFlag();
}


void CFileActiveXCtrl::OnOutPutFileSizeChanged(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 在此添加属性处理程序代码

	SetModifiedFlag();
}


void CFileActiveXCtrl::OnOutPutShowFileNameChanged(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 在此添加属性处理程序代码

	SetModifiedFlag();
}


void CFileActiveXCtrl::OnOutPutErrorCodeChanged(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 在此添加属性处理程序代码

	SetModifiedFlag();
}


LONG CFileActiveXCtrl::OnCanelupLoad(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
	// TODO: 在此添加调度处理程序代码
	pfileupdatethread.SendCanel(); 
	//OnCanel();
	return 0;
}
