#pragma once

#include<stdio.h>
#include<iostream> 
#include <sys/stat.h>
#include<string.h>
#include "../Common/Util.h"
#include "../Common/InfoStruct.h"
#include "../Common/md5.h" 
#include "../Common/InfoStruct.h"
#include "../Common/FileToken.h"
#include "../Common/AppConf.h" 
#include "../Common/zlib.h"
#include "../Common/netcrypt.h"
#include "Thread.h"
#define MSG_STATE_INIT             1
#define MSG_STATE_INITCONT         2
#define MSG_STATE_SENDPARTDATA     3
#define MSG_STATE_SENDCOMPTELE     4

#pragma comment(lib,"zlib.lib")

class FileUploadThread:public CThread{
public:
	FileUploadThread();
	~FileUploadThread();
	bool SendCanel();              //取消上传
	CString m_InputFilePath;       //文件的本地存放路径
	CString m_ConnIP;              //ip
	LONG m_ConnPort;               //端口
	CString m_ErrMSG;              //错误信息
	CString m_FileName;            //文件保存路径
	CString m_InputToken;          //输入令牌
	CString m_OutPutFileLocation;  //文件服务器保持路径 
	CString m_SendLog;             //日志记录
	int m_nOffset;                 //记录文件位移位置
	long m_fileSize;
	int m_contprogress;
	int m_currenProgress;
	int m_fileSeek;
	SOCKET client;      ///
	bool m_state;                  //运行状态
	bool m_isEncrypt;                //是否加密
	bool m_isCompress;			   //是否压缩
	CHAR CERINFOPATH[MAX_PATH];     //临时x509数字证书路径 
	char SECERTKEY[MAX_SECERTKEY_LEN+1];
private: 
	double  m_MD5duration; //计算MD5花费时间
	bool m_inCanel;                //是否取消
	int m_MsgState;
	CString m_fileMd5;
	BOOL SendPacketGetpart(const int nOffset,int nfileSize,int nPartSize,char * pFileid);
	BOOL SendPacketGetpartCont(int nfileSize,int nPartSize,char * pFileid);
	BOOL Send_Init_ShakeHands();
	BOOL SendInit(int nfileSize);
	BOOL ReConnectSend(int nfileSize,int nPartSize,char * pFileid);
	BOOL SendInitCont(int nfileSize);
	BOOL SendCompleteFile(char * pFileId);
	void Progress(int nprogr);
	BOOL RecvError(long nlength);
	BOOL isHellError(int errid);
	BOOL RecvPacket(char *pBuf);
	void getIPORPort();
	CString m_OutputParameter; 
	bool InitUpload(void);
	void LogFile(const char * logmsg,int logstrlen);
	char sUserId[MAX_USERNAME_LEN+1];	//用户ID
	DWORD ThreadMethod(); 
	bool CloseSocket();               //上传完成之后关闭SOCKET连接
	bool SendFailed(char * pFileId);
};