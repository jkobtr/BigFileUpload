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
	bool SendCanel();              //ȡ���ϴ�
	CString m_InputFilePath;       //�ļ��ı��ش��·��
	CString m_ConnIP;              //ip
	LONG m_ConnPort;               //�˿�
	CString m_ErrMSG;              //������Ϣ
	CString m_FileName;            //�ļ�����·��
	CString m_InputToken;          //��������
	CString m_OutPutFileLocation;  //�ļ�����������·�� 
	CString m_SendLog;             //��־��¼
	int m_nOffset;                 //��¼�ļ�λ��λ��
	long m_fileSize;
	int m_contprogress;
	int m_currenProgress;
	int m_fileSeek;
	SOCKET client;      ///
	bool m_state;                  //����״̬
	bool m_isEncrypt;                //�Ƿ����
	bool m_isCompress;			   //�Ƿ�ѹ��
	CHAR CERINFOPATH[MAX_PATH];     //��ʱx509����֤��·�� 
	char SECERTKEY[MAX_SECERTKEY_LEN+1];
private: 
	double  m_MD5duration; //����MD5����ʱ��
	bool m_inCanel;                //�Ƿ�ȡ��
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
	char sUserId[MAX_USERNAME_LEN+1];	//�û�ID
	DWORD ThreadMethod(); 
	bool CloseSocket();               //�ϴ����֮��ر�SOCKET����
	bool SendFailed(char * pFileId);
};