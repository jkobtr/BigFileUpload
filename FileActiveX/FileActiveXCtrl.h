#pragma once
#include "UpLoadFileDialog.h"
#include "../Common/Util.h"
#include "../Common/InfoStruct.h"
#include "FileUploadThread.h"
#include "ICallBack.h"

#include <MsHTML.h>
// FileActiveXCtrl.h : Declaration of the CFileActiveXCtrl ActiveX Control class.


// CFileActiveXCtrl : See FileActiveXCtrl.cpp for implementation.
bool g_bConnected ;									// 已连接
HANDLE hStopEvent = NULL;							// 服务要停止时会被通知
class CFileActiveXCtrl : public COleControl,public ICallBack
{
	
	DECLARE_DYNCREATE(CFileActiveXCtrl)

// Constructor
public:
	CFileActiveXCtrl();

// Overrides
public:
	virtual void OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void DoPropExchange(CPropExchange* pPX);
	virtual void OnResetState();
	virtual void OnSetClientSite();
	virtual DWORD GetControlFlags();
	virtual void OnClose(DWORD dwSaveOption);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	IWebBrowser2* pWebBrowser; //IE浏览器
	IHTMLDocument2* pHTMLDocument; //包含此控件的web页面

// Implementation
protected:
	~CFileActiveXCtrl();

	DECLARE_OLECREATE_EX(CFileActiveXCtrl)    // Class factory and guid
	DECLARE_OLETYPELIB(CFileActiveXCtrl)      // GetTypeInfo
	DECLARE_PROPPAGEIDS(CFileActiveXCtrl)     // Property page IDs
	DECLARE_OLECTLTYPE(CFileActiveXCtrl)		// Type name and misc status

	// Subclassed control support
	BOOL IsSubclassedControl();
	LRESULT OnOcmCommand(WPARAM wParam, LPARAM lParam);

// Message maps
	DECLARE_MESSAGE_MAP()

// Dispatch maps
	DECLARE_DISPATCH_MAP()

// Event maps
	DECLARE_EVENT_MAP()

// Dispatch and event IDs
public:
	enum {
		dispidOnCanelupLoad = 17L,
		dispidOutPutErrorCode = 16,
		dispidOutPutShowFileName = 15,
		dispidOutPutFileSize = 14,
		dispidOutPutUpLdLog = 13,
		dispidOutPutFileName = 12,
		dispidOutPutFileLocation = 11,
		dispidOpenDialog = 10L,
		dispidInputToken = 9,
		dispidOutputProgress = 8, 
		dispidOutputErrMSG = 7,
		dispidInputConnPort = 6,
		dispidInputConnIP = 5, 
		eventidParameterLoaded = 1L,
		dispidLoadParameter = 3L,
		dispidOutputParameter = 2,
		dispidInputFilePath = 1
	};
public:
	//UpLoadFileDialog m_MainDialog;
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	void OnSuccesful();
	void OnFailed();
	void OnCanel();
	void OnProgress(int progressvalue);
	bool GetJScript(CComPtr<IDispatch>& spDisp);
	bool GetJScripts(CComPtr<IHTMLElementCollection>& spColl);

	bool CallJScript(const CString strFunc,CComVariant* pVarResult = NULL);
	bool CallJScript(const CString strFunc,const CString strArg1,CComVariant* pVarResult = NULL);
	bool CallJScript(const CString strFunc,const CString strArg1,const CString strArg2,CComVariant* pVarResult = NULL);
	bool CallJScript(const CString strFunc,const CString strArg1,const CString strArg2,const CString strArg3,CComVariant* pVarResult = NULL);
	bool CallJScript(const CString strFunc,const CStringArray& paramArray,CComVariant* pVarResult = NULL);
protected:
	
	void cleanData();
	void OnInputParameterChanged(void);
	
	void OnOutputParameterChanged(void); 
	//bool InitSocket(SOCKET &client,const char *ipaddr,const int port);	
	/*BOOL SendPacketGetpart(SOCKET &hSock,char sncheck,const int nOffset,int nfileSize,int nPartSize,char * pFileid);
	BOOL SendInit(SOCKET &hSock,int nfileSize);
	BOOL SendCompleteFile(SOCKET &hSock,char * pFileId);*/
	void InitParameter();
	void getIPORPort();
	CString m_OutputParameter; 
	void InitUpload(void);

	void FireParameterLoaded(void)
	{
		FireEvent(eventidParameterLoaded, EVENT_PARAM(VTS_NONE));
	}  
	
	CString m_InputFilePath;                     //文件的本地存放路径
	CString m_ConnIP;                            //ip
	LONG m_ConnPort;                             //端口
	CString m_ErrMSG;                            //错误信息
	LONG m_OutputProgress;                       //传输进度
	CString m_InputToken;                        //输入令牌
	CString m_OutPutFileLocation;                //文件服务器保持路径 
	FileUploadThread pfileupdatethread;
	CString m_OutPutFileName;
	CString m_OutPutUpLdLog;
	ULONG m_OutPutFileSize;
	CString m_OutPutShowFileName;               //原始文件名
	LONG m_OutPutErrorCode;
	char sUserId[MAX_USERNAME_LEN+1];	        //用户ID 

	void OnConnPortChanged(void); 
	void OnConnIPChanged(void); 
	void OnErrMSGChanged(void);  
	void OnOutputProgressChanged(void); 
	void OnInputTokenChanged(void); 
	BSTR OnOutputOpenDialog(LPCTSTR nFileName, LPCTSTR nFileStream);
	void OnOutPutFileLocationChanged(void);
	
	void OnOutPutFileNameChanged(void);
	
	void OnOutPutUpLdLogChanged(void);
	
	void OnOutPutFileSizeChanged(void);
	
	void OnOutPutShowFileNameChanged(void);
	
	void OnOutPutErrorCodeChanged(void);
	
	LONG OnCanelupLoad(void);
};

