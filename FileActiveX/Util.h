#pragma once

#include <io.h>
#include <winsock2.h>

// Windows�汾��
// Windows 2000   0x0500
// Windows XP     0x0501
// Windows Vista  0x0600
// Windows 7      0x0601
//#define _WIN32_WINNT=0x0500

// SOCKET������ʱ(ms)
#define LISTEN_TIMEOUT					100				// �������ӳ�ʱ
#define SEND_TIMEOUT					10000			// �������ݰ���ʱ
#define RECV_TIMEOUT					10000			// �������ݰ���ʱ

#define ERROR_RECV_OK					0				// �������ݰ��ɹ�
#define ERROR_RECV_PACKAGE_HEADER		-1				// ���ݰ�ͷ���մ���
#define ERROR_RECV_PACKAGE_BODY			-2				// ���ݰ�����մ���
#define ERROR_CONNECTION_LOST			-3				// ���ݰ����մ���
#define ERROR_PACKAGE_FORMAT			-4				// ���ݰ���ʽ����ȷ

#define SPLIT_MAXLEN					64				// �ָ��ַ��������Ƭ�γ���
#define TAG_LEN							128				// ���ݱ�ǳ���

 
//...													// ����(������Ȩ)
#define INFO_ITEM_ICID					21				// ���ܿ���
#define INFO_ITEM_CRC					22				// ����У��
#define INFO_ITEM_MAC					23				// MAC��ַ
#define INFO_ITEM_KEY					24				// ���ܺ����Կ
#define INFO_ITEM_KEYDATE				25				// ��Կʱ���(YYYYMMDD)
#define INFO_ITEM_ENCORGSIZE			26				// ����ǰ�ĳ���
#define INFO_ITEM_MIP					27				// ת���鲥��ַ
#define INFO_ITEM_MPORT					28				// ת���鲥�˿�
#define INFO_ITEM_END					0xFE			// ��������
#define INFO_ITEM_EOF					0xFF			// �����б�
#define MAX_ATTRLIST_SIZE				500				// ͨ�ſ��������б���󳤶� 

#define MSG_BUFSIZE	(100*1024)	// ��Ϣ����С(�ļ���ƬΪ64KB,�����ʵ�����) 
#define TRANS_BLOCK_SIZE				4096			// �����Ƭ��С
#define MAX_FILENAME_LEN				64				// �ļ�����󳤶�

#define MSGID_INIT						1				// ��ʼ��¼
#define MSGID_REINIT					2				// ������ʼ��
#define MSGID_PUTPART					3				// ��Ƭ�ϴ�
#define MSGID_FIN						4				// �ϴ����


#pragma pack(1)
// �����ͷ��ʽ
typedef struct 
{
	DWORD nPkgSize;										// ���Ĵ�С(��Ϣ���С)
	WORD nMsgId;										// ��Ϣ����
	DWORD nSessionId;										// ������
	BYTE nCheck;										// У��(��Ϣͷ���ֽ�XOR)
}MSG_REQ_COMMON_HEAD;


// Ӧ���ͷ��ʽ
typedef struct 
{
	DWORD nPkgSize;										// ���Ĵ�С(��Ϣ���С)
	WORD nMsgId;										// ��Ϣ����
	BYTE bResult;										// �����Ƿ�ɹ�(��ʧ�ܣ���̵�Ϊ������Ϣ��)
	DWORD nSessionId;										// ������
	BYTE nCheck;										// У��(��Ϣͷ���ֽ�XOR)
}MSG_RET_COMMON_HEAD;

//Ӧ���Ƭ
typedef struct
{
	char strFileName[MAX_FILENAME_LEN+1];				// �ļ�����
	DWORD nUpdateTime;									// �ļ�����ʱ��
	DWORD nOrgSize;										// �ļ���С
	DWORD nZipSize;										// ѹ����С
	DWORD nDataSize;									// ���ݿ��С
	DWORD nOffset;										// ��Ƭƫ��
	DWORD nPartSize;									// ��Ƭ��С 
} MSG_RET_GETPART_BODY;


/////////////////////////////
// �ϴ���Ƭ
typedef struct
{
	char nSessionID[MAX_FILENAME_LEN+1];				// �����SessionID [Ψһ��ʶ]
	char strMd5[16];                                    //MD5
	DWORD nOrgSize;										// �ļ���С
	int nOffset;										// ��Ƭƫ��
	DWORD nPartSize;									// ��Ƭ��С
	char pData[0];										// ��Ƭ����(�䳤)
} MSG_REQ_PUTPART_BODY;
#pragma pack()

// ��ʼ������
bool InitSocket(SOCKET &sockListen, char strInterface[], short nPort);

// ��������
bool AcceptSocket(SOCKET &sockListen, SOCKET &sockClient, DWORD &nIP, long nTimeout = LISTEN_TIMEOUT);


// ���ӷ�����/�Ͽ�����
bool Connect(SOCKET &sock, const char strHost[], short nPort);
bool ConnectUseIP(SOCKET &sock, const char strIP[], short nPort);
void Disconnect(SOCKET &sock);

// �ж��Ƿ�������
bool IsDataAvailable(SOCKET sock, int nTimeOut);

// �շ�����
typedef struct
{
	// ������
	int nTaskNum;			// �ļ���Ŀ

	// �ۼ�������
	int nTaskTotal;			// ��������
	int nTaskTrans;			// �ѷ��͡�����������

	// ĳ���ļ���������
	int nTotal;				// ��������
	int nOffset;			// ����ƫ����(HTTPͷ��С)
	int nTrans;				// �ѷ��͡�����������

	// �շ�״̬
	bool bFinished;			// �����߳���ֹͣ
	bool bCancel;			// �û����ȡ��
	bool bFail;				// �շ�ʧ��
} COMM_STATUS;

// �շ�����
bool SendBuffer(SOCKET sock, char *pBuffer, long nLength, long nTimeout = SEND_TIMEOUT, int *pCommStatus /*�ļ���ȡ��λ�� */=0);
bool RecvBuffer(SOCKET sock, char *pBuffer, long nLength, long nTimeout = RECV_TIMEOUT);  
int LoadData(LPCTSTR strFileName, char *pFileBuf, DWORD &nFileLen, bool bFull, int nOffset);